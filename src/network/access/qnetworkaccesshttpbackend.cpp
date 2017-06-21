
#ifdef SSLAB__DEBUG
#ifndef SSLAB__QDATETIME
#define SSLAB__QDATETIME
#include <QDateTime>
#endif
#endif

#ifdef SSLAB__DEBUG
#ifndef SSLAB__QTHREAD
#define SSLAB__QTHREAD
#include <QThread>
#endif
#endif
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QNETWORKACCESSHTTPBACKEND_DEBUG
#define CACHESD_POSTFIX QLatin1String(".sd")

#include "qnetworkaccesshttpbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkaccesscache_p.h"
#include "qabstractnetworkcache.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkrequest_p.h"
#include "qnetworkcookie_p.h"
#include "QtCore/qdatetime.h"
#include "QtCore/qelapsedtimer.h"
#include "QtNetwork/qsslconfiguration.h"

#include <qfile.h>

#ifndef QT_NO_HTTP

#include <string.h>             // for strchr

#ifndef SSLAB_SR	//sslab.sr
#define SSLAB_SR
#endif
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>

QT_BEGIN_NAMESPACE

enum {
    DefaultHttpPort = 80,
    DefaultHttpsPort = 443
};

class QNetworkProxy;

static QByteArray makeCacheKey(QNetworkAccessHttpBackend *backend, QNetworkProxy *proxy)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QByteArray result;
    QUrl copy = backend->url();
    bool isEncrypted = copy.scheme().toLower() == QLatin1String("https");
    copy.setPort(copy.port(isEncrypted ? DefaultHttpsPort : DefaultHttpPort));
    result = copy.toEncoded(QUrl::RemovePassword | QUrl::RemovePath |
                            QUrl::RemoveQuery | QUrl::RemoveFragment);

#ifndef QT_NO_NETWORKPROXY
    if (proxy->type() != QNetworkProxy::NoProxy) {
        QUrl key;

        switch (proxy->type()) {
        case QNetworkProxy::Socks5Proxy:
            key.setScheme(QLatin1String("proxy-socks5"));
            break;

        case QNetworkProxy::HttpProxy:
        case QNetworkProxy::HttpCachingProxy:
            key.setScheme(QLatin1String("proxy-http"));
            break;

        default:
            break;
        }

        if (!key.scheme().isEmpty()) {
            key.setUserName(proxy->user());
            key.setHost(proxy->hostName());
            key.setPort(proxy->port());
            key.setEncodedQuery(result);
            result = key.toEncoded();
        }
    }
#endif

    return "http-connection:" + result;
}

static inline bool isSeparator(register char c)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    static const char separators[] = "()<>@,;:\\\"/[]?={}";
    return isLWS(c) || strchr(separators, c) != 0;
}

// ### merge with nextField in cookiejar.cpp
static QHash<QByteArray, QByteArray> parseHttpOptionHeader(const QByteArray &header)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // The HTTP header is of the form:
    // header          = #1(directives)
    // directives      = token | value-directive
    // value-directive = token "=" (token | quoted-string)
    QHash<QByteArray, QByteArray> result;

    int pos = 0;
    while (true) {
        // skip spaces
        pos = nextNonWhitespace(header, pos);
        if (pos == header.length())
            return result;      // end of parsing

        // pos points to a non-whitespace
        int comma = header.indexOf(',', pos);
        int equal = header.indexOf('=', pos);
        if (comma == pos || equal == pos)
            // huh? Broken header.
            return result;

        // The key name is delimited by either a comma, an equal sign or the end
        // of the header, whichever comes first
        int end = comma;
        if (end == -1)
            end = header.length();
        if (equal != -1 && end > equal)
            end = equal;        // equal sign comes before comma/end
        QByteArray key = QByteArray(header.constData() + pos, end - pos).trimmed().toLower();
        pos = end + 1;

        if (uint(equal) < uint(comma)) {
            // case: token "=" (token | quoted-string)
            // skip spaces
            pos = nextNonWhitespace(header, pos);
            if (pos == header.length())
                // huh? Broken header
                return result;

            QByteArray value;
            value.reserve(header.length() - pos);
            if (header.at(pos) == '"') {
                // case: quoted-string
                // quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
                // qdtext         = <any TEXT except <">>
                // quoted-pair    = "\" CHAR
                ++pos;
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (c == '"') {
                        // end of quoted text
                        break;
                    } else if (c == '\\') {
                        ++pos;
                        if (pos >= header.length())
                            // broken header
                            return result;
                        c = header.at(pos);
                    }

                    value += c;
                    ++pos;
                }
            } else {
                // case: token
                while (pos < header.length()) {
                    register char c = header.at(pos);
                    if (isSeparator(c))
                        break;
                    value += c;
                    ++pos;
                }
            }

            result.insert(key, value);

            // find the comma now:
            comma = header.indexOf(',', pos);
            if (comma == -1)
                return result;  // end of parsing
            pos = comma + 1;
        } else {
            // case: token
            // key is already set
            result.insert(key, QByteArray());
        }
    }
}

QNetworkAccessBackend *
QNetworkAccessHttpBackendFactory::create(QNetworkAccessManager::Operation op,
                                         const QNetworkRequest &request) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // check the operation
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PostOperation:
    case QNetworkAccessManager::HeadOperation:
    case QNetworkAccessManager::PutOperation:
    case QNetworkAccessManager::DeleteOperation:
    case QNetworkAccessManager::CustomOperation:
        break;

    default:
        // no, we can't handle this request
        return 0;
    }

    QUrl url = request.url();
    QString scheme = url.scheme().toLower();
    if (scheme == QLatin1String("http") || scheme == QLatin1String("https"))
        return new QNetworkAccessHttpBackend;

    return 0;
}

static QNetworkReply::NetworkError statusCodeFromHttp(int httpStatusCode, const QUrl &url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QNetworkReply::NetworkError code;
    // we've got an error
    switch (httpStatusCode) {
    case 401:               // Authorization required
        code = QNetworkReply::AuthenticationRequiredError;
        break;

    case 403:               // Access denied
        code = QNetworkReply::ContentOperationNotPermittedError;
        break;

    case 404:               // Not Found
        code = QNetworkReply::ContentNotFoundError;
        break;

    case 405:               // Method Not Allowed
        code = QNetworkReply::ContentOperationNotPermittedError;
        break;

    case 407:
        code = QNetworkReply::ProxyAuthenticationRequiredError;
        break;

    default:
        if (httpStatusCode > 500) {
            // some kind of server error
            code = QNetworkReply::ProtocolUnknownError;
        } else if (httpStatusCode >= 400) {
            // content error we did not handle above
            code = QNetworkReply::UnknownContentError;
        } else {
            qWarning("QNetworkAccess: got HTTP status code %d which is not expected from url: \"%s\"",
                     httpStatusCode, qPrintable(url.toString()));
            code = QNetworkReply::ProtocolFailure;
        }
    }

    return code;
}

