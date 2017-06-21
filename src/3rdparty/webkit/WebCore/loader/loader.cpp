
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
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "loader.h"

#include "Cache.h"
#include "CachedImage.h"
#include "CachedResource.h"
#include "CString.h"
#include "DocLoader.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "HTMLDocument.h"
#include "Request.h"
#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "SecurityOrigin.h"
#include "SharedBuffer.h"
#include "SubresourceLoader.h"
#include <wtf/Assertions.h>
#include <wtf/Vector.h>

#define REQUEST_MANAGEMENT_ENABLED 1
#define REQUEST_DEBUG 0

#ifndef SSLAB_SR
#define SSLAB_SR
#endif
#include "CString.h"
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <QFile>
#include <QDateTime>

namespace WebCore {

#if REQUEST_MANAGEMENT_ENABLED
// Match the parallel connection count used by the networking layer
static unsigned maxRequestsInFlightPerHost;
// Having a limit might still help getting more important resources first
static const unsigned maxRequestsInFlightForNonHTTPProtocols = 20;
#else
static const unsigned maxRequestsInFlightPerHost = 10000;
static const unsigned maxRequestsInFlightForNonHTTPProtocols = 10000;
#endif

Loader::Loader()
    : m_requestTimer(this, &Loader::requestTimerFired)
    , m_isSuspendingPendingRequests(false)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_nonHTTPProtocolHost = Host::create(AtomicString(), maxRequestsInFlightForNonHTTPProtocols);
#if REQUEST_MANAGEMENT_ENABLED
    maxRequestsInFlightPerHost = initializeMaximumHTTPConnectionCountPerHost();
#endif
}

Loader::~Loader()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
    
    ASSERT_NOT_REACHED();
}
    
static ResourceRequest::TargetType cachedResourceTypeToTargetType(CachedResource::Type type)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    switch (type) {
    case CachedResource::CSSStyleSheet:
#if ENABLE(XSLT)
    case CachedResource::XSLStyleSheet:
#endif
#if ENABLE(XBL)
    case CachedResource::XBL:
#endif
        return ResourceRequest::TargetIsStyleSheet;
    case CachedResource::Script: 
        return ResourceRequest::TargetIsScript;
    case CachedResource::FontResource:
        return ResourceRequest::TargetIsFontResource;
    case CachedResource::ImageResource:
        return ResourceRequest::TargetIsImage;
    }
    return ResourceRequest::TargetIsSubresource;
}

Loader::Priority Loader::determinePriority(const CachedResource* resource) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if REQUEST_MANAGEMENT_ENABLED
    switch (resource->type()) {
        case CachedResource::CSSStyleSheet:
#if ENABLE(XSLT)
        case CachedResource::XSLStyleSheet:
#endif
#if ENABLE(XBL)
        case CachedResource::XBL:
#endif
            return High;
        case CachedResource::Script: 
        case CachedResource::FontResource:
            return Medium;
        case CachedResource::ImageResource:
            return Low;
    }
    ASSERT_NOT_REACHED();
    return Low;
#else
    return High;
#endif
}

void Loader::load(DocLoader* docLoader, CachedResource* resource, bool incremental, SecurityCheckPolicy securityCheck, bool sendResourceLoadCallbacks)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(docLoader);
    Request* request = new Request(docLoader, resource, incremental, securityCheck, sendResourceLoadCallbacks);
#ifdef SSLAB_SR
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	int sr_flag = *shdflag;
	shmdt(shdflag);

	if (sr_flag == 1 && resource->type() == CachedResource::ImageResource) {	//sslab.sr
		int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
		char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
		if (!strlen(shdurl)) {
			strcpy(shdurl, resource->url().latin1().data());
			srDocLoader = docLoader;
			srResource = resource;
			srIncremental = incremental;
			srSecurityCheck = securityCheck;
			srSendResourceLoadCallbacks = sendResourceLoadCallbacks;
		}
		shmdt(shdurl);
	}
/*	else if (sr_flag != 0) {
		srDocLoader = NULL;
		srResource = NULL;
	}*/
