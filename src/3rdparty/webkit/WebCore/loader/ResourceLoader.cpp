
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
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2007 Graham Dennis (graham.dennis@gmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ResourceLoader.h"

#include "ApplicationCacheHost.h"
#include "DocumentLoader.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "InspectorTimelineAgent.h"
#include "Page.h"
#include "ProgressTracker.h"
#include "ResourceHandle.h"
#include "ResourceError.h"
#include "Settings.h"
#include "SharedBuffer.h"

#include "CString.h"	//sslab.dbg

#ifndef SSLAB_SR	//sslab.sr
#define SSLAB_SR
#endif
#include <QDateTime>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <QFile>

namespace WebCore {

PassRefPtr<SharedBuffer> ResourceLoader::resourceData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_resourceData)
        return m_resourceData;

    if (ResourceHandle::supportsBufferedData()/*always false(ksm)*/ && m_handle)
        return m_handle->bufferedData();
    
    return 0;
}

ResourceLoader::ResourceLoader(Frame* frame, bool sendResourceLoadCallbacks, bool shouldContentSniff)
    : m_frame(frame)
    , m_documentLoader(frame->loader()->activeDocumentLoader())
    , m_identifier(0)
    , m_reachedTerminalState(false)
    , m_cancelled(false)
    , m_calledDidFinishLoad(false)
    , m_sendResourceLoadCallbacks(sendResourceLoadCallbacks)
    , m_shouldContentSniff(shouldContentSniff)
    , m_shouldBufferData(true)
    , m_defersLoading(frame->page()->defersLoading())
{
}

ResourceLoader::~ResourceLoader()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(m_reachedTerminalState);
}

void ResourceLoader::releaseResources()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_reachedTerminalState);
    
    // It's possible that when we release the handle, it will be
    // deallocated and release the last reference to this object.
    // We need to retain to avoid accessing the object after it
    // has been deallocated and also to avoid reentering this method.
    RefPtr<ResourceLoader> protector(this);

    m_frame = 0;
    m_documentLoader = 0;
    
    // We need to set reachedTerminalState to true before we release
    // the resources to prevent a double dealloc of WebView <rdar://problem/4372628>
    m_reachedTerminalState = true;

    m_identifier = 0;

    if (m_handle) {
        // Clear out the ResourceHandle's client so that it doesn't try to call
        // us back after we release it, unless it has been replaced by someone else.
        if (m_handle->client() == this)
            m_handle->setClient(0);
        m_handle = 0;
    }

    m_resourceData = 0;
/*#ifdef SSLAB_SR
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	if (*shdflag != 2)
		m_deferredRequest = ResourceRequest();
	shmdt(shdflag);
#endif
#ifndef SSLAB_SR*/
    m_deferredRequest = ResourceRequest();
//#endif
}

bool ResourceLoader::load(const ResourceRequest& r)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_handle);
    ASSERT(m_deferredRequest.isNull());
    ASSERT(!m_documentLoader->isSubstituteLoadPending(this));
    
    ResourceRequest clientRequest(r);
    
    // https://bugs.webkit.org/show_bug.cgi?id=26391
    // The various plug-in implementations call directly to ResourceLoader::load() instead of piping requests
    // through FrameLoader. As a result, they miss the FrameLoader::addExtraFieldsToRequest() step which sets
    // up the 1st party for cookies URL. Until plug-in implementations can be reigned in to pipe through that
    // method, we need to make sure there is always a 1st party for cookies set.
    if (clientRequest.firstPartyForCookies().isNull()) {
        if (Document* document = m_frame->document())
            clientRequest.setFirstPartyForCookies(document->firstPartyForCookies());
    }

    willSendRequest(clientRequest, ResourceResponse());

    if (clientRequest.isNull()) {
        didFail(frameLoader()->cancelledError(r));
        return false;
    }

    if (m_documentLoader->scheduleArchiveLoad(this, clientRequest, r.url()))
        return true;

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    if (m_documentLoader->applicationCacheHost()->maybeLoadResource(this, clientRequest, r.url()))
        return true;
#endif

    if (m_defersLoading) {
        m_deferredRequest = clientRequest;
        return true;
    }

    m_handle = ResourceHandle::create(clientRequest, this, m_frame.get(), m_defersLoading, m_shouldContentSniff);

    return true;
}

void ResourceLoader::setDefersLoading(bool defers)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_defersLoading = defers;
    if (m_handle)
        m_handle->setDefersLoading(defers);
    if (!defers && !m_deferredRequest.isNull()) {
        ResourceRequest request(m_deferredRequest);
        m_deferredRequest = ResourceRequest();
        load(request);
    }
}

FrameLoader* ResourceLoader::frameLoader() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame)
        return 0;
    return m_frame->loader();
}

void ResourceLoader::setShouldBufferData(bool shouldBufferData)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    m_shouldBufferData = shouldBufferData; 

    // Reset any already buffered data
    if (!m_shouldBufferData)
        m_resourceData = 0;
}
    

void ResourceLoader::addData(const char* data, int length, bool allAtOnce)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_shouldBufferData)
        return;

    if (allAtOnce) {
        m_resourceData = SharedBuffer::create(data, length);
        return;
    }
        
    if (ResourceHandle::supportsBufferedData()/*always false(ksm)*/) {
        // Buffer data only if the connection has handed us the data because is has stopped buffering it.
        if (m_resourceData)
            m_resourceData->append(data, length);
    } else {
        if (!m_resourceData)
            m_resourceData = SharedBuffer::create(data, length);
        else
            m_resourceData->append(data, length);
    }
}

void ResourceLoader::clearResourceData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_resourceData)
        m_resourceData->clear();
}

void ResourceLoader::willSendRequest(ResourceRequest& request, const ResourceResponse& redirectResponse)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);

    ASSERT(!m_reachedTerminalState);

#ifdef SSLAB_SR	
	QDateTime currentTime = QDateTime::currentDateTime();
	qint64 startTime = currentTime.currentMSecsSinceEpoch();
	
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	int sr_flag = *shdflag;
	shmdt(shdflag);

	int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
	char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
	char readyUrl[4096] = {0,};
	strcpy(readyUrl, shdurl);
	shmdt(shdurl);
	
	if ((sr_flag == 1 || sr_flag == 3) && !strcmp(request.url().string().utf8().data(),readyUrl) && strlen(readyUrl)) { //Estimation in progress.
		if (sr_flag == 1) {
			/* increment to total estimation count */
			int seg_totalEstCnt = shmget(6666, PAGE_SIZE, 0666);
			long long int* shdtotalEstCnt = (long long int*)shmat(seg_totalEstCnt, NULL, 0);
			(*shdtotalEstCnt)++;
			shmdt(shdtotalEstCnt);
		}

		int seg_startTime = shmget(4444, PAGE_SIZE, 0666);
		long long int* shdstartTime = (long long int*)shmat(seg_startTime, NULL, 0);
		if (*shdstartTime == 0) *shdstartTime = startTime;
		shmdt(shdstartTime);
		qWarning("[SR-time]startTime:%lld, url:%s", startTime, request.url().string().utf8().data());
	}
	
	//m_startTime = startTime;
	char buff[4096] = {0,};
		
	if (sr_flag == 3 && !strcmp(request.url().string().utf8().data(),readyUrl) && strlen(readyUrl))
		sprintf(buff, "---(SR-Net)--->Resource URL: %s< :: Load Start Time: %lld ms \n", request.url().string().utf8().data(), startTime);
	else sprintf(buff, ">Resource URL: %s< :: Load Start Time: %lld ms \n", request.url().string().utf8().data(), startTime);
#if 0
	QFile outputFile(QLatin1String("/mnt/ramdisk/log.output"));
	if (!outputFile.open(QFile::Append)) {
		qWarning("[sslab] outputFile open error!!");
		exit(-1);
	}
	outputFile.write(buff, strlen(buff));
	outputFile.close();
#endif
#endif

    if (m_sendResourceLoadCallbacks) {
        if (!m_identifier) {
            m_identifier = m_frame->page()->progress()->createUniqueIdentifier();
            frameLoader()->notifier()->assignIdentifierToInitialRequest(m_identifier, documentLoader(), request);
        }

        frameLoader()->notifier()->willSendRequest(this, request, redirectResponse);
    }

    m_request = request;
#ifdef SSLAB__DEBUG
	qWarning("[url:%s] willSendRequest in ResourceLoader", m_request.url().string().utf8().data());	//sslab.dbg
#endif

}

void ResourceLoader::didSendData(unsigned long long, unsigned long long)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void ResourceLoader::didReceiveResponse(const ResourceResponse& r)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_reachedTerminalState);

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);

    m_response = r;

    if (FormData* data = m_request.httpBody())
        data->removeGeneratedFilesIfNeeded();
        
    if (m_sendResourceLoadCallbacks)
        frameLoader()->notifier()->didReceiveResponse(this, m_response);
}

void ResourceLoader::didReceiveData(const char* data, int length, long long lengthReceived, bool allAtOnce)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // The following assertions are not quite valid here, since a subclass
    // might override didReceiveData in a way that invalidates them. This
    // happens with the steps listed in 3266216
    // ASSERT(con == connection);
    // ASSERT(!m_reachedTerminalState);

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);
#ifdef SSLAB__DEBUG
	qWarning("[url:%s] addData in ResourceLoader", m_request.url().string().utf8().data());	//sslab.dbg
#endif
    addData(data, length, allAtOnce);
    // FIXME: If we get a resource with more than 2B bytes, this code won't do the right thing.
    // However, with today's computers and networking speeds, this won't happen in practice.
    // Could be an issue with a giant local file.
    if (m_sendResourceLoadCallbacks && m_frame)
        frameLoader()->notifier()->didReceiveData(this, data, length, static_cast<int>(lengthReceived));
}

void ResourceLoader::willStopBufferingData(const char* data, int length)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_shouldBufferData)
        return;

    ASSERT(!m_resourceData);
    m_resourceData = SharedBuffer::create(data, length);
}

void ResourceLoader::didFinishLoading()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // If load has been cancelled after finishing (which could happen with a 
    // JavaScript that changes the window location), do nothing.
    if (m_cancelled)
        return;
    ASSERT(!m_reachedTerminalState);

    didFinishLoadingOnePart();
    releaseResources();
}

void ResourceLoader::didFinishLoadingOnePart()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_cancelled)
        return;
    ASSERT(!m_reachedTerminalState);

    if (m_calledDidFinishLoad)
        return;
    m_calledDidFinishLoad = true;
    if (m_sendResourceLoadCallbacks)
        frameLoader()->notifier()->didFinishLoad(this);
}

void ResourceLoader::didFail(const ResourceError& error)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_cancelled)
        return;
    ASSERT(!m_reachedTerminalState);

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);

    if (FormData* data = m_request.httpBody())
        data->removeGeneratedFilesIfNeeded();

    if (m_sendResourceLoadCallbacks && !m_calledDidFinishLoad)
        frameLoader()->notifier()->didFailToLoad(this, error);

    releaseResources();
}

void ResourceLoader::didCancel(const ResourceError& error)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_cancelled);
    ASSERT(!m_reachedTerminalState);

    if (FormData* data = m_request.httpBody())
        data->removeGeneratedFilesIfNeeded();

    // This flag prevents bad behavior when loads that finish cause the
    // load itself to be cancelled (which could happen with a javascript that 
    // changes the window location). This is used to prevent both the body
    // of this method and the body of connectionDidFinishLoading: running
    // for a single delegate. Canceling wins.
/*#ifdef SSLAB_SR
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	int sr_flag = *shdflag;
	shmdt(shdflag);
	if (sr_flag != 2) m_cancelled = true;
#endif
#ifndef SSLAB_SR*/
    m_cancelled = true;
//#endif
    if (m_handle)
        m_handle->clearAuthentication();

	m_documentLoader->cancelPendingSubstituteLoad(this);
	if (m_handle) {
		m_handle->cancel();
		m_handle = 0;
	}
/*#ifdef SSLAB_SR
	if (sr_flag != 2 && m_sendResourceLoadCallbacks && !m_calledDidFinishLoad)
		frameLoader()->notifier()->didFailToLoad(this, error);
#endif
#ifndef SSLAB_SR*/
	if (m_sendResourceLoadCallbacks && !m_calledDidFinishLoad)
		frameLoader()->notifier()->didFailToLoad(this, error);