class QNetworkAccessCachedHttpConnection: public QHttpNetworkConnection,
                                      public QNetworkAccessCache::CacheableObject
{
	// Q_OBJECT
public:
    QNetworkAccessCachedHttpConnection(const QString &hostName, quint16 port, bool encrypt)
        : QHttpNetworkConnection(hostName, port, encrypt)
    {
        setExpires(true);
        setShareable(true);
    }

    virtual void dispose()
    {
#if 0  // sample code; do this right with the API
        Q_ASSERT(!isWorking());
#endif
        delete this;
    }
};

QNetworkAccessHttpBackend::QNetworkAccessHttpBackend()
    : QNetworkAccessBackend(), httpReply(0), http(0), uploadDevice(0)
#ifndef QT_NO_OPENSSL
    , pendingSslConfiguration(0), pendingIgnoreAllSslErrors(false)
#endif
    , resumeOffset(0)
{
}

QNetworkAccessHttpBackend::~QNetworkAccessHttpBackend()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (http)
        disconnectFromHttp();
#ifndef QT_NO_OPENSSL
    delete pendingSslConfiguration;
#endif

	 /*sslab cache*/
/*	 qWarning("[cache1Count]:%f",getCache1Count());
	 qWarning("[cache2Count]:%f",getCache2Count());
	 qWarning("[requestCount]:%f",getRequestCount());
	 qWarning("[cache1HitRatio] %f%%",cache1HitRatio()*100);
	 qWarning("[cache2HitRatio] %f%%",cache2HitRatio()*100);*/
//	 qWarning("[totalCacheTime] %lld(ms)",getCacheTime());
//	 qWarning("[totalResourceLoadTime] %lld(ms)",getResourceLoadTime());
//	 qWarning("[averageResourceLoadTime] %f(ms)",((double)getResourceLoadTime()) / getRequestCount());
//	 qWarning("[avg.cacheResourceLoadTime] %f(ms)", ((double)getFromCacheResourceLoadTime()) / (getCache1Count() + getCache2Count())); 

}

void QNetworkAccessHttpBackend::disconnectFromHttp()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (http) {
        // This is abut disconnecting signals, not about disconnecting TCP connections
        disconnect(http, 0, this, 0);

        // Get the object cache that stores our QHttpNetworkConnection objects
        QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getObjectCache(this);

        // synchronous calls are not put into the cache, so for them the key is empty
        if (!cacheKey.isEmpty())
            cache->releaseEntry(cacheKey);
    }

    // This is abut disconnecting signals, not about disconnecting TCP connections
    if (httpReply)
        disconnect(httpReply, 0, this, 0);

    http = 0;
    httpReply = 0;
    cacheKey.clear();
}

void QNetworkAccessHttpBackend::finished()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (http)
        disconnectFromHttp();
    // call parent
    QNetworkAccessBackend::finished();
}

#ifdef SSLAB_SR
void QNetworkAccessHttpBackend::validateCache(QHttpNetworkRequest &httpRequest, bool &loadedFromCache, const bool estimatingTime, const int sr_flag)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QNetworkRequest::CacheLoadControl CacheLoadControlAttribute =
        (QNetworkRequest::CacheLoadControl)request().attribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork).toInt();

	 incRequestCount();

	 if (CacheLoadControlAttribute == QNetworkRequest::AlwaysNetwork) {
        // forced reload from the network
        // tell any caching proxy servers to reload too
        httpRequest.setHeaderField("Cache-Control", "no-cache");
        httpRequest.setHeaderField("Pragma", "no-cache");
        return;
    }

    // The disk cache API does not currently support partial content retrieval.
    // That is why we don't use the disk cache for any such requests.
    if (request().hasRawHeader("Range"))
        return;

	 /*[sslab cache]*/

	 QAbstractNetworkCache *nc = networkTopCache();
	 QNetworkCacheMetaData metaData;
	 if (nc) {
		 metaData = nc->metaData(url());
		 if (!metaData.isValid()) {
			nc = networkCache();
			if (!nc) return;
			metaData = nc->metaData(url());
			if (!metaData.isValid())
				return;
			incCache2Count();
		 }
		 else incCache1Count();
		 reorderFile(nc);
	 }
	 else {
		 nc = networkCache();
		 if (!nc) return;
		 metaData = nc->metaData(url());
		 if (!metaData.isValid())
			 return;
		 incCache2Count();
		 QString saveFileName = nc->_cacheFileName(url());
		 qWarning("[onHit]from:%s",saveFileName.toAscii().data());
	 }

    if (!metaData.saveToDisk())
        return;
	 
	/*[sslab cache]*/
	// if hit, if in ntc => move to ntc.mru
	//   "   , else in nc => copy to ntc.mru(migrate) & remove

    QNetworkHeadersPrivate cacheHeaders;
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;
    cacheHeaders.setAllRawHeaders(metaData.rawHeaders());

    it = cacheHeaders.findRawHeader("etag");
    if (it != cacheHeaders.rawHeaders.constEnd())
        httpRequest.setHeaderField("If-None-Match", it->second);

    QDateTime lastModified = metaData.lastModified();
    if (lastModified.isValid())
        httpRequest.setHeaderField("If-Modified-Since", QNetworkHeadersPrivate::toHttpDate(lastModified));

    if (CacheLoadControlAttribute == QNetworkRequest::PreferNetwork) {
        it = cacheHeaders.findRawHeader("Cache-Control");
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            QHash<QByteArray, QByteArray> cacheControl = parseHttpOptionHeader(it->second);
            if (cacheControl.contains("must-revalidate"))
                return;
        }
    }

	if (sr_flag == 1 && estimatingTime) {
		qWarning("[SR-test] hit, sr_flag=%d, estimatingTime=%d",sr_flag, estimatingTime);
		int seg_flag = shmget(1111, PAGE_SIZE, 0666);
		int* shdflag = (int*)shmat(seg_flag, NULL, 0);
		*shdflag = 2;
		qWarning("[SR-test] sr_flag was set to %d", *shdflag);
		shmdt(shdflag);
	}

	QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime expirationDate = metaData.expirationDate();

	bool response_is_fresh = currentDateTime.secsTo(expirationDate) >= 0;

    if (!response_is_fresh)
        return;

    loadedFromCache = true;
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "response_is_fresh" << CacheLoadControlAttribute;
#endif

	 /* Hit count(smkim2015) */
	int seg_cacheHitCnt = shmget(5555, PAGE_SIZE, 0666);	 
	int* shdcacheHitCnt = (int*)shmat(seg_cacheHitCnt, NULL, 0);
	(*shdcacheHitCnt)++;
	shmdt(shdcacheHitCnt);

	 if (!sendCacheContents(metaData))
        loadedFromCache = false;
}
#endif