#endif
    RefPtr<Host> host;
    KURL url(ParsedURLString, resource->url());
    if (url.protocolInHTTPFamily()) {
        m_hosts.checkConsistency();
        AtomicString hostName = url.host();
        host = m_hosts.get(hostName.impl());
        if (!host) {
            host = Host::create(hostName, maxRequestsInFlightPerHost);
            m_hosts.add(hostName.impl(), host);
        }
    } else 
        host = m_nonHTTPProtocolHost;
    
    bool hadRequests = host->hasRequests();
    Priority priority = determinePriority(resource);
    host->addRequest(request, priority);
    docLoader->incrementRequestCount();

    if (priority > Low || !url.protocolInHTTPFamily() || !hadRequests) {
        // Try to request important resources immediately
        host->servePendingRequests(priority);
    } else {
        // Handle asynchronously so early low priority requests don't get scheduled before later high priority ones
        scheduleServePendingRequests();
    }
}

#ifdef SSLAB_SR
void Loader::reLoad()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

	DocLoader* docLoader = srDocLoader;
	CachedResource* resource = srResource;
	bool incremental = srIncremental;
   	SecurityCheckPolicy securityCheck = srSecurityCheck;
   	bool sendResourceLoadCallbacks = srSendResourceLoadCallbacks;

    ASSERT(docLoader);

    Request* request = new Request(docLoader, resource, incremental, securityCheck, sendResourceLoadCallbacks);

    RefPtr<Host> host;
    KURL url(ParsedURLString, resource->url());
    if (url.protocolInHTTPFamily()) {
        m_hosts.checkConsistency();
        AtomicString hostName = url.host();
        host = m_hosts.get(hostName.impl());
        if (!host) {
            host = Host::create(hostName, maxRequestsInFlightPerHost);
            m_hosts.add(hostName.impl(), host);
        }
    } else {
        host = m_nonHTTPProtocolHost;
	}
    
	bool hadRequests = host->hasRequests();
    Priority priority = determinePriority(resource);
    host->addRequest(request, priority);
    docLoader->incrementRequestCount();

    if (priority > Low || !url.protocolInHTTPFamily() || !hadRequests) {
        // Try to request important resources immediately
        host->servePendingRequests(priority);
    } else {
        // Handle asynchronously so early low priority requests don't get scheduled before later high priority ones
        scheduleServePendingRequests();
    }
}
#endif

    
void Loader::scheduleServePendingRequests()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_requestTimer.isActive())
        m_requestTimer.startOneShot(0);
}

void Loader::requestTimerFired(Timer<Loader>*) 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    servePendingRequests();
}

void Loader::servePendingRequests(Priority minimumPriority)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_isSuspendingPendingRequests)
        return;

    m_requestTimer.stop();
    
    m_nonHTTPProtocolHost->servePendingRequests(minimumPriority);

    Vector<Host*> hostsToServe;
    m_hosts.checkConsistency();
    HostMap::iterator i = m_hosts.begin();
    HostMap::iterator end = m_hosts.end();
    for (;i != end; ++i)
        hostsToServe.append(i->second.get());
        
    for (unsigned n = 0; n < hostsToServe.size(); ++n) {
        Host* host = hostsToServe[n];
        if (host->hasRequests())
            host->servePendingRequests(minimumPriority);
        else if (!host->processingResource()){
            AtomicString name = host->name();
            m_hosts.remove(name.impl());
        }
    }
}

void Loader::suspendPendingRequests()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_isSuspendingPendingRequests);
    m_isSuspendingPendingRequests = true;
}

void Loader::resumePendingRequests()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(m_isSuspendingPendingRequests);
    m_isSuspendingPendingRequests = false;
    if (!m_hosts.isEmpty() || m_nonHTTPProtocolHost->hasRequests())
        scheduleServePendingRequests();
}

void Loader::nonCacheRequestInFlight(const KURL& url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!url.protocolInHTTPFamily())
        return;
    
    AtomicString hostName = url.host();
    m_hosts.checkConsistency();
    RefPtr<Host> host = m_hosts.get(hostName.impl());
    if (!host) {
        host = Host::create(hostName, maxRequestsInFlightPerHost);
        m_hosts.add(hostName.impl(), host);
    }

    host->nonCacheRequestInFlight();
}

void Loader::nonCacheRequestComplete(const KURL& url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!url.protocolInHTTPFamily())
        return;
    
    AtomicString hostName = url.host();
    m_hosts.checkConsistency();
    RefPtr<Host> host = m_hosts.get(hostName.impl());
    ASSERT(host);
    if (!host)
        return;

    host->nonCacheRequestComplete();
}

