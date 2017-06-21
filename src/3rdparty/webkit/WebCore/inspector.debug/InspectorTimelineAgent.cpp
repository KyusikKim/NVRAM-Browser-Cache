
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
* Copyright (C) 2009 Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the
* distribution.
*     * Neither the name of Google Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include "InspectorTimelineAgent.h"

#if ENABLE(INSPECTOR)

#include "Event.h"
#include "InspectorFrontend.h"
#include "IntRect.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "TimelineRecordFactory.h"

#include <wtf/CurrentTime.h>

namespace WebCore {

int InspectorTimelineAgent::s_instanceCount = 0;

InspectorTimelineAgent::InspectorTimelineAgent(InspectorFrontend* frontend)
    : m_frontend(frontend)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ++s_instanceCount;
    ASSERT(m_frontend);
}

InspectorTimelineAgent::~InspectorTimelineAgent()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(s_instanceCount);
    --s_instanceCount;
}

void InspectorTimelineAgent::willCallFunction(const String& scriptName, int scriptLine)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createFunctionCallData(m_frontend, scriptName, scriptLine), FunctionCallTimelineRecordType);
}

void InspectorTimelineAgent::didCallFunction()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(FunctionCallTimelineRecordType);
}

void InspectorTimelineAgent::willDispatchEvent(const Event& event)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createEventDispatchData(m_frontend, event),
        EventDispatchTimelineRecordType);
}

void InspectorTimelineAgent::didDispatchEvent()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(EventDispatchTimelineRecordType);
}

void InspectorTimelineAgent::willLayout()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(m_frontend->newScriptObject(), LayoutTimelineRecordType);
}

void InspectorTimelineAgent::didLayout()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(LayoutTimelineRecordType);
}

void InspectorTimelineAgent::willRecalculateStyle()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(m_frontend->newScriptObject(), RecalculateStylesTimelineRecordType);
}

void InspectorTimelineAgent::didRecalculateStyle()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(RecalculateStylesTimelineRecordType);
}

void InspectorTimelineAgent::willPaint(const IntRect& rect)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createPaintData(m_frontend, rect), PaintTimelineRecordType);
}

void InspectorTimelineAgent::didPaint()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(PaintTimelineRecordType);
}

void InspectorTimelineAgent::willWriteHTML(unsigned int length, unsigned int startLine)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createParseHTMLData(m_frontend, length, startLine), ParseHTMLTimelineRecordType);
}

void InspectorTimelineAgent::didWriteHTML(unsigned int endLine)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!m_recordStack.isEmpty()) {
        TimelineRecordEntry entry = m_recordStack.last();
        entry.data.set("endLine", endLine);
        didCompleteCurrentRecord(ParseHTMLTimelineRecordType);
    }
}

void InspectorTimelineAgent::didInstallTimer(int timerId, int timeout, bool singleShot)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    record.set("data", TimelineRecordFactory::createTimerInstallData(m_frontend, timerId, timeout, singleShot));
    addRecordToTimeline(record, TimerInstallTimelineRecordType);
}

void InspectorTimelineAgent::didRemoveTimer(int timerId)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    record.set("data", TimelineRecordFactory::createGenericTimerData(m_frontend, timerId));
    addRecordToTimeline(record, TimerRemoveTimelineRecordType);
}

void InspectorTimelineAgent::willFireTimer(int timerId)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createGenericTimerData(m_frontend, timerId), TimerFireTimelineRecordType); 
}

void InspectorTimelineAgent::didFireTimer()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(TimerFireTimelineRecordType);
}

void InspectorTimelineAgent::willChangeXHRReadyState(const String& url, int readyState)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createXHRReadyStateChangeData(m_frontend, url, readyState), XHRReadyStateChangeRecordType);
}

void InspectorTimelineAgent::didChangeXHRReadyState()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(XHRReadyStateChangeRecordType);
}