/*
    For a given httpRequest
    1) If AlwaysNetwork, return
    2) If we have a cache entry for this url populate headers so the server can return 304
    3) Calculate if response_is_fresh and if so send the cache and set loadedFromCache to true
 */
void QNetworkAccessHttpBackend::validateCache(QHttpNetworkRequest &httpRequest, bool &loadedFromCache)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QNetworkRequest::CacheLoadControl CacheLoadControlAttribute =
        (QNetworkRequest::CacheLoadControl)request().attribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork).toInt();

	 incRequestCount();

	 if (CacheLoadControlAttribute == QNetworkRequest::AlwaysNetwork) {
        // forced reload from the network
        // tell any caching proxy servers to reload too
        httpRequest.setHeaderField("Cache-Control", "no-cache");
        httpRequest.setHeaderField("Pragma", "no-cache");
        return;
    }

    // The disk cache API does not currently support partial content retrieval.
    // That is why we don't use the disk cache for any such requests.
    if (request().hasRawHeader("Range"))
        return;

	 /*[sslab cache]*/

	 QAbstractNetworkCache *nc = networkTopCache();
	 QNetworkCacheMetaData metaData;
	 if (nc) {
		 metaData = nc->metaData(url());
		 if (!metaData.isValid()) {
			nc = networkCache();
			if (!nc) return;
			metaData = nc->metaData(url());
			if (!metaData.isValid())
				return;
			incCache2Count();
		 }
		 else incCache1Count();
		 reorderFile(nc);
	 }
	 else {
		 nc = networkCache();
		 if (!nc) return;
		 metaData = nc->metaData(url());
		 if (!metaData.isValid())
			 return;
		 incCache2Count();
		 QString saveFileName = nc->_cacheFileName(url());
		 qWarning("[onHit]from:%s",saveFileName.toAscii().data());
	 }

    if (!metaData.saveToDisk())
        return;
	 
/*[sslab cache]*/
	 // if hit, if in ntc => move to ntc.mru
	 //   "   , else in nc => copy to ntc.mru(migrate) & remove

    QNetworkHeadersPrivate cacheHeaders;
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;
    cacheHeaders.setAllRawHeaders(metaData.rawHeaders());

    it = cacheHeaders.findRawHeader("etag");
    if (it != cacheHeaders.rawHeaders.constEnd())
        httpRequest.setHeaderField("If-None-Match", it->second);

    QDateTime lastModified = metaData.lastModified();
    if (lastModified.isValid())
        httpRequest.setHeaderField("If-Modified-Since", QNetworkHeadersPrivate::toHttpDate(lastModified));

    if (CacheLoadControlAttribute == QNetworkRequest::PreferNetwork) {
        it = cacheHeaders.findRawHeader("Cache-Control");
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            QHash<QByteArray, QByteArray> cacheControl = parseHttpOptionHeader(it->second);
            if (cacheControl.contains("must-revalidate"))
                return;
        }
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDateTime expirationDate = metaData.expirationDate();

#if 0
    /*
     * age_value
     *      is the value of Age: header received by the cache with
     *              this response.
     * date_value
     *      is the value of the origin server's Date: header
     * request_time
     *      is the (local) time when the cache made the request
     *              that resulted in this cached response
     * response_time
     *      is the (local) time when the cache received the
     *              response
     * now
     *      is the current (local) time
     */
    int age_value = 0;
    it = cacheHeaders.findRawHeader("age");
    if (it != cacheHeaders.rawHeaders.constEnd())
        age_value = it->second.toInt();

    QDateTime dateHeader;
    int date_value = 0;
    it = cacheHeaders.findRawHeader("date");
    if (it != cacheHeaders.rawHeaders.constEnd()) {
        dateHeader = QNetworkHeadersPrivate::fromHttpDate(it->second);
        date_value = dateHeader.toTime_t();
    }

    int now = currentDateTime.toUTC().toTime_t();
    int request_time = now;
    int response_time = now;

    // Algorithm from RFC 2616 section 13.2.3
    int apparent_age = qMax(0, response_time - date_value);
    int corrected_received_age = qMax(apparent_age, age_value);
    int response_delay = response_time - request_time;
    int corrected_initial_age = corrected_received_age + response_delay;
    int resident_time = now - response_time;
    int current_age   = corrected_initial_age + resident_time;

    // RFC 2616 13.2.4 Expiration Calculations
    if (!expirationDate.isValid()) {
        if (lastModified.isValid()) {
            int diff = currentDateTime.secsTo(lastModified);
            expirationDate = lastModified;
            expirationDate.addSecs(diff / 10);
            if (httpRequest.headerField("Warning").isEmpty()) {
                QDateTime dt;
                dt.setTime_t(current_age);
                if (dt.daysTo(currentDateTime) > 1)
                    httpRequest.setHeaderField("Warning", "113");
            }
        }
    }

    // the cache-saving code below sets the expirationDate with date+max_age
    // if "max-age" is present, or to Expires otherwise
    int freshness_lifetime = dateHeader.secsTo(expirationDate);
    bool response_is_fresh = (freshness_lifetime > current_age);
#else
    bool response_is_fresh = currentDateTime.secsTo(expirationDate) >= 0;
#endif

    if (!response_is_fresh)
        return;

    loadedFromCache = true;
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "response_is_fresh" << CacheLoadControlAttribute;
#endif
	 /*[sslab cache]*/
/*	 qint64 before_ts = currentDateTime.currentMSecsSinceEpoch();*/

	 if (!sendCacheContents(metaData))
        loadedFromCache = false;

/*	 QDateTime currentTS = QDateTime::currentDateTime();
	 qint64 after_ts = currentTS.currentMSecsSinceEpoch();
	 qWarning("[cacheTime]sendCacheContents() consumes %lld(ms)",after_ts - before_ts);
	 incCacheTime(after_ts - before_ts);*/
}

static QHttpNetworkRequest::Priority convert(const QNetworkRequest::Priority& prio)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    switch (prio) {
    case QNetworkRequest::LowPriority:
        return QHttpNetworkRequest::LowPriority;
    case QNetworkRequest::HighPriority:
        return QHttpNetworkRequest::HighPriority;
    case QNetworkRequest::NormalPriority:
    default:
        return QHttpNetworkRequest::NormalPriority;
    }
}

void QNetworkAccessHttpBackend::postRequest()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#ifdef SSLAB_SR
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	const int sr_flag = *shdflag;
	shmdt(shdflag);
	qWarning("[SR-Flag]: %d", sr_flag);
	
	int sr_scheme;
	bool estimatingTime = false;

	int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
	char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
	char readyUrl[4096] = {0,};
	strcpy(readyUrl, shdurl);
	shmdt(shdurl);

//	qWarning("[SR-URLs(backend)] url():%s , readyUrl:%s",request().url().toString().toLatin1().data(), readyUrl);
//	if (!sr_flag || strcmp(request().url().toString().toLatin1().data(), readyUrl)) {
	qWarning("[SR-URLs(backend)] url():%s , readyUrl:%s, readyUrl(len):%d",request().originalUrl(), readyUrl, strlen(readyUrl));
	if (!sr_flag % 2 || !strlen(readyUrl) || strcmp(request().originalUrl(), readyUrl)) {
		int seg_sr = shmget(1234, PAGE_SIZE, 0666);
		int* shdsr;
		shdsr = (int*)shmat(seg_sr, NULL, 0);
		sr_scheme = *shdsr;
		shmdt(shdsr);
	}
	else {
		sr_scheme = sr_flag;
		estimatingTime = true;
	}
/*	bool estimatingTime = false;
	QString _typeName(request().rawHeader("Accept"));
	qWarning("[SR-content-type]: %s", _typeName.toAscii().data());

	int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
	char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
	char readyUrl[4096] = {0,};
	strcpy(readyUrl, shdurl);
	shmdt(shdurl);

	if (strlen(readyUrl) || 
			!sr_flag || 
			strncmp(_typeName.toAscii().data(), "text/css", 8)) {
		int seg_sr;
		int* shdsr;
		seg_sr = shmget(1234, PAGE_SIZE, 0666);
		shdsr = (int*)shmat(seg_sr, NULL, 0);
		sr_scheme = *shdsr;
		shmdt(shdsr);
	}
	else {
		sr_scheme = sr_flag;	
		estimatingTime = true;
	}
	qWarning("[SR-Scheme]: %d", sr_scheme);*/
#endif
	bool loadedFromCache = false;
    QHttpNetworkRequest httpRequest;
    httpRequest.setPriority(convert(request().priority()));
    switch (operation()) {
    case QNetworkAccessManager::GetOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Get);
#ifndef SSLAB_SR
        validateCache(httpRequest, loadedFromCache);
#endif
#ifdef SSLAB_SR
		if (sr_scheme < 2) {
			validateCache(httpRequest, loadedFromCache, estimatingTime, sr_flag);
			if (estimatingTime) {
				seg_flag = shmget(1111, PAGE_SIZE, 0666);
				shdflag = (int*)shmat(seg_flag, NULL, 0);
				if (sr_flag == *shdflag) {
					*shdflag = 0;	//because miss	
					qWarning("[SR]shdflag set to [%d] (caused by miss.)", *shdflag);
					seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
					shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
					memset(shdurl, 0x00, sizeof(char)*4096);
					shmdt(shdurl);
				}
				shmdt(shdflag);
			}
		}
/*		else if (sr_scheme == 2) {
			seg_flag = shmget(1111, PAGE_SIZE, 0666);
			shdflag = (int*)shmat(seg_flag, NULL, 0);
			*shdflag = 3;
//			qWarning("[SR]shdflag set to [%d]", *shdflag);
			shmdt(shdflag);
		}*/
#endif
        break;

    case QNetworkAccessManager::HeadOperation:
        httpRequest.setOperation(QHttpNetworkRequest::Head);
        validateCache(httpRequest, loadedFromCache);
        break;

    case QNetworkAccessManager::PostOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Post);
        httpRequest.setUploadByteDevice(createUploadByteDevice());
        break;

    case QNetworkAccessManager::PutOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Put);
        httpRequest.setUploadByteDevice(createUploadByteDevice());
        break;

    case QNetworkAccessManager::DeleteOperation:
        invalidateCache();
        httpRequest.setOperation(QHttpNetworkRequest::Delete);
        break;

    case QNetworkAccessManager::CustomOperation:
        invalidateCache(); // for safety reasons, we don't know what the operation does
        httpRequest.setOperation(QHttpNetworkRequest::Custom);
        httpRequest.setUploadByteDevice(createUploadByteDevice());
        httpRequest.setCustomVerb(request().attribute(
                QNetworkRequest::CustomVerbAttribute).toByteArray());
        break;

    default:
        break;                  // can't happen
    }
    httpRequest.setUrl(url());

    QList<QByteArray> headers = request().rawHeaderList();
    if (resumeOffset != 0) {
        if (headers.contains("Range")) {
            // Need to adjust resume offset for user specified range

            headers.removeOne("Range");

            // We've already verified that requestRange starts with "bytes=", see canResume.
            QByteArray requestRange = request().rawHeader("Range").mid(6);

            int index = requestRange.indexOf('-');

            quint64 requestStartOffset = requestRange.left(index).toULongLong();
            quint64 requestEndOffset = requestRange.mid(index + 1).toULongLong();

            requestRange = "bytes=" + QByteArray::number(resumeOffset + requestStartOffset) +
                           '-' + QByteArray::number(requestEndOffset);

            httpRequest.setHeaderField("Range", requestRange);
        } else {
            httpRequest.setHeaderField("Range", "bytes=" + QByteArray::number(resumeOffset) + '-');
        }
    }
    foreach (const QByteArray &header, headers)
        httpRequest.setHeaderField(header, request().rawHeader(header));
		
    if (loadedFromCache) {
		setResourceFromCache(loadedFromCache);
        // commented this out since it will be called later anyway
        // by copyFinished()
        //QNetworkAccessBackend::finished();
        return;    // no need to send the request! :)
    }

    if (request().attribute(QNetworkRequest::HttpPipeliningAllowedAttribute).toBool() == true)
        httpRequest.setPipeliningAllowed(true);

    if (static_cast<QNetworkRequest::LoadControl>
        (request().attribute(QNetworkRequest::AuthenticationReuseAttribute,
                             QNetworkRequest::Automatic).toInt()) == QNetworkRequest::Manual)
        httpRequest.setWithCredentials(false);

    httpReply = http->sendRequest(httpRequest);
    httpReply->setParent(this);
#ifndef QT_NO_OPENSSL
    if (pendingSslConfiguration)
        httpReply->setSslConfiguration(*pendingSslConfiguration);
    if (pendingIgnoreAllSslErrors)
        httpReply->ignoreSslErrors();
    httpReply->ignoreSslErrors(pendingIgnoreSslErrorsList);
    connect(httpReply, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
#endif

    connect(httpReply, SIGNAL(readyRead()), SLOT(replyReadyRead()));
    connect(httpReply, SIGNAL(finished()), SLOT(replyFinished()));
    connect(httpReply, SIGNAL(finishedWithError(QNetworkReply::NetworkError,QString)),
            SLOT(httpError(QNetworkReply::NetworkError,QString)));
    connect(httpReply, SIGNAL(headerChanged()), SLOT(replyHeaderChanged()));
    connect(httpReply, SIGNAL(cacheCredentials(QHttpNetworkRequest,QAuthenticator*)),
            SLOT(httpCacheCredentials(QHttpNetworkRequest,QAuthenticator*)));
#ifndef QT_NO_NETWORKPROXY
    connect(httpReply, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
#endif
    connect(httpReply, SIGNAL(authenticationRequired(const QHttpNetworkRequest,QAuthenticator*)),
                SLOT(httpAuthenticationRequired(const QHttpNetworkRequest,QAuthenticator*)));
}

void QNetworkAccessHttpBackend::invalidateCache()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QAbstractNetworkCache *nc = networkCache();
    if (nc)
        nc->remove(url());
	 
	 /*[sslab cache]*/
	 nc = networkTopCache();
	 if (nc)
		 nc->remove(url());
}

void QNetworkAccessHttpBackend::open()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QUrl url = request().url();
    bool encrypt = url.scheme().toLower() == QLatin1String("https");
    setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, encrypt);

    // set the port number in the reply if it wasn't set
    url.setPort(url.port(encrypt ? DefaultHttpsPort : DefaultHttpPort));

    QNetworkProxy *theProxy = 0;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy transparentProxy, cacheProxy;

    foreach (const QNetworkProxy &p, proxyList()) {
        // use the first proxy that works
        // for non-encrypted connections, any transparent or HTTP proxy
        // for encrypted, only transparent proxies
        if (!encrypt
            && (p.capabilities() & QNetworkProxy::CachingCapability)
            && (p.type() == QNetworkProxy::HttpProxy ||
                p.type() == QNetworkProxy::HttpCachingProxy)) {
            cacheProxy = p;
            transparentProxy = QNetworkProxy::NoProxy;
            theProxy = &cacheProxy;
            break;
        }
        if (p.isTransparentProxy()) {
            transparentProxy = p;
            cacheProxy = QNetworkProxy::NoProxy;
            theProxy = &transparentProxy;
            break;
        }
    }

    // check if at least one of the proxies
    if (transparentProxy.type() == QNetworkProxy::DefaultProxy &&
        cacheProxy.type() == QNetworkProxy::DefaultProxy) {
        // unsuitable proxies
        if (isSynchronous()) {
            error(QNetworkReply::ProxyNotFoundError, tr("No suitable proxy found"));
            finished();
        } else {
            QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                      Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ProxyNotFoundError),
                                      Q_ARG(QString, tr("No suitable proxy found")));
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        }
            return;
    }
#endif

    if (isSynchronous()) {
        // for synchronous requests, we just create a new connection
        http = new QHttpNetworkConnection(1, url.host(), url.port(), encrypt, this);
#ifndef QT_NO_NETWORKPROXY
        http->setTransparentProxy(transparentProxy);
        http->setCacheProxy(cacheProxy);
#endif
        postRequest();
        processRequestSynchronously();
    } else {
        // check if we have an open connection to this host
        cacheKey = makeCacheKey(this, theProxy);
        QNetworkAccessCache *cache = QNetworkAccessManagerPrivate::getObjectCache(this);
        // the http object is actually a QHttpNetworkConnection
        http = static_cast<QNetworkAccessCachedHttpConnection *>(cache->requestEntryNow(cacheKey));
        if (http == 0) {
            // no entry in cache; create an object
            // the http object is actually a QHttpNetworkConnection
            http = new QNetworkAccessCachedHttpConnection(url.host(), url.port(), encrypt);

#ifndef QT_NO_NETWORKPROXY
            http->setTransparentProxy(transparentProxy);
            http->setCacheProxy(cacheProxy);
#endif

            // cache the QHttpNetworkConnection corresponding to this cache key
            cache->addEntry(cacheKey, static_cast<QNetworkAccessCachedHttpConnection *>(http.data()));
        }
        postRequest();
    }
}

void QNetworkAccessHttpBackend::closeDownstreamChannel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // this indicates that the user closed the stream while the reply isn't finished yet
}

void QNetworkAccessHttpBackend::downstreamReadyWrite()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    readFromHttp();
    if (httpReply && httpReply->bytesAvailable() == 0 && httpReply->isFinished())
        replyFinished();
}

void QNetworkAccessHttpBackend::setDownstreamLimited(bool b)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply)
        httpReply->setDownstreamLimited(b);
}

void QNetworkAccessHttpBackend::replyReadyRead()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    readFromHttp();
}

void QNetworkAccessHttpBackend::readFromHttp()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!httpReply)
        return;

    // We read possibly more than nextDownstreamBlockSize(), but
    // this is not a critical thing since it is already in the
    // memory anyway

    QByteDataBuffer list;

    while (httpReply->bytesAvailable() != 0 && nextDownstreamBlockSize() != 0 && nextDownstreamBlockSize() > list.byteAmount()) {
        list.append(httpReply->readAny());
    }

    if (!list.isEmpty())
      writeDownstreamData(list);
}

void QNetworkAccessHttpBackend::replyFinished()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply->bytesAvailable())
        // we haven't read everything yet. Wait some more.
        return;

    int statusCode = httpReply->statusCode();
    if (statusCode >= 400) {
        // it's an error reply
        QString msg = QLatin1String(QT_TRANSLATE_NOOP("QNetworkReply",
                                                      "Error downloading %1 - server replied: %2"));
        msg = msg.arg(url().toString(), httpReply->reasonPhrase());
        error(statusCodeFromHttp(httpReply->statusCode(), httpReply->url()), msg);
    }

#ifndef QT_NO_OPENSSL
    // store the SSL configuration now
    // once we call finished(), we won't have access to httpReply anymore
    QSslConfiguration sslConfig = httpReply->sslConfiguration();
    if (pendingSslConfiguration) {
        *pendingSslConfiguration = sslConfig;
    } else if (!sslConfig.isNull()) {
        QT_TRY {
            pendingSslConfiguration = new QSslConfiguration(sslConfig);
        } QT_CATCH(...) {
            qWarning("QNetworkAccess: could not allocate a QSslConfiguration object for a SSL connection.");
        }
    }
#endif

    finished();
}

void QNetworkAccessHttpBackend::checkForRedirect(const int statusCode)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    switch (statusCode) {
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // What do we do about the caching of the HTML note?
        // The response to a 303 MUST NOT be cached, while the response to
        // all of the others is cacheable if the headers indicate it to be
        QByteArray header = rawHeader("location");
        QUrl url = QUrl::fromEncoded(header);
        if (!url.isValid())
            url = QUrl(QLatin1String(header));
        redirectionRequested(url);
    }
}

void QNetworkAccessHttpBackend::replyHeaderChanged()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    setAttribute(QNetworkRequest::HttpPipeliningWasUsedAttribute, httpReply->isPipeliningUsed());

    // reconstruct the HTTP header
    QList<QPair<QByteArray, QByteArray> > headerMap = httpReply->header();
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = headerMap.constBegin(),
                                                        end = headerMap.constEnd();
    QByteArray header;

    for (; it != end; ++it) {
        QByteArray value = rawHeader(it->first);
        if (!value.isEmpty()) {
            if (qstricmp(it->first.constData(), "set-cookie") == 0)
                value += '\n';
            else
                value += ", ";
        }
        value += it->second;
        setRawHeader(it->first, value);
    }

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, httpReply->statusCode());
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, httpReply->reasonPhrase());

    // is it a redirection?
    const int statusCode = httpReply->statusCode();
    checkForRedirect(statusCode);

    if (statusCode >= 500 && statusCode < 600) {
		   /*[sslab cache]*/
			QAbstractNetworkCache *nc = networkTopCache();
			if (nc) {
				QNetworkCacheMetaData tmpMetaData = nc->metaData(url());
				if (!tmpMetaData.isValid())
					nc = networkCache();
			}
			else nc = networkCache();

			if (nc) {
			   QNetworkCacheMetaData metaData = nc->metaData(url());
            QNetworkHeadersPrivate cacheHeaders;
            cacheHeaders.setAllRawHeaders(metaData.rawHeaders());
            QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;
            it = cacheHeaders.findRawHeader("Cache-Control");
            bool mustReValidate = false;
            if (it != cacheHeaders.rawHeaders.constEnd()) {
                QHash<QByteArray, QByteArray> cacheControl = parseHttpOptionHeader(it->second);
                if (cacheControl.contains("must-revalidate"))
                    mustReValidate = true;
            }
            if (!mustReValidate && sendCacheContents(metaData))
                return;
        }
    }

    if (statusCode == 304) {
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        qDebug() << "Received a 304 from" << url();
#endif
		  /*[sslab cache]*/
		  QAbstractNetworkCache *nc = networkTopCache();
		  if (nc) {
			  QNetworkCacheMetaData tmpMetaData = nc->metaData(url());
			  if (!tmpMetaData.isValid()) 
				  nc = networkCache();
		  }
		  else nc = networkCache();

		  if (nc) {
			   QNetworkCacheMetaData oldMetaData = nc->metaData(url());
			   QNetworkCacheMetaData metaData = fetchCacheMetaData(oldMetaData); /*[sslab cache]*/
            if (oldMetaData != metaData)
                nc->updateMetaData(metaData);
            if (sendCacheContents(metaData))
                return;
        }
    }


    if (statusCode != 304 && statusCode != 303) {
        if (!isCachingEnabled())
            setCachingEnabled(true);
    }
    metaDataChanged();
}

void QNetworkAccessHttpBackend::httpAuthenticationRequired(const QHttpNetworkRequest &,
                                                           QAuthenticator *auth)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    authenticationRequired(auth);
}

void QNetworkAccessHttpBackend::httpCacheCredentials(const QHttpNetworkRequest &,
                                                 QAuthenticator *auth)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    cacheCredentials(auth);
}

void QNetworkAccessHttpBackend::httpError(QNetworkReply::NetworkError errorCode,
                                          const QString &errorString)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "http error!" << errorCode << errorString;
#endif
    error(errorCode, errorString);
    finished();
}

/*
    A simple web page that can be used to test us: http://www.procata.com/cachetest/
 */
bool QNetworkAccessHttpBackend::sendCacheContents(const QNetworkCacheMetaData &metaData)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    setCachingEnabled(false);
    if (!metaData.isValid())
        return false;

	 /*[sslab cache]*/
    QAbstractNetworkCache *nc = networkTopCache();
	 if (!nc) nc = networkCache();
	 else {
		 QNetworkCacheMetaData confirm = nc->metaData(url());
		 if (!confirm.isValid())
			 nc = networkCache();
	 }
	 //////////////////

	 Q_ASSERT(nc);
    QIODevice *contents = nc->data(url());
    if (!contents) {
#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        qDebug() << "Can not send cache, the contents are 0" << url();
#endif
        return false;
    }
    contents->setParent(this);

    QNetworkCacheMetaData::AttributesMap attributes = metaData.attributes();
    int status = attributes.value(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status < 100)
        status = 200;           // fake it

    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, attributes.value(QNetworkRequest::HttpReasonPhraseAttribute));
    setAttribute(QNetworkRequest::SourceIsFromCacheAttribute, true);

    QNetworkCacheMetaData::RawHeaderList rawHeaders = metaData.rawHeaders();
    QNetworkCacheMetaData::RawHeaderList::ConstIterator it = rawHeaders.constBegin(),
                                                       end = rawHeaders.constEnd();
    for ( ; it != end; ++it)
        setRawHeader(it->first, it->second);

    checkForRedirect(status);

    // This needs to be emitted in the event loop because it can be reached at
    // the direct code path of qnam.get(...) before the user has a chance
    // to connect any signals.
    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    qRegisterMetaType<QIODevice*>("QIODevice*");
    QMetaObject::invokeMethod(this, "writeDownstreamData", Qt::QueuedConnection, Q_ARG(QIODevice*, contents));


#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
    qDebug() << "Successfully sent cache:" << url() << contents->size() << "bytes";
#endif
    if (httpReply)
        disconnect(httpReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    return true;
}

void QNetworkAccessHttpBackend::copyFinished(QIODevice *dev)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    delete dev;
    finished();
}

#ifndef QT_NO_OPENSSL
void QNetworkAccessHttpBackend::ignoreSslErrors()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply)
        httpReply->ignoreSslErrors();
    else
        pendingIgnoreAllSslErrors = true;
}

void QNetworkAccessHttpBackend::ignoreSslErrors(const QList<QSslError> &errors)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply) {
        httpReply->ignoreSslErrors(errors);
    } else {
        // the pending list is set if QNetworkReply::ignoreSslErrors(const QList<QSslError> &errors)
        // is called before QNetworkAccessManager::get() (or post(), etc.)
        pendingIgnoreSslErrorsList = errors;
    }
}

void QNetworkAccessHttpBackend::fetchSslConfiguration(QSslConfiguration &config) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply)
        config = httpReply->sslConfiguration();
    else if (pendingSslConfiguration)
        config = *pendingSslConfiguration;
}

void QNetworkAccessHttpBackend::setSslConfiguration(const QSslConfiguration &newconfig)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (httpReply)
        httpReply->setSslConfiguration(newconfig);
    else if (pendingSslConfiguration)
        *pendingSslConfiguration = newconfig;
    else
        pendingSslConfiguration = new QSslConfiguration(newconfig);
}
#endif

QNetworkCacheMetaData QNetworkAccessHttpBackend::fetchCacheMetaData(const QNetworkCacheMetaData &oldMetaData) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QNetworkCacheMetaData metaData = oldMetaData;

    QNetworkHeadersPrivate cacheHeaders;
    cacheHeaders.setAllRawHeaders(metaData.rawHeaders());
    QNetworkHeadersPrivate::RawHeadersList::ConstIterator it;

    QList<QByteArray> newHeaders = rawHeaderList();
    foreach (QByteArray header, newHeaders) {
        QByteArray originalHeader = header;
        header = header.toLower();
        bool hop_by_hop =
            (header == "connection"
             || header == "keep-alive"
             || header == "proxy-authenticate"
             || header == "proxy-authorization"
             || header == "te"
             || header == "trailers"
             || header == "transfer-encoding"
             || header ==  "upgrade");
        if (hop_by_hop)
            continue;

        // we are currently not using the date header to determine the expiration time of a page,
        // but only the "Expires", "max-age" and "s-maxage" headers, see
        // QNetworkAccessHttpBackend::validateCache() and below ("metaData.setExpirationDate()").
        if (header == "date")
            continue;

        // Don't store Warning 1xx headers
        if (header == "warning") {
            QByteArray v = rawHeader(header);
            if (v.length() == 3
                && v[0] == '1'
                && v[1] >= '0' && v[1] <= '9'
                && v[2] >= '0' && v[2] <= '9')
                continue;
        }

        it = cacheHeaders.findRawHeader(header);
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            // Match the behavior of Firefox and assume Cache-Control: "no-transform"
            if (header == "content-encoding"
                || header == "content-range"
                || header == "content-type")
                continue;

            // For MS servers that send "Content-Length: 0" on 304 responses
            // ignore this too
            if (header == "content-length")
                continue;
        }

#if defined(QNETWORKACCESSHTTPBACKEND_DEBUG)
        QByteArray n = rawHeader(header);
        QByteArray o;
        if (it != cacheHeaders.rawHeaders.constEnd())
            o = (*it).second;
        if (n != o && header != "date") {
            qDebug() << "replacing" << header;
            qDebug() << "new" << n;
            qDebug() << "old" << o;
        }
#endif
        cacheHeaders.setRawHeader(originalHeader, rawHeader(header));
    }
    metaData.setRawHeaders(cacheHeaders.rawHeaders);

    bool checkExpired = true;

    QHash<QByteArray, QByteArray> cacheControl;
    it = cacheHeaders.findRawHeader("Cache-Control");
    if (it != cacheHeaders.rawHeaders.constEnd()) {
        cacheControl = parseHttpOptionHeader(it->second);
        QByteArray maxAge = cacheControl.value("max-age");
        if (!maxAge.isEmpty()) {
            checkExpired = false;
            QDateTime dt = QDateTime::currentDateTime();
            dt = dt.addSecs(maxAge.toInt());
            metaData.setExpirationDate(dt);
        }
    }
    if (checkExpired) {
        it = cacheHeaders.findRawHeader("expires");
        if (it != cacheHeaders.rawHeaders.constEnd()) {
            QDateTime expiredDateTime = QNetworkHeadersPrivate::fromHttpDate(it->second);
            metaData.setExpirationDate(expiredDateTime);
        }
    }

    it = cacheHeaders.findRawHeader("last-modified");
    if (it != cacheHeaders.rawHeaders.constEnd())
        metaData.setLastModified(QNetworkHeadersPrivate::fromHttpDate(it->second));

    bool canDiskCache;
    // only cache GET replies by default, all other replies (POST, PUT, DELETE)
    //  are not cacheable by default (according to RFC 2616 section 9)
    if (httpReply->request().operation() == QHttpNetworkRequest::Get) {

        canDiskCache = true;
        // 14.32
        // HTTP/1.1 caches SHOULD treat "Pragma: no-cache" as if the client
        // had sent "Cache-Control: no-cache".
        it = cacheHeaders.findRawHeader("pragma");
        if (it != cacheHeaders.rawHeaders.constEnd()
            && it->second == "no-cache")
            canDiskCache = false;

        // HTTP/1.1. Check the Cache-Control header
        if (cacheControl.contains("no-cache"))
            canDiskCache = false;
        else if (cacheControl.contains("no-store"))
            canDiskCache = false;

    // responses to POST might be cacheable
    } else if (httpReply->request().operation() == QHttpNetworkRequest::Post) {

        canDiskCache = false;
        // some pages contain "expires:" and "cache-control: no-cache" field,
        // so we only might cache POST requests if we get "cache-control: max-age ..."
        if (cacheControl.contains("max-age"))
            canDiskCache = true;

    // responses to PUT and DELETE are not cacheable
    } else {
        canDiskCache = false;
    }

    metaData.setSaveToDisk(canDiskCache);
    int statusCode = httpReply->statusCode();
    QNetworkCacheMetaData::AttributesMap attributes;
    if (statusCode != 304) {
        // update the status code
        attributes.insert(QNetworkRequest::HttpStatusCodeAttribute, statusCode);
        attributes.insert(QNetworkRequest::HttpReasonPhraseAttribute, httpReply->reasonPhrase());
    } else {
        // this is a redirection, keep the attributes intact
        attributes = oldMetaData.attributes();
    }
    metaData.setAttributes(attributes);
    return metaData;
}

