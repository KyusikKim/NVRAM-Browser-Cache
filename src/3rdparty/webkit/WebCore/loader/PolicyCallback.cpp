
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
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "PolicyCallback.h"

#include "FormState.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "HTMLFormElement.h"

namespace WebCore {

PolicyCallback::PolicyCallback()
    : m_navigationFunction(0)
    , m_newWindowFunction(0)
    , m_contentFunction(0)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

PolicyCallback::~PolicyCallback()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void PolicyCallback::clear()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    clearRequest();
    m_navigationFunction = 0;
    m_newWindowFunction = 0;
    m_contentFunction = 0;
}

void PolicyCallback::set(const ResourceRequest& request, PassRefPtr<FormState> formState,
    NavigationPolicyDecisionFunction function, void* argument)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_request = request;
    m_formState = formState;
    m_frameName = String();

    m_navigationFunction = function;
    m_newWindowFunction = 0;
    m_contentFunction = 0;
    m_argument = argument;
}

void PolicyCallback::set(const ResourceRequest& request, PassRefPtr<FormState> formState,
    const String& frameName, NewWindowPolicyDecisionFunction function, void* argument)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_request = request;
    m_formState = formState;
    m_frameName = frameName;

    m_navigationFunction = 0;
    m_newWindowFunction = function;
    m_contentFunction = 0;
    m_argument = argument;
}

void PolicyCallback::set(ContentPolicyDecisionFunction function, void* argument)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_request = ResourceRequest();
    m_formState = 0;
    m_frameName = String();

    m_navigationFunction = 0;
    m_newWindowFunction = 0;
    m_contentFunction = function;
    m_argument = argument;
}

void PolicyCallback::call(bool shouldContinue)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_navigationFunction)
        m_navigationFunction(m_argument, m_request, m_formState.get(), shouldContinue);
    if (m_newWindowFunction)
        m_newWindowFunction(m_argument, m_request, m_formState.get(), m_frameName, shouldContinue);
    ASSERT(!m_contentFunction);
}

void PolicyCallback::call(PolicyAction action)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!m_navigationFunction);
    ASSERT(!m_newWindowFunction);
    ASSERT(m_contentFunction);
    m_contentFunction(m_argument, action);
}

void PolicyCallback::clearRequest()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_request = ResourceRequest();
    m_formState = 0;
    m_frameName = String();
}

void PolicyCallback::cancel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    clearRequest();
    if (m_navigationFunction)
        m_navigationFunction(m_argument, m_request, m_formState.get(), false);
    if (m_newWindowFunction)
        m_newWindowFunction(m_argument, m_request, m_formState.get(), m_frameName, false);
    if (m_contentFunction)
        m_contentFunction(m_argument, PolicyIgnore);
}

} // namespace WebCore
