
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
#include "History.h"

#include "ExceptionCode.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "HistoryItem.h"
#include "Page.h"

namespace WebCore {

History::History(Frame* frame)
    : m_frame(frame)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

Frame* History::frame() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_frame;
}

void History::disconnectFrame()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_frame = 0;
}

unsigned History::length() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame)
        return 0;
    if (!m_frame->page())
        return 0;
    return m_frame->page()->getHistoryLength();
}

void History::back()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame)
        return;
    m_frame->redirectScheduler()->scheduleHistoryNavigation(-1);
}

void History::forward()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame)
        return;
    m_frame->redirectScheduler()->scheduleHistoryNavigation(1);
}

void History::go(int distance)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame)
        return;
    m_frame->redirectScheduler()->scheduleHistoryNavigation(distance);
}

KURL History::urlForState(const String& urlString)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    KURL baseURL = m_frame->loader()->baseURL();
    if (urlString.isEmpty())
        return baseURL;
        
    KURL absoluteURL(baseURL, urlString);
    if (!absoluteURL.isValid())
        return KURL();
    
    if (absoluteURL.string().left(absoluteURL.pathStart()) != baseURL.string().left(baseURL.pathStart()))
        return KURL();
    
    return absoluteURL;
}

void History::stateObjectAdded(PassRefPtr<SerializedScriptValue> data, const String& title, const String& urlString, StateObjectType stateObjectType, ExceptionCode& ec)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_frame || !m_frame->page())
        return;
    
    KURL fullURL = urlForState(urlString);
    if (!fullURL.isValid()) {
        ec = SECURITY_ERR;
        return;
    }

    if (stateObjectType == StateObjectPush)
        m_frame->loader()->history()->pushState(data, title, fullURL.string());
    else if (stateObjectType == StateObjectReplace)
        m_frame->loader()->history()->replaceState(data, title, fullURL.string());
            
    if (!urlString.isEmpty())
        m_frame->document()->updateURLForPushOrReplaceState(fullURL);

    if (stateObjectType == StateObjectPush)
        m_frame->loader()->client()->dispatchDidPushStateWithinPage();
    else if (stateObjectType == StateObjectReplace)
        m_frame->loader()->client()->dispatchDidReplaceStateWithinPage();
}

} // namespace WebCore