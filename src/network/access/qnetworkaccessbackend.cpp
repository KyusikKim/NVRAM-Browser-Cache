
/*#ifdef SSLAB__DEBUG
#ifndef SSLAB__QDATETIME
#define SSLAB__QDATETIME
#include <QDateTime>
#endif
#endif*/

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

#include "qnetworkaccessbackend_p.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qnetworkreply_p.h"
#include "QtCore/qhash.h"
#include "QtCore/qmutex.h"
#include "QtNetwork/qnetworksession.h"
#include "QtCore/qdatetime.h" /*[sslab cache]*/

#include "qnetworkaccesscachebackend_p.h"
#include "qabstractnetworkcache.h"
#include "qhostinfo.h"

#include "private/qnoncontiguousbytedevice_p.h"

QT_BEGIN_NAMESPACE

static bool factoryDataShutdown = false;
class QNetworkAccessBackendFactoryData: public QList<QNetworkAccessBackendFactory *>
{
public:
    QNetworkAccessBackendFactoryData() : mutex(QMutex::Recursive) { }
    ~QNetworkAccessBackendFactoryData()
    {
        QMutexLocker locker(&mutex); // why do we need to lock?
        factoryDataShutdown = true;
    }

    QMutex mutex;
};
Q_GLOBAL_STATIC(QNetworkAccessBackendFactoryData, factoryData)

QNetworkAccessBackendFactory::QNetworkAccessBackendFactory()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QMutexLocker locker(&factoryData()->mutex);
    factoryData()->append(this);
}

QNetworkAccessBackendFactory::~QNetworkAccessBackendFactory()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!factoryDataShutdown) {
        QMutexLocker locker(&factoryData()->mutex);
        factoryData()->removeAll(this);
    }
}

QNetworkAccessBackend *QNetworkAccessManagerPrivate::findBackend(QNetworkAccessManager::Operation op,
                                                                 const QNetworkRequest &request)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!factoryDataShutdown) {
        QMutexLocker locker(&factoryData()->mutex);
        QNetworkAccessBackendFactoryData::ConstIterator it = factoryData()->constBegin(),
                                                           end = factoryData()->constEnd();
        while (it != end) {
            QNetworkAccessBackend *backend = (*it)->create(op, request);
            if (backend) {
                backend->manager = this;
                return backend; // found a factory that handled our request
            }
            ++it;
        }
    }
    return 0;
}

QNonContiguousByteDevice* QNetworkAccessBackend::createUploadByteDevice()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QNonContiguousByteDevice* device = 0;

    if (reply->outgoingDataBuffer)
        device = QNonContiguousByteDeviceFactory::create(reply->outgoingDataBuffer);
    else if (reply->outgoingData) {
        device = QNonContiguousByteDeviceFactory::create(reply->outgoingData);
    } else {
        return 0;
    }

    bool bufferDisallowed =
            reply->request.attribute(QNetworkRequest::DoNotBufferUploadDataAttribute,
                          QVariant(false)) == QVariant(true);
    if (bufferDisallowed)
        device->disableReset();

    // make sure we delete this later
    device->setParent(this);

    connect(device, SIGNAL(readProgress(qint64,qint64)), this, SLOT(emitReplyUploadProgress(qint64,qint64)));

    return device;
}

// need to have this function since the reply is a private member variable
// and the special backends need to access this.
void QNetworkAccessBackend::emitReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (reply->isFinished())
        return;
    reply->emitUploadProgress(bytesSent, bytesTotal);
}

QNetworkAccessBackend::QNetworkAccessBackend()
    : manager(0)
    , reply(0)
    , synchronous(false)
	 , resourceLoadStartTime(0), resourceLoadEndTime(0) /*[sslab cache]*/
	 , resourceFromCache(false) /*[sslab cache]*/
{
}

QNetworkAccessBackend::~QNetworkAccessBackend()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	/*sslab.replay*/
	QFile outputFile(QLatin1String("/mnt/ramdisk/log.output"));
	if (!outputFile.open(QFile::Append)) {
		qWarning("[sslab] %s:%d:%s outputFile open error!",__FILE__,__LINE__,__func__);
		exit(-1);
	}
	char buff[4096] = {0,};
//	sprintf(buff, "[cache1Count]:%f\n[cache2Count]:%f\n[requestCount]:%f\n[cache1HitRatio]:%f%%\n[cache2HitRatio]:%f%%\n"
	sprintf(buff, "[cache1Count]:%f\n[cache2Count]:%f\n[requestCount]:%f\n[cache1HitRatio]:%f%%\n[cache2HitRatio]:%f%%\n[avg.ResourceLoadTime]:%fms\n"
			,getCache1Count()
			,getCache2Count()
			,getRequestCount()
			,cache1HitRatio()*100
			,cache2HitRatio()*100
			,((double)getResourceLoadTime()) / getRequestCount());
	outputFile.write(buff, strlen(buff));
	outputFile.close();
}