void Loader::cancelRequests(DocLoader* docLoader)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    docLoader->clearPendingPreloads();

    if (m_nonHTTPProtocolHost->hasRequests())
        m_nonHTTPProtocolHost->cancelRequests(docLoader);
    
    Vector<Host*> hostsToCancel;
    m_hosts.checkConsistency();
    HostMap::iterator i = m_hosts.begin();
    HostMap::iterator end = m_hosts.end();
    for (;i != end; ++i)
        hostsToCancel.append(i->second.get());

    for (unsigned n = 0; n < hostsToCancel.size(); ++n) {
        Host* host = hostsToCancel[n];
        if (host->hasRequests())
            host->cancelRequests(docLoader);
    }

    scheduleServePendingRequests();
    
    ASSERT(docLoader->requestCount() == (docLoader->loadInProgress() ? 1 : 0));
}

Loader::Host::Host(const AtomicString& name, unsigned maxRequestsInFlight)
    : m_name(name)
    , m_maxRequestsInFlight(maxRequestsInFlight)
    , m_numResourcesProcessing(0)
    , m_nonCachedRequestsInFlight(0)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

Loader::Host::~Host()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(m_requestsLoading.isEmpty());
    for (unsigned p = 0; p <= High; p++)
        ASSERT(m_requestsPending[p].isEmpty());
}
    
void Loader::Host::addRequest(Request* request, Priority priority)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_requestsPending[priority].append(request);
}
    
void Loader::Host::nonCacheRequestInFlight()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ++m_nonCachedRequestsInFlight;
}

void Loader::Host::nonCacheRequestComplete()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    --m_nonCachedRequestsInFlight;
    ASSERT(m_nonCachedRequestsInFlight >= 0);

    cache()->loader()->scheduleServePendingRequests();
}

bool Loader::Host::hasRequests() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_requestsLoading.isEmpty())
        return true;
    for (unsigned p = 0; p <= High; p++) {
        if (!m_requestsPending[p].isEmpty())
            return true;
    }
    return false;
}

void Loader::Host::servePendingRequests(Loader::Priority minimumPriority)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (cache()->loader()->isSuspendingPendingRequests())
        return;

    bool serveMore = true;
    for (int priority = High; priority >= minimumPriority && serveMore; --priority)
        servePendingRequests(m_requestsPending[priority], serveMore);
}

void Loader::Host::servePendingRequests(RequestQueue& requestsPending, bool& serveLowerPriority)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    while (!requestsPending.isEmpty()) {        
        Request* request = requestsPending.first();
        DocLoader* docLoader = request->docLoader();
        bool resourceIsCacheValidator = request->cachedResource()->isCacheValidator();

        // For named hosts - which are only http(s) hosts - we should always enforce the connection limit.
        // For non-named hosts - everything but http(s) - we should only enforce the limit if the document isn't done parsing 
        // and we don't know all stylesheets yet.
        bool shouldLimitRequests = !m_name.isNull() || docLoader->doc()->parsing() || !docLoader->doc()->haveStylesheetsLoaded();
        if (shouldLimitRequests && m_requestsLoading.size() + m_nonCachedRequestsInFlight >= m_maxRequestsInFlight) {
            serveLowerPriority = false;
            return;
        }
        requestsPending.removeFirst();
        
        ResourceRequest resourceRequest(request->cachedResource()->url());
        resourceRequest.setTargetType(cachedResourceTypeToTargetType(request->cachedResource()->type()));
        
        if (!request->cachedResource()->accept().isEmpty())
            resourceRequest.setHTTPAccept(request->cachedResource()->accept());
        
         // Do not set the referrer or HTTP origin here. That's handled by SubresourceLoader::create.
        
        if (resourceIsCacheValidator) {
            CachedResource* resourceToRevalidate = request->cachedResource()->resourceToRevalidate();
            ASSERT(resourceToRevalidate->canUseCacheValidator());
            ASSERT(resourceToRevalidate->isLoaded());
            const String& lastModified = resourceToRevalidate->response().httpHeaderField("Last-Modified");
            const String& eTag = resourceToRevalidate->response().httpHeaderField("ETag");
            if (!lastModified.isEmpty() || !eTag.isEmpty()) {
                ASSERT(docLoader->cachePolicy() != CachePolicyReload);
                if (docLoader->cachePolicy() == CachePolicyRevalidate)
                    resourceRequest.setHTTPHeaderField("Cache-Control", "max-age=0");
                if (!lastModified.isEmpty())
                    resourceRequest.setHTTPHeaderField("If-Modified-Since", lastModified);
                if (!eTag.isEmpty())
                    resourceRequest.setHTTPHeaderField("If-None-Match", eTag);
            }
        }

        RefPtr<SubresourceLoader> loader = SubresourceLoader::create(docLoader->doc()->frame(),
            this, resourceRequest, request->shouldDoSecurityCheck(), request->sendResourceLoadCallbacks());
        if (loader) {
            m_requestsLoading.add(loader.release(), request);
            request->cachedResource()->setRequestedFromNetworkingLayer();
#if REQUEST_DEBUG
            printf("HOST %s COUNT %d LOADING %s\n", resourceRequest.url().host().latin1().data(), m_requestsLoading.size(), request->cachedResource()->url().latin1().data());
#endif
        } else {            
            docLoader->decrementRequestCount();
            docLoader->setLoadInProgress(true);
            request->cachedResource()->error();
            docLoader->setLoadInProgress(false);
            delete request;
        }
    }
}

void Loader::Host::didFinishLoading(SubresourceLoader* loader)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<Host> myProtector(this);

    RequestMap::iterator i = m_requestsLoading.find(loader);
    if (i == m_requestsLoading.end())
        return;
    
    Request* request = i->second;
    m_requestsLoading.remove(i);
    DocLoader* docLoader = request->docLoader();
    // Prevent the document from being destroyed before we are done with
    // the docLoader that it will delete when the document gets deleted.
    RefPtr<Document> protector(docLoader->doc());
    if (!request->isMultipart())
        docLoader->decrementRequestCount();

    CachedResource* resource = request->cachedResource();
    ASSERT(!resource->resourceToRevalidate());

#ifdef SSLAB_SR
//#ifdef SSLAB_SAVE
	int seg_flag = shmget(1111, PAGE_SIZE, 0666);
	int* shdflag = (int*)shmat(seg_flag, NULL, 0);
	int sr_flag = *shdflag;
	shmdt(shdflag);

	int seg_readyUrl = shmget(2222, sizeof(char)*4096, 0666);
	char* shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
	char readyUrl[4096] = {0,};
	strcpy(readyUrl, shdurl);
	shmdt(shdurl);

	qWarning("[SR-URLs(loader) url:%s, readyUrl:%s",resource->url().latin1().data(), readyUrl);
	if (sr_flag == 2 && !strcmp(resource->url().latin1().data(), readyUrl) && strlen(readyUrl)) {
		qint64 endTime = QDateTime::currentDateTime().currentMSecsSinceEpoch();
		int seg_startTime = shmget(4444, PAGE_SIZE, 0666);
		long long int* shdstartTime = (long long int*)shmat(seg_startTime, NULL, 0);
		cacheTime = endTime - (*shdstartTime);
//		*shdstartTime = endTime;
		qWarning("[SR-time]endTime:%lld, - startTime:%lld", endTime, *shdstartTime);
		*shdstartTime = 0;
		shmdt(shdstartTime);
		
		qWarning("[SR-time]cacheTime:%lld, url:%s", cacheTime, resource->url().latin1().data());	

		seg_flag = shmget(1111, PAGE_SIZE, 0666);
		shdflag = (int*)shmat(seg_flag, NULL, 0);
		*shdflag = 3;
		qWarning("[SR-test] sr_flag was set to %d", *shdflag);
		shmdt(shdflag);

		//re-load (copy from load() function)
		clearForReload(request, loader, false);
		cache()->loader()->reLoad();
	}
	else if (sr_flag == 3 && !strcmp(resource->url().latin1().data(), readyUrl) && strlen(readyUrl)) {
		//end of estimation
		qint64 endTime = QDateTime::currentDateTime().currentMSecsSinceEpoch();
		int seg_startTime = shmget(4444, PAGE_SIZE, 0666);
		long long int* shdstartTime = (long long int*)shmat(seg_startTime, NULL, 0);
		qint64 networkTime = endTime - (*shdstartTime);
		qWarning("[SR-time]endTime:%lld, - startTime:%lld", endTime, *shdstartTime);
		*shdstartTime = 0;
		shmdt(shdstartTime);

		int seg_sr = shmget(1234, PAGE_SIZE, 0666);
		int* shdsr = (int*)shmat(seg_sr, NULL, 0);
		qWarning("[SR-time]netTime:%lld, cacheTime:%lld, url:%s", networkTime, cacheTime, resource->url().latin1().data());
		if (networkTime > cacheTime) {
			*shdsr = 1;	//cache is faster!!
			//total - net_count = cache selection count.
		}
		else {
		  	*shdsr = 2;	//network is faster!!

			int seg_selecNetCnt = shmget(7777, PAGE_SIZE, 0666);
			long long int* shdselecNetCnt = (long long int*)shmat(seg_selecNetCnt, NULL, 0);
			(*shdselecNetCnt)++;	//increment to network selection count
			shmdt(shdselecNetCnt);
		}
		shmdt(shdsr);

		seg_flag = shmget(1111, PAGE_SIZE, 0666);
		shdflag = (int*)shmat(seg_flag, NULL, 0);
		*shdflag = 0;
		qWarning("[SR-test] sr_flag was set to %d", *shdflag);
		shmdt(shdflag);
		
		cacheTime = 0;

		if (!resource->errorOccurred()) {
			int seg_resourceCnt = shmget(3333, PAGE_SIZE, 0666);
			long long int* shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
			(*shdresCnt)++;
			shmdt(shdresCnt);

			docLoader->setLoadInProgress(true);
			resource->data(loader->resourceData(), true);
			resource->finish();
		}
	}
	else {
		if (!resource->errorOccurred()) {
			int seg_resourceCnt = shmget(3333, PAGE_SIZE, 0666);
			long long int* shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
			(*shdresCnt)++;
			shmdt(shdresCnt);

			docLoader->setLoadInProgress(true);
			resource->data(loader->resourceData(), true);
			resource->finish();
		}
	}
//#endif
#endif
#ifndef SSLAB_SR
    // If we got a 4xx response, we're pretending to have received a network
    // error, so we can't send the successful data() and finish() callbacks.
    if (!resource->errorOccurred()) {
        docLoader->setLoadInProgress(true);
        resource->data(loader->resourceData(), true);
        resource->finish();
    }
#endif

    delete request;

    docLoader->setLoadInProgress(false);
    
    docLoader->checkForPendingPreloads();

#if REQUEST_DEBUG
    KURL u(ParsedURLString, resource->url());
    printf("HOST %s COUNT %d RECEIVED %s\n", u.host().latin1().data(), m_requestsLoading.size(), resource->url().latin1().data());
#endif
    servePendingRequests();
}

void Loader::Host::didFail(SubresourceLoader* loader, const ResourceError&)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didFail(loader);
}

#ifdef SSLAB_SR
void Loader::Host::clearForReload(Request* request, SubresourceLoader* loader, bool cancelled)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<Host> myProtector(this);

    loader->clearClient();

    DocLoader* docLoader = request->docLoader();
    // Prevent the document from being destroyed before we are done with
    // the docLoader that it will delete when the document gets deleted.
    RefPtr<Document> protector(docLoader->doc());
/*    if (!request->isMultipart())
        docLoader->decrementRequestCount();*/
    CachedResource* resource = request->cachedResource();
    
    if (resource->resourceToRevalidate())
        cache()->revalidationFailed(resource);

    if (!cancelled) {
        docLoader->setLoadInProgress(true);
        resource->error();
    }

/*    docLoader->setLoadInProgress(false);
    if (cancelled || !resource->isPreloaded()) {
        cache()->remove(resource);
		qWarning("remove?");
	}*/

	resource->toNullRequest();	//sslab.sr
//	if (!resource->getRequest()) qWarning("[SR]m_request was that set to NULL");
}
#endif


void Loader::Host::didFail(SubresourceLoader* loader, bool cancelled)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<Host> myProtector(this);

    loader->clearClient();

    RequestMap::iterator i = m_requestsLoading.find(loader);
    if (i == m_requestsLoading.end())
        return;
    
    Request* request = i->second;
    m_requestsLoading.remove(i);
    DocLoader* docLoader = request->docLoader();
    // Prevent the document from being destroyed before we are done with
    // the docLoader that it will delete when the document gets deleted.
    RefPtr<Document> protector(docLoader->doc());
    if (!request->isMultipart())
        docLoader->decrementRequestCount();

    CachedResource* resource = request->cachedResource();
    
    if (resource->resourceToRevalidate())
        cache()->revalidationFailed(resource);

    if (!cancelled) {
        docLoader->setLoadInProgress(true);
        resource->error();
    }
    
    docLoader->setLoadInProgress(false);
    if (cancelled || !resource->isPreloaded())
        cache()->remove(resource);
    
    delete request;

    docLoader->checkForPendingPreloads();

    servePendingRequests();
}

