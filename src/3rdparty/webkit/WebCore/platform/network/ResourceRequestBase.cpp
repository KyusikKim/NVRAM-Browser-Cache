
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
 * Copyright (C) 2003, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "ResourceRequestBase.h"
#include "ResourceRequest.h"

using namespace std;

namespace WebCore {

inline const ResourceRequest& ResourceRequestBase::asResourceRequest() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return *static_cast<const ResourceRequest*>(this);
}

PassOwnPtr<ResourceRequest> ResourceRequestBase::adopt(PassOwnPtr<CrossThreadResourceRequestData> data)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    OwnPtr<ResourceRequest> request(new ResourceRequest());
    request->setURL(data->m_url);
    request->setCachePolicy(data->m_cachePolicy);
    request->setTimeoutInterval(data->m_timeoutInterval);
    request->setFirstPartyForCookies(data->m_firstPartyForCookies);
    request->setHTTPMethod(data->m_httpMethod);

    request->updateResourceRequest();
    request->m_httpHeaderFields.adopt(data->m_httpHeaders.release());

    size_t encodingCount = data->m_responseContentDispositionEncodingFallbackArray.size();
    if (encodingCount > 0) {
        String encoding1 = data->m_responseContentDispositionEncodingFallbackArray[0];
        String encoding2;
        String encoding3;
        if (encodingCount > 1) {
            encoding2 = data->m_responseContentDispositionEncodingFallbackArray[1];
            if (encodingCount > 2)
                encoding3 = data->m_responseContentDispositionEncodingFallbackArray[2];
        }
        ASSERT(encodingCount <= 3);
        request->setResponseContentDispositionEncodingFallbackArray(encoding1, encoding2, encoding3);
    }
    request->setHTTPBody(data->m_httpBody);
    request->setAllowCookies(data->m_allowCookies);
    return request.release();
}

PassOwnPtr<CrossThreadResourceRequestData> ResourceRequestBase::copyData() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    OwnPtr<CrossThreadResourceRequestData> data(new CrossThreadResourceRequestData());
    data->m_url = url().copy();
    data->m_cachePolicy = cachePolicy();
    data->m_timeoutInterval = timeoutInterval();
    data->m_firstPartyForCookies = firstPartyForCookies().copy();
    data->m_httpMethod = httpMethod().crossThreadString();
    data->m_httpHeaders = httpHeaderFields().copyData();

    data->m_responseContentDispositionEncodingFallbackArray.reserveInitialCapacity(m_responseContentDispositionEncodingFallbackArray.size());
    size_t encodingArraySize = m_responseContentDispositionEncodingFallbackArray.size();
    for (size_t index = 0; index < encodingArraySize; ++index) {
        data->m_responseContentDispositionEncodingFallbackArray.append(m_responseContentDispositionEncodingFallbackArray[index].crossThreadString());
    }
    if (m_httpBody)
        data->m_httpBody = m_httpBody->deepCopy();
    data->m_allowCookies = m_allowCookies;
    return data.release();
}

bool ResourceRequestBase::isEmpty() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_url.isEmpty(); 
}

bool ResourceRequestBase::isNull() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_url.isNull();
}

const KURL& ResourceRequestBase::url() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_url;
}

void ResourceRequestBase::setURL(const KURL& url)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    updateResourceRequest(); 

    m_url = url; 
    
    m_platformRequestUpdated = false;
}

void ResourceRequestBase::removeCredentials()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 

    m_url.setUser(String());
    m_url.setPass(String());

    m_platformRequestUpdated = false;
}

ResourceRequestCachePolicy ResourceRequestBase::cachePolicy() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_cachePolicy; 
}

void ResourceRequestBase::setCachePolicy(ResourceRequestCachePolicy cachePolicy)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_cachePolicy = cachePolicy;
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

double ResourceRequestBase::timeoutInterval() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_timeoutInterval; 
}

void ResourceRequestBase::setTimeoutInterval(double timeoutInterval) 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_timeoutInterval = timeoutInterval; 
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

const KURL& ResourceRequestBase::firstPartyForCookies() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_firstPartyForCookies;
}

void ResourceRequestBase::setFirstPartyForCookies(const KURL& firstPartyForCookies)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    updateResourceRequest(); 
    
    m_firstPartyForCookies = firstPartyForCookies;
    
    m_platformRequestUpdated = false;
}

const String& ResourceRequestBase::httpMethod() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_httpMethod; 
}

void ResourceRequestBase::setHTTPMethod(const String& httpMethod) 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 

    m_httpMethod = httpMethod;
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

const HTTPHeaderMap& ResourceRequestBase::httpHeaderFields() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 

    return m_httpHeaderFields; 
}

String ResourceRequestBase::httpHeaderField(const AtomicString& name) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_httpHeaderFields.get(name);
}