void InspectorTimelineAgent::willLoadXHR(const String& url) 
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createXHRLoadData(m_frontend, url), XHRLoadRecordType);
}

void InspectorTimelineAgent::didLoadXHR()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(XHRLoadRecordType);
}

void InspectorTimelineAgent::willEvaluateScript(const String& url, int lineNumber)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createEvaluateScriptData(m_frontend, url, lineNumber), EvaluateScriptTimelineRecordType);
}
    
void InspectorTimelineAgent::didEvaluateScript()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(EvaluateScriptTimelineRecordType);
}

void InspectorTimelineAgent::willSendResourceRequest(unsigned long identifier, bool isMainResource,
    const ResourceRequest& request)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    record.set("data", TimelineRecordFactory::createResourceSendRequestData(m_frontend, identifier, isMainResource, request));
    record.set("type", ResourceSendRequestTimelineRecordType);
    m_frontend->addRecordToTimeline(record);
}

void InspectorTimelineAgent::willReceiveResourceData(unsigned long identifier)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createReceiveResourceData(m_frontend, identifier), ReceiveResourceDataTimelineRecordType);
}

void InspectorTimelineAgent::didReceiveResourceData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(ReceiveResourceDataTimelineRecordType);
}
    
void InspectorTimelineAgent::willReceiveResourceResponse(unsigned long identifier, const ResourceResponse& response)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    pushCurrentRecord(TimelineRecordFactory::createResourceReceiveResponseData(m_frontend, identifier, response), ResourceReceiveResponseTimelineRecordType);
}

void InspectorTimelineAgent::didReceiveResourceResponse()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    didCompleteCurrentRecord(ResourceReceiveResponseTimelineRecordType);
}

void InspectorTimelineAgent::didFinishLoadingResource(unsigned long identifier, bool didFail)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    record.set("data", TimelineRecordFactory::createResourceFinishData(m_frontend, identifier, didFail));
    record.set("type", ResourceFinishTimelineRecordType);
    m_frontend->addRecordToTimeline(record);
}

void InspectorTimelineAgent::didMarkTimeline(const String& message)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    record.set("data", TimelineRecordFactory::createMarkTimelineData(m_frontend, message));
    addRecordToTimeline(record, MarkTimelineRecordType);
}

void InspectorTimelineAgent::reset()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_recordStack.clear();
}

void InspectorTimelineAgent::resetFrontendProxyObject(InspectorFrontend* frontend)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(frontend);
    reset();
    m_frontend = frontend;
}

void InspectorTimelineAgent::addRecordToTimeline(ScriptObject record, TimelineRecordType type)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    record.set("type", type);
    if (m_recordStack.isEmpty())
        m_frontend->addRecordToTimeline(record);
    else {
        TimelineRecordEntry parent = m_recordStack.last();
        parent.children.set(parent.children.length(), record);
    }
}

void InspectorTimelineAgent::didCompleteCurrentRecord(TimelineRecordType type)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    // An empty stack could merely mean that the timeline agent was turned on in the middle of
    // an event.  Don't treat as an error.
    if (!m_recordStack.isEmpty()) {
        TimelineRecordEntry entry = m_recordStack.last();
        m_recordStack.removeLast();
        ASSERT(entry.type == type);
        entry.record.set("data", entry.data);
        entry.record.set("children", entry.children);
        entry.record.set("endTime", currentTimeInMilliseconds());
        addRecordToTimeline(entry.record, type);
    }
}

double InspectorTimelineAgent::currentTimeInMilliseconds()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return currentTime() * 1000.0;
}

void InspectorTimelineAgent::pushCurrentRecord(ScriptObject data, TimelineRecordType type)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ScriptObject record = TimelineRecordFactory::createGenericRecord(m_frontend, currentTimeInMilliseconds());
    m_recordStack.append(TimelineRecordEntry(record, data, m_frontend->newScriptArray(), type));
}
} // namespace WebCore

#endif // ENABLE(INSPECTOR)