void Loader::Host::didReceiveResponse(SubresourceLoader* loader, const ResourceResponse& response)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<Host> protector(this);

    Request* request = m_requestsLoading.get(loader);
    
    // FIXME: This is a workaround for <rdar://problem/5236843>
    // If a load starts while the frame is still in the provisional state 
    // (this can be the case when loading the user style sheet), committing the load then causes all
    // requests to be removed from the m_requestsLoading map. This means that request might be null here.
    // In that case we just return early. 
    // ASSERT(request);
    if (!request)
        return;
    
    CachedResource* resource = request->cachedResource();
    
    if (resource->isCacheValidator()) {
        if (response.httpStatusCode() == 304) {
            // 304 Not modified / Use local copy
            m_requestsLoading.remove(loader);
            loader->clearClient();
            request->docLoader()->decrementRequestCount();

            // Existing resource is ok, just use it updating the expiration time.
            cache()->revalidationSucceeded(resource, response);
            
            if (request->docLoader()->frame())
                request->docLoader()->frame()->loader()->checkCompleted();

            delete request;

            servePendingRequests();
            return;
        } 
        // Did not get 304 response, continue as a regular resource load.
        cache()->revalidationFailed(resource);
    }

    resource->setResponse(response);

    String encoding = response.textEncodingName();
    if (!encoding.isNull())
        resource->setEncoding(encoding);
    
    if (request->isMultipart()) {
        ASSERT(resource->isImage());
        static_cast<CachedImage*>(resource)->clear();
        if (request->docLoader()->frame())
            request->docLoader()->frame()->loader()->checkCompleted();
    } else if (response.isMultipart()) {
        request->setIsMultipart(true);
        
        // We don't count multiParts in a DocLoader's request count
        request->docLoader()->decrementRequestCount();

        // If we get a multipart response, we must have a handle
        ASSERT(loader->handle());
        if (!resource->isImage())
            loader->handle()->cancel();
    }
}

void Loader::Host::didReceiveData(SubresourceLoader* loader, const char* data, int size)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<Host> protector(this);

    Request* request = m_requestsLoading.get(loader);
    if (!request)
        return;

    CachedResource* resource = request->cachedResource();    
    ASSERT(!resource->isCacheValidator());
    
    if (resource->errorOccurred())
        return;
        
    if (resource->response().httpStatusCode() / 100 == 4) {
        // Treat a 4xx response like a network error for all resources but images (which will ignore the error and continue to load for 
        // legacy compatibility).
        resource->httpStatusCodeError();
        return;
    }

    // Set the data.
    if (request->isMultipart()) {
        // The loader delivers the data in a multipart section all at once, send eof.
        // The resource data will change as the next part is loaded, so we need to make a copy.
        RefPtr<SharedBuffer> copiedData = SharedBuffer::create(data, size);
        resource->data(copiedData.release(), true);
    } else if (request->isIncremental()) 
        resource->data(loader->resourceData(), false);
}

void Loader::Host::cancelPendingRequests(RequestQueue& requestsPending, DocLoader* docLoader)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RequestQueue remaining;
    RequestQueue::iterator end = requestsPending.end();
    for (RequestQueue::iterator it = requestsPending.begin(); it != end; ++it) {
        Request* request = *it;
        if (request->docLoader() == docLoader) {
            cache()->remove(request->cachedResource());
            delete request;
            docLoader->decrementRequestCount();
        } else
            remaining.append(request);
    }
    requestsPending.swap(remaining);
}

void Loader::Host::cancelRequests(DocLoader* docLoader)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    for (unsigned p = 0; p <= High; p++)
        cancelPendingRequests(m_requestsPending[p], docLoader);

    Vector<SubresourceLoader*, 256> loadersToCancel;

    RequestMap::iterator end = m_requestsLoading.end();
    for (RequestMap::iterator i = m_requestsLoading.begin(); i != end; ++i) {
        Request* r = i->second;
        if (r->docLoader() == docLoader)
            loadersToCancel.append(i->first.get());
    }

    for (unsigned i = 0; i < loadersToCancel.size(); ++i) {
        SubresourceLoader* loader = loadersToCancel[i];
        didFail(loader, true);
    }
}

} //namespace WebCore