String ResourceRequestBase::httpHeaderField(const char* name) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_httpHeaderFields.get(name);
}

void ResourceRequestBase::setHTTPHeaderField(const AtomicString& name, const String& value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_httpHeaderFields.set(name, value); 
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

void ResourceRequestBase::setHTTPHeaderField(const char* name, const String& value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    setHTTPHeaderField(AtomicString(name), value);
}

void ResourceRequestBase::clearHTTPReferrer()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 

    m_httpHeaderFields.remove("Referer");

    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

void ResourceRequestBase::clearHTTPOrigin()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 

    m_httpHeaderFields.remove("Origin");

    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

void ResourceRequestBase::setResponseContentDispositionEncodingFallbackArray(const String& encoding1, const String& encoding2, const String& encoding3)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_responseContentDispositionEncodingFallbackArray.clear();
    if (!encoding1.isNull())
        m_responseContentDispositionEncodingFallbackArray.append(encoding1);
    if (!encoding2.isNull())
        m_responseContentDispositionEncodingFallbackArray.append(encoding2);
    if (!encoding3.isNull())
        m_responseContentDispositionEncodingFallbackArray.append(encoding3);
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

FormData* ResourceRequestBase::httpBody() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    updateResourceRequest(); 
    
    return m_httpBody.get(); 
}

void ResourceRequestBase::setHTTPBody(PassRefPtr<FormData> httpBody)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_httpBody = httpBody; 
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
} 

bool ResourceRequestBase::allowCookies() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    return m_allowCookies;
}

void ResourceRequestBase::setAllowCookies(bool allowCookies)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest(); 
    
    m_allowCookies = allowCookies;
    
    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

void ResourceRequestBase::addHTTPHeaderField(const AtomicString& name, const String& value) 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    updateResourceRequest();
    pair<HTTPHeaderMap::iterator, bool> result = m_httpHeaderFields.add(name, value); 
    if (!result.second)
        result.first->second += "," + value;

    if (url().protocolInHTTPFamily())
        m_platformRequestUpdated = false;
}

void ResourceRequestBase::addHTTPHeaderFields(const HTTPHeaderMap& headerFields)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    HTTPHeaderMap::const_iterator end = headerFields.end();
    for (HTTPHeaderMap::const_iterator it = headerFields.begin(); it != end; ++it)
        addHTTPHeaderField(it->first, it->second);
}

bool equalIgnoringHeaderFields(const ResourceRequestBase& a, const ResourceRequestBase& b)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (a.url() != b.url())
        return false;
    
    if (a.cachePolicy() != b.cachePolicy())
        return false;
    
    if (a.timeoutInterval() != b.timeoutInterval())
        return false;
    
    if (a.firstPartyForCookies() != b.firstPartyForCookies())
        return false;
    
    if (a.httpMethod() != b.httpMethod())
        return false;
    
    if (a.allowCookies() != b.allowCookies())
        return false;
    
    FormData* formDataA = a.httpBody();
    FormData* formDataB = b.httpBody();
    
    if (!formDataA)
        return !formDataB;
    if (!formDataB)
        return !formDataA;
    
    if (*formDataA != *formDataB)
        return false;
    
    return true;
}

bool operator==(const ResourceRequestBase& a, const ResourceRequestBase& b)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!equalIgnoringHeaderFields(a, b))
        return false;
    
    if (a.httpHeaderFields() != b.httpHeaderFields())
        return false;
        
    return true;
}

bool ResourceRequestBase::isConditional() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return (m_httpHeaderFields.contains("If-Match") ||
            m_httpHeaderFields.contains("If-Modified-Since") ||
            m_httpHeaderFields.contains("If-None-Match") ||
            m_httpHeaderFields.contains("If-Range") ||
            m_httpHeaderFields.contains("If-Unmodified-Since"));
}

void ResourceRequestBase::updatePlatformRequest() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_platformRequestUpdated)
        return;
    
    const_cast<ResourceRequest&>(asResourceRequest()).doUpdatePlatformRequest();
    m_platformRequestUpdated = true;
}

void ResourceRequestBase::updateResourceRequest() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_resourceRequestUpdated)
        return;

    const_cast<ResourceRequest&>(asResourceRequest()).doUpdateResourceRequest();
    m_resourceRequestUpdated = true;
}

#if !PLATFORM(MAC) && !USE(CFNETWORK) && !USE(SOUP) && !PLATFORM(CHROMIUM) && !PLATFORM(ANDROID) && !PLATFORM(QT)
unsigned initializeMaximumHTTPConnectionCountPerHost()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // This is used by the loader to control the number of issued parallel load requests. 
    // Four seems to be a common default in HTTP frameworks.
    return 4;
}
#endif

}
