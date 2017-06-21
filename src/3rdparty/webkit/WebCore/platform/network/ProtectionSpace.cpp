
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
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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
#include "ProtectionSpace.h"

#if USE(CFNETWORK) && !PLATFORM(MAC)
#include "AuthenticationCF.h"
#include <CFNetwork/CFURLProtectionSpacePriv.h>
#include <wtf/RetainPtr.h>
#endif

namespace WebCore {

// Need to enforce empty, non-null strings due to the pickiness of the String == String operator
// combined with the semantics of the String(NSString*) constructor
ProtectionSpace::ProtectionSpace()
    : m_host("")
    , m_port(0)
    , m_serverType(ProtectionSpaceServerHTTP)
    , m_realm("")
    , m_authenticationScheme(ProtectionSpaceAuthenticationSchemeDefault)
    , m_isHashTableDeletedValue(false)
{
}
 
// Need to enforce empty, non-null strings due to the pickiness of the String == String operator
// combined with the semantics of the String(NSString*) constructor
ProtectionSpace::ProtectionSpace(const String& host, int port, ProtectionSpaceServerType serverType, const String& realm, ProtectionSpaceAuthenticationScheme authenticationScheme)
    : m_host(host.length() ? host : "")
    , m_port(port)
    , m_serverType(serverType)
    , m_realm(realm.length() ? realm : "")
    , m_authenticationScheme(authenticationScheme)
    , m_isHashTableDeletedValue(false)
{    
}
    
const String& ProtectionSpace::host() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    return m_host; 
}

int ProtectionSpace::port() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_port; 
}

ProtectionSpaceServerType ProtectionSpace::serverType() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_serverType; 
}

bool ProtectionSpace::isProxy() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return (m_serverType == ProtectionSpaceProxyHTTP ||
            m_serverType == ProtectionSpaceProxyHTTPS ||
            m_serverType == ProtectionSpaceProxyFTP ||
            m_serverType == ProtectionSpaceProxySOCKS);
}

const String& ProtectionSpace::realm() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    return m_realm; 
}

ProtectionSpaceAuthenticationScheme ProtectionSpace::authenticationScheme() const 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
 
    return m_authenticationScheme; 
}

bool ProtectionSpace::receivesCredentialSecurely() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#if USE(CFNETWORK) && !PLATFORM(MAC)
    RetainPtr<CFURLProtectionSpaceRef> cfSpace(AdoptCF, createCF(*this));
    return cfSpace && CFURLProtectionSpaceReceivesCredentialSecurely(cfSpace.get());
#else
    return (m_serverType == ProtectionSpaceServerHTTPS || 
            m_serverType == ProtectionSpaceServerFTPS || 
            m_serverType == ProtectionSpaceProxyHTTPS || 
            m_authenticationScheme == ProtectionSpaceAuthenticationSchemeHTTPDigest); 
#endif
}

bool operator==(const ProtectionSpace& a, const ProtectionSpace& b)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (a.host() != b.host())
        return false;
    if (a.port() != b.port())
        return false;
    if (a.serverType() != b.serverType())
        return false;
    // Ignore realm for proxies
    if (!a.isProxy() && a.realm() != b.realm())
        return false;
    if (a.authenticationScheme() != b.authenticationScheme())
        return false;
    
    return true;
}

}