bool QNetworkAccessHttpBackend::canResume() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // Only GET operation supports resuming.
    if (operation() != QNetworkAccessManager::GetOperation)
        return false;

    // Can only resume if server/resource supports Range header.
    if (httpReply->headerField("Accept-Ranges", "none") == "none")
        return false;

    // We only support resuming for byte ranges.
    if (request().hasRawHeader("Range")) {
        QByteArray range = request().rawHeader("Range");
        if (!range.startsWith("bytes="))
            return false;
    }

    return true;
}

void QNetworkAccessHttpBackend::setResumeOffset(quint64 offset)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    resumeOffset = offset;
}

bool QNetworkAccessHttpBackend::processRequestSynchronously()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QHttpNetworkConnectionChannel *channel = &http->channels()[0];

    // Disconnect all socket signals. They will only confuse us when using waitFor*
    QObject::disconnect(channel->socket, 0, 0, 0);

    qint64 timeout = 20*1000; // 20 sec
    QElapsedTimer timeoutTimer;

    bool waitResult = channel->socket->waitForConnected(timeout);
    timeoutTimer.start();

    if (!waitResult || channel->socket->state() != QAbstractSocket::ConnectedState) {
        error(QNetworkReply::UnknownNetworkError, QLatin1String("could not connect"));
        return false;
    }
    channel->_q_connected(); // this will send the request (via sendRequest())

#ifndef QT_NO_OPENSSL
    if (http->isSsl()) {
        qint64 remainingTimeEncrypted = timeout - timeoutTimer.elapsed();
        if (!static_cast<QSslSocket *>(channel->socket)->waitForEncrypted(remainingTimeEncrypted)) {
            error(QNetworkReply::SslHandshakeFailedError,
                  QLatin1String("could not encrypt or timeout while encrypting"));
            return false;
        }
        channel->_q_encrypted();
    }
#endif

    // if we get a 401 or 407, we might need to send the request twice, see below
    bool authenticating = false;

    do {
        channel->sendRequest();

        qint64 remainingTimeBytesWritten;
        while(channel->socket->bytesToWrite() > 0 ||
              channel->state == QHttpNetworkConnectionChannel::WritingState) {
            remainingTimeBytesWritten = timeout - timeoutTimer.elapsed();
            channel->sendRequest(); // triggers channel->socket->write()
            if (!channel->socket->waitForBytesWritten(remainingTimeBytesWritten)) {
                error(QNetworkReply::TimeoutError,
                      QLatin1String("could not write bytes to socket or timeout while writing"));
                return false;
            }
        }

        qint64 remainingTimeBytesRead = timeout - timeoutTimer.elapsed();
        // Loop for at most remainingTime until either the socket disconnects
        // or the reply is finished
        do {
            waitResult = channel->socket->waitForReadyRead(remainingTimeBytesRead);
            remainingTimeBytesRead = timeout - timeoutTimer.elapsed();
            if (!waitResult || remainingTimeBytesRead <= 0
                || channel->socket->state() != QAbstractSocket::ConnectedState) {
                error(QNetworkReply::TimeoutError,
                      QLatin1String("could not read from socket or timeout while reading"));
                return false;
            }

            if (channel->socket->bytesAvailable())
                channel->_q_readyRead();

            if (!httpReply)
                return false; // we got a 401 or 407 and cannot handle it (it might happen that
                              // disconnectFromHttp() was called, in that case the reply is zero)
            // ### I am quite sure this does not work for NTLM
            // ### how about uploading to an auth / proxyAuth site?

            authenticating = (httpReply->statusCode() == 401 || httpReply->statusCode() == 407);

            if (httpReply->isFinished())
                break;
        } while (remainingTimeBytesRead > 0);
    } while (authenticating);

    return true;
}

/*[sslab cache]*/
void QNetworkAccessHttpBackend::reorderFile(const QAbstractNetworkCache* nc)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	QString saveFileName = nc->_cacheFileName(url());
	qWarning("[onHit]from:%s",saveFileName.toAscii().data());
	if (!QFile::exists(saveFileName))
		return;
	QFile file(saveFileName);
	if (nc->lastCache() == true) {//In the 2nd cache,
		QString migFileName = nc->migrateDirectory();
		char tmp[128] = {0,};
		char *ptr;
		sprintf(tmp, "%s", saveFileName.toAscii().data());
		qint32 len = strlen((nc->cacheDirectory()).toAscii().data());
		ptr = &(tmp[len]);
		migFileName += QLatin1String(ptr);	
		file.copy(migFileName);
		file.remove(saveFileName);	
		qWarning("[onHit]to:%s",migFileName.toAscii().data());
	}
/*	else if (saveFileName.endsWith(CACHESD_POSTFIX)) {// if in the 1st cache(SIZE)
		char tmp[128] = {0,};
		sprintf(tmp, "%s", saveFileName.toAscii().data());
		qint32 full_len = strlen(tmp);
		QString s = CACHESD_POSTFIX;
		qint32 postfix_len = strlen(s.toAscii().data());
		tmp[full_len - postfix_len] = '\0';
		saveFileName = QLatin1String(tmp);
		file.rename(saveFileName);
		qWarning("[onHit]to:%s",saveFileName.toAscii().data());
	}*/
}

QT_END_NAMESPACE

#endif // QT_NO_HTTP