void QNetworkAccessBackend::downstreamReadyWrite()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // do nothing
}

void QNetworkAccessBackend::setDownstreamLimited(bool b)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_UNUSED(b);
    // do nothing
}

void QNetworkAccessBackend::copyFinished(QIODevice *)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // do nothing
}

void QNetworkAccessBackend::ignoreSslErrors()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // do nothing
}

void QNetworkAccessBackend::ignoreSslErrors(const QList<QSslError> &errors)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_UNUSED(errors);
    // do nothing
}

void QNetworkAccessBackend::fetchSslConfiguration(QSslConfiguration &) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // do nothing
}

void QNetworkAccessBackend::setSslConfiguration(const QSslConfiguration &)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // do nothing
}

QNetworkCacheMetaData QNetworkAccessBackend::fetchCacheMetaData(const QNetworkCacheMetaData &) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return QNetworkCacheMetaData();
}

QNetworkAccessManager::Operation QNetworkAccessBackend::operation() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->operation;
}

QNetworkRequest QNetworkAccessBackend::request() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->request;
}

#ifndef QT_NO_NETWORKPROXY
QList<QNetworkProxy> QNetworkAccessBackend::proxyList() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->proxyList;
}
#endif

QAbstractNetworkCache *QNetworkAccessBackend::networkCache() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!manager)
        return 0;
    return manager->networkCache;
}

/*[sslab cache]*/
QAbstractNetworkCache *QNetworkAccessBackend::networkTopCache() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!manager)
        return 0;
    return manager->networkTopCache;
}

/*[sslab cache]*/
void QNetworkAccessBackend::incCache1Count()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->cache1Count++;
}

void QNetworkAccessBackend::incCache2Count()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->cache2Count++;
}

void QNetworkAccessBackend::incRequestCount()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->requestCount++;
}

qreal QNetworkAccessBackend::getCache1Count()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->cache1Count;
}
qreal QNetworkAccessBackend::getCache2Count()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->cache2Count;
}
qreal QNetworkAccessBackend::getRequestCount()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->requestCount;
}

qreal QNetworkAccessBackend::cache1HitRatio()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return (manager->cache1Count)/(manager->requestCount);
}

qreal QNetworkAccessBackend::cache2HitRatio()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return (manager->cache2Count)/(manager->requestCount);
}

void QNetworkAccessBackend::incCacheTime(qint64 consumeTime)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->cacheTime += consumeTime;
}

qint64 QNetworkAccessBackend::getCacheTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->cacheTime;
}

void QNetworkAccessBackend::setResourceLoadStartTime(qint64 timestamp)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	resourceLoadStartTime = timestamp;
}

qint64 QNetworkAccessBackend::getResourceLoadStartTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	return resourceLoadStartTime;
}

void QNetworkAccessBackend::setResourceLoadEndTime(qint64 timestamp)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	resourceLoadEndTime = timestamp;
}

qint64 QNetworkAccessBackend::getResourceLoadEndTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	return resourceLoadEndTime;
}

void QNetworkAccessBackend::incResourceLoadTime(qint64 consumedTime)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->resourceLoadTime += consumedTime;
}

qint64 QNetworkAccessBackend::getResourceLoadTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->resourceLoadTime;
}

void QNetworkAccessBackend::incFromCacheResourceLoadTime(qint64 fromCacheTime)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	manager->fromCacheResourceLoadTime += fromCacheTime;
}

void QNetworkAccessBackend::setResourceFromCache(bool loadedFromCache)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	resourceFromCache = loadedFromCache;
}

bool QNetworkAccessBackend::getResourceFromCache()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	return resourceFromCache;
}

qint64 QNetworkAccessBackend::getFromCacheResourceLoadTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->fromCacheResourceLoadTime;
}

void QNetworkAccessBackend::setPageLoadStartTime(qint64 timestamp)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return;
	if (manager->pageLoadStartTime == 0)
		manager->pageLoadStartTime = timestamp;
}

qint64 QNetworkAccessBackend::getPageLoadStartTime()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	if (!manager)
		return 0;
	return manager->pageLoadStartTime;
}

void QNetworkAccessBackend::setCachingEnabled(bool enable)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->setCachingEnabled(enable);
}

bool QNetworkAccessBackend::isCachingEnabled() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->isCachingEnabled();
}

qint64 QNetworkAccessBackend::nextDownstreamBlockSize() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->nextDownstreamBlockSize();
}

void QNetworkAccessBackend::writeDownstreamData(QByteDataBuffer &list)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->appendDownstreamData(list);
}

void QNetworkAccessBackend::writeDownstreamData(QIODevice *data)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->appendDownstreamData(data);
}

