
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
 * Copyright (C) 2009 Google Inc.  All rights reserved.
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
#include "SocketStreamHandleBase.h"

#include "SocketStreamHandle.h"
#include "SocketStreamHandleClient.h"

namespace WebCore {

const unsigned int bufferSize = 100 * 1024 * 1024;

SocketStreamHandleBase::SocketStreamHandleBase(const KURL& url, SocketStreamHandleClient* client)
    : m_url(url)
    , m_client(client)
    , m_state(Connecting)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

SocketStreamHandleBase::SocketStreamState SocketStreamHandleBase::state() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_state;
}

bool SocketStreamHandleBase::send(const char* data, int length)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_state == Connecting)
        return false;
    if (!m_buffer.isEmpty()) {
        if (m_buffer.size() + length > bufferSize) {
            // FIXME: report error to indicate that buffer has no more space.
            return false;
        }
        m_buffer.append(data, length);
        return true;
    }
    int bytesWritten = 0;
    if (m_state == Open)
        bytesWritten = platformSend(data, length);
    if (bytesWritten < 0)
        return false;
    if (m_buffer.size() + length - bytesWritten > bufferSize) {
        // FIXME: report error to indicate that buffer has no more space.
        return false;
    }
    if (bytesWritten < length)
        m_buffer.append(data + bytesWritten, length - bytesWritten);
    return true;
}

void SocketStreamHandleBase::close()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    RefPtr<SocketStreamHandle> protect(static_cast<SocketStreamHandle*>(this)); // platformClose calls the client, which may make the handle get deallocated immediately.

    platformClose();
    m_state = Closed;
}

void SocketStreamHandleBase::setClient(SocketStreamHandleClient* client)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    ASSERT(!client || (!m_client && m_state == Connecting));
    m_client = client;
}

bool SocketStreamHandleBase::sendPendingData()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (m_state != Open)
        return false;
    if (m_buffer.isEmpty())
        return false;
    int bytesWritten = platformSend(m_buffer.data(), m_buffer.size());
    if (bytesWritten <= 0)
        return false;
    Vector<char> remainingData;
    ASSERT(m_buffer.size() - bytesWritten <= bufferSize);
    remainingData.append(m_buffer.data() + bytesWritten, m_buffer.size() - bytesWritten);
    m_buffer.swap(remainingData);
    return true;
}

}  // namespace WebCore