//#endif

	releaseResources();
}

void ResourceLoader::cancel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    cancel(ResourceError());
}

void ResourceLoader::cancel(const ResourceError& error)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_reachedTerminalState)
        return;
    if (!error.isNull())
        didCancel(error);
    else
        didCancel(cancelledError());
}

const ResourceResponse& ResourceLoader::response() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_response;
}

ResourceError ResourceLoader::cancelledError()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return frameLoader()->cancelledError(m_request);
}

ResourceError ResourceLoader::blockedError()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return frameLoader()->blockedError(m_request);
}

ResourceError ResourceLoader::cannotShowURLError()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return frameLoader()->cannotShowURLError(m_request);
}

void ResourceLoader::willSendRequest(ResourceHandle*, ResourceRequest& request, const ResourceResponse& redirectResponse)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    if (documentLoader()->applicationCacheHost()->maybeLoadFallbackForRedirect(this, request, redirectResponse))
        return;
#endif
    willSendRequest(request, redirectResponse);
}

void ResourceLoader::didSendData(ResourceHandle*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didSendData(bytesSent, totalBytesToBeSent);
}

void ResourceLoader::didReceiveResponse(ResourceHandle*, const ResourceResponse& response)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if ENABLE(INSPECTOR)
    if (InspectorTimelineAgent::instanceCount()) {
        InspectorTimelineAgent* timelineAgent = m_frame->page() ? m_frame->page()->inspectorTimelineAgent() : 0;
        if (timelineAgent)
            timelineAgent->willReceiveResourceResponse(identifier(), response);
    }
#endif
#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    if (documentLoader()->applicationCacheHost()->maybeLoadFallbackForResponse(this, response))
        return;
#endif
    didReceiveResponse(response);
#if ENABLE(INSPECTOR)
    if (InspectorTimelineAgent::instanceCount()) {
        InspectorTimelineAgent* timelineAgent = m_frame->page() ? m_frame->page()->inspectorTimelineAgent() : 0;
        if (timelineAgent)
            timelineAgent->didReceiveResourceResponse();
    }
#endif
}

void ResourceLoader::didReceiveData(ResourceHandle*, const char* data, int length, int lengthReceived)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if ENABLE(INSPECTOR)
    if (InspectorTimelineAgent::instanceCount()) {
        InspectorTimelineAgent* timelineAgent = m_frame->page() ? m_frame->page()->inspectorTimelineAgent() : 0;
        if (timelineAgent)
            timelineAgent->willReceiveResourceData(identifier());
    }
#endif
    didReceiveData(data, length, lengthReceived, false);
#if ENABLE(INSPECTOR)
    if (InspectorTimelineAgent::instanceCount()) {
        InspectorTimelineAgent* timelineAgent = m_frame->page() ? m_frame->page()->inspectorTimelineAgent() : 0;
        if (timelineAgent)
            timelineAgent->didReceiveResourceData();
    }
#endif
}

void ResourceLoader::didFinishLoading(ResourceHandle*)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