QVariant QNetworkAccessBackend::header(QNetworkRequest::KnownHeaders header) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->q_func()->header(header);
}

void QNetworkAccessBackend::setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->setCookedHeader(header, value);
}

bool QNetworkAccessBackend::hasRawHeader(const QByteArray &headerName) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->q_func()->hasRawHeader(headerName);
}

QByteArray QNetworkAccessBackend::rawHeader(const QByteArray &headerName) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->q_func()->rawHeader(headerName);
}

QList<QByteArray> QNetworkAccessBackend::rawHeaderList() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->q_func()->rawHeaderList();
}

void QNetworkAccessBackend::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->setRawHeader(headerName, headerValue);
}

QVariant QNetworkAccessBackend::attribute(QNetworkRequest::Attribute code) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->q_func()->attribute(code);
}

void QNetworkAccessBackend::setAttribute(QNetworkRequest::Attribute code, const QVariant &value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (value.isValid())
        reply->attributes.insert(code, value);
    else
        reply->attributes.remove(code);
}
QUrl QNetworkAccessBackend::url() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return reply->url;
}

void QNetworkAccessBackend::setUrl(const QUrl &url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->url = url;
}

void QNetworkAccessBackend::finished()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->finished();

	 /*[sslab cache]*/
	 QDateTime currentDateTime = QDateTime::currentDateTime();
	 setResourceLoadEndTime(currentDateTime.currentMSecsSinceEpoch());
	 incResourceLoadTime(getResourceLoadEndTime() - getResourceLoadStartTime());
	 QString thisURL = reply->url.toString();
/*	 if (getResourceFromCache()) {
		 incFromCacheResourceLoadTime(getResourceLoadEndTime() - getResourceLoadStartTime());
		 qWarning("[resource]fromCacheResourceLoadTime:%lld", getFromCacheResourceLoadTime());
	 }*/
/*	 qWarning("[resource]URL:%s, EndTime:%lld",thisURL.toAscii().data(),getResourceLoadEndTime());
	 qWarning("[resource]ConsumedTime:%lld", getResourceLoadEndTime() - getResourceLoadStartTime());
	 qWarning("[page]LoadingTime:%lld", getResourceLoadEndTime() - getPageLoadStartTime());*/
}

void QNetworkAccessBackend::error(QNetworkReply::NetworkError code, const QString &errorString)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->error(code, errorString);
}

#ifndef QT_NO_NETWORKPROXY
void QNetworkAccessBackend::proxyAuthenticationRequired(const QNetworkProxy &proxy,
                                                        QAuthenticator *authenticator)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    manager->proxyAuthenticationRequired(this, proxy, authenticator);
}
#endif

void QNetworkAccessBackend::authenticationRequired(QAuthenticator *authenticator)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    manager->authenticationRequired(this, authenticator);
}

void QNetworkAccessBackend::cacheCredentials(QAuthenticator *authenticator)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    manager->cacheCredentials(this->reply->url, authenticator);
}

void QNetworkAccessBackend::metaDataChanged()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->metaDataChanged();
}

void QNetworkAccessBackend::redirectionRequested(const QUrl &target)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    reply->redirectionRequested(target);
}

void QNetworkAccessBackend::sslErrors(const QList<QSslError> &errors)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#ifndef QT_NO_OPENSSL
    reply->sslErrors(errors);
#else
    Q_UNUSED(errors);
#endif
}

#ifndef QT_NO_BEARERMANAGEMENT

/*!
    Starts the backend.  Returns true if the backend is started.  Returns false if the backend
    could not be started due to an unopened or roaming session.  The caller should recall this
    function once the session has been opened or the roaming process has finished.
*/
bool QNetworkAccessBackend::start()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	 /*[sslab cache]*/
	 QDateTime currentDateTime = QDateTime::currentDateTime();
	 setResourceLoadStartTime(currentDateTime.currentMSecsSinceEpoch());
	 setPageLoadStartTime(currentDateTime.currentMSecsSinceEpoch());
	 qWarning("[page]pageLoadStartTime:%lld",getPageLoadStartTime());
	 QString thisURL = reply->url.toString();
	 qWarning("[resource]URL:%s, StartTime:%lld",thisURL.toAscii().data(),getResourceLoadStartTime());

 	 if (!manager->networkSession) {
        open();
        return true;
    }

    // This is not ideal.
    const QString host = reply->url.host();
    if (host == QLatin1String("localhost") ||
        QHostAddress(host) == QHostAddress::LocalHost ||
        QHostAddress(host) == QHostAddress::LocalHostIPv6) {
        // Don't need an open session for localhost access.
        open();
        return true;
    }

    if (manager->networkSession->isOpen() &&
        manager->networkSession->state() == QNetworkSession::Connected) {
        open();
        return true;
    }

    return false;
}
#endif

QT_END_NAMESPACE
