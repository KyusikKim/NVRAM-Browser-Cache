
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
/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ResourceHandle.h"

#include "ChromeClientQt.h"
#include "DocLoader.h"
#include "Frame.h"
#include "FrameLoaderClientQt.h"
#include "NotImplemented.h"
#include "Page.h"
#include "QNetworkReplyHandler.h"
#include "ResourceHandleClient.h"
#include "ResourceHandleInternal.h"
#include "SharedBuffer.h"

// FIXME: WebCore including these headers from WebKit is a massive layering violation.
#include "qwebframe_p.h"
#include "qwebpage_p.h"

#include <QAbstractNetworkCache>
#include <QCoreApplication>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace WebCore {

class WebCoreSynchronousLoader : public ResourceHandleClient {
public:
    WebCoreSynchronousLoader();

    void waitForCompletion();

    virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&);
    virtual void didReceiveData(ResourceHandle*, const char*, int, int lengthReceived);
    virtual void didFinishLoading(ResourceHandle*);
    virtual void didFail(ResourceHandle*, const ResourceError&);

    ResourceResponse resourceResponse() const { return m_response; }
    ResourceError resourceError() const { return m_error; }
    Vector<char> data() const { return m_data; }

private:
    ResourceResponse m_response;
    ResourceError m_error;
    Vector<char> m_data;
    QEventLoop m_eventLoop;
};

WebCoreSynchronousLoader::WebCoreSynchronousLoader()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void WebCoreSynchronousLoader::didReceiveResponse(ResourceHandle*, const ResourceResponse& response)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_response = response;
}

void WebCoreSynchronousLoader::didReceiveData(ResourceHandle*, const char* data, int length, int)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_data.append(data, length);
}

void WebCoreSynchronousLoader::didFinishLoading(ResourceHandle*)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_eventLoop.exit();
}

void WebCoreSynchronousLoader::didFail(ResourceHandle*, const ResourceError& error)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_error = error;
    m_eventLoop.exit();
}

void WebCoreSynchronousLoader::waitForCompletion()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

ResourceHandleInternal::~ResourceHandleInternal()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

ResourceHandle::~ResourceHandle()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (d->m_job)
        cancel();
}

bool ResourceHandle::start(Frame* frame)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!frame)
        return false;

    Page *page = frame->page();
    // If we are no longer attached to a Page, this must be an attempted load from an
    // onUnload handler, so let's just block it.
    if (!page)
        return false;

    if (!(d->m_user.isEmpty() || d->m_pass.isEmpty())) {
        // If credentials were specified for this request, add them to the url,
        // so that they will be passed to QNetworkRequest.
        KURL urlWithCredentials(d->m_request.url());
        urlWithCredentials.setUser(d->m_user);
        urlWithCredentials.setPass(d->m_pass);
        d->m_request.setURL(urlWithCredentials);
    }

    getInternal()->m_frame = static_cast<FrameLoaderClientQt*>(frame->loader()->client())->webFrame();
    ResourceHandleInternal *d = getInternal();
    d->m_job = new QNetworkReplyHandler(this, QNetworkReplyHandler::LoadMode(d->m_defersLoading));
    return true;
}

void ResourceHandle::cancel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (d->m_job) {
        d->m_job->abort();
        d->m_job = 0;
    }
}

bool ResourceHandle::loadsBlocked()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

bool ResourceHandle::willLoadFromCache(ResourceRequest& request, Frame* frame)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!frame)
        return false;

    QNetworkAccessManager* manager = QWebFramePrivate::kit(frame)->page()->networkAccessManager();
    QAbstractNetworkCache* cache = manager->cache();

    if (!cache)
        return false;

    QNetworkCacheMetaData data = cache->metaData(request.url());
    if (data.isValid()) {
        request.setCachePolicy(ReturnCacheDataDontLoad);
        return true;
    }

    return false;
}

bool ResourceHandle::supportsBufferedData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

PassRefPtr<SharedBuffer> ResourceHandle::bufferedData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT_NOT_REACHED();
    return 0;
}

void ResourceHandle::loadResourceSynchronously(const ResourceRequest& request, StoredCredentials /*storedCredentials*/, ResourceError& error, ResourceResponse& response, Vector<char>& data, Frame* frame)
{
    WebCoreSynchronousLoader syncLoader;
    ResourceHandle handle(request, &syncLoader, true, false);

    ResourceHandleInternal *d = handle.getInternal();
    if (!(d->m_user.isEmpty() || d->m_pass.isEmpty())) {
        // If credentials were specified for this request, add them to the url,
        // so that they will be passed to QNetworkRequest.
        KURL urlWithCredentials(d->m_request.url());
        urlWithCredentials.setUser(d->m_user);
        urlWithCredentials.setPass(d->m_pass);
        d->m_request.setURL(urlWithCredentials);
    }
    d->m_frame = static_cast<FrameLoaderClientQt*>(frame->loader()->client())->webFrame();
    d->m_job = new QNetworkReplyHandler(&handle, QNetworkReplyHandler::LoadNormal);

    syncLoader.waitForCompletion();
    error = syncLoader.resourceError();
    data = syncLoader.data();
    response = syncLoader.resourceResponse();
}

 
void ResourceHandle::setDefersLoading(bool defers)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    d->m_defersLoading = defers;

    if (d->m_job)
        d->m_job->setLoadMode(QNetworkReplyHandler::LoadMode(defers));
}

} // namespace WebCore