/*#ifdef SSLAB_SR		//sslab.sr
	QFile outputFile(QLatin1String("/mnt/ramdisk/log.output"));
	if (!outputFile.open(QFile::Append)) {
		qWarning("[sslab] outputFile open error!!");
		exit(-1);
	}
	QDateTime currentTime = QDateTime::currentDateTime();
	qint64 endTime = currentTime.currentMSecsSinceEpoch();
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);

	int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
	char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
	char readyUrl[4096] = {0,};
	strcpy(readyUrl, shdurl);
	int urlLength = strlen(readyUrl);
	shmdt(shdurl);

	if (*shdflag == 0 || !urlLength) {		//Normal
		shmdt(shdflag);
		int seg_resourceCnt = shmget(3333, PAGE_SIZE, 0666);
		long long int* shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
		int resourceCnt = (*shdresCnt)++;
		shmdt(shdresCnt);

		//			totalResourceLoadTime += endTime - m_startTime;
		char buff[4096] = {0,};
		sprintf(buff, ">Resource URL: %s< :: Load End Time: %lld ms\n", m_request.url().string().utf8().data(), endTime);
		//			sprintf(buff, "%s>Avg.ResourceLoadTime: %.2f ms\n", buff, (double)totalResourceLoadTime / (double)resourceCnt);
		outputFile.write(buff, strlen(buff));
		outputFile.close();
	}
	else if (*shdflag == 1 && urlLength) {		//Estimating...(load from cache)
		//confirm seg_url(must img resource), write time to seg_cachetime, flag = 2, re-request
		qWarning("[what the readyUrl: %s", readyUrl);
		qWarning("[url(): %s", m_request.url().string().utf8().data());

		if (!strcmp(readyUrl, m_request.url().string().utf8().data())) {
			int seg_cacheTime = shmget(4444, PAGE_SIZE, 0666);
			long long int* shdcacheTime = (long long int*)shmat(seg_cacheTime, NULL, 0);
			*shdcacheTime = endTime - m_startTime;
			shmdt(shdcacheTime);
			*shdflag = 2;
			shmdt(shdflag);
			//re-request
			//didCancel(ResourceError());
			cancel();
			//		releaseResources();
			//load(ResourceRequest());
		}
		else shmdt(shdflag);
	}
	else if (*shdflag == 2 && urlLength) {							//Estimating...(load from network)
		//confirm seg_url(same img resource in "case 1"), cache time compare with network time => set seg_sr, flag = 0

		if (!strcmp(readyUrl, m_request.url().string().utf8().data())) {
			qint64 networkTime = endTime - m_startTime;
			int seg_cacheTime = shmget(4444, PAGE_SIZE, IPC_CREAT |	S_IRUSR | S_IWUSR);
			long long int* shdcacheTime = (long long int*)shmat(seg_cacheTime, NULL, 0);
			int seg_sr = shmget(1234, PAGE_SIZE, 0666);
			int* shdsr = (int*)shmat(seg_sr, NULL, 0);
			if (networkTime > *shdcacheTime) *shdsr = 1;
			else *shdsr = 2;
			shmdt(shdsr);
			shmdt(shdcacheTime);
			*shdflag = 0;
		}
		shmdt(shdflag);	
	}
	else //can't happen!!
		qWarning("error occured. in ResourceLoader::didFinishLoading()");
#endif*/

#ifdef SSLAB__DEBUG
	qWarning("[url:%s] didFinishLoading in ResourceLoader", m_request.url().string().utf8().data());	//sslab.dbg
#endif
    didFinishLoading();
}

void ResourceLoader::didFail(ResourceHandle*, const ResourceError& error)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    if (documentLoader()->applicationCacheHost()->maybeLoadFallbackForError(this, error))
        return;
#endif
    didFail(error);
}

void ResourceLoader::wasBlocked(ResourceHandle*)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didFail(blockedError());
}

void ResourceLoader::cannotShowURL(ResourceHandle*)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didFail(cannotShowURLError());
}

bool ResourceLoader::shouldUseCredentialStorage()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<ResourceLoader> protector(this);
    return frameLoader()->shouldUseCredentialStorage(this);
}

void ResourceLoader::didReceiveAuthenticationChallenge(const AuthenticationChallenge& challenge)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);
    frameLoader()->notifier()->didReceiveAuthenticationChallenge(this, challenge);
}

void ResourceLoader::didCancelAuthenticationChallenge(const AuthenticationChallenge& challenge)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // Protect this in this delegate method since the additional processing can do
    // anything including possibly derefing this; one example of this is Radar 3266216.
    RefPtr<ResourceLoader> protector(this);
    frameLoader()->notifier()->didCancelAuthenticationChallenge(this, challenge);
}

void ResourceLoader::receivedCancellation(const AuthenticationChallenge&)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    cancel();
}

void ResourceLoader::willCacheResponse(ResourceHandle*, CacheStoragePolicy& policy)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // When in private browsing mode, prevent caching to disk
    if (policy == StorageAllowed && m_frame->settings()->privateBrowsingEnabled())
        policy = StorageAllowedInMemoryOnly;    
}

}
