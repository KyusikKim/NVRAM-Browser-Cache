
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

#include "qnetworkaccessdatabackend_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qurlinfo.h"
#include "private/qdataurl_p.h"
#include <qcoreapplication.h>

QT_BEGIN_NAMESPACE

QNetworkAccessBackend *
QNetworkAccessDataBackendFactory::create(QNetworkAccessManager::Operation,
                                         const QNetworkRequest &request) const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (request.url().scheme() == QLatin1String("data"))
        return new QNetworkAccessDataBackend;

    return 0;
}

QNetworkAccessDataBackend::QNetworkAccessDataBackend()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

QNetworkAccessDataBackend::~QNetworkAccessDataBackend()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void QNetworkAccessDataBackend::open()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QUrl uri = request().url();

    if (operation() != QNetworkAccessManager::GetOperation &&
        operation() != QNetworkAccessManager::HeadOperation) {
        // data: doesn't support anything but GET
        const QString msg = QCoreApplication::translate("QNetworkAccessDataBackend",
                                                        "Operation not supported on %1")
                      .arg(uri.toString());
        error(QNetworkReply::ContentOperationNotPermittedError, msg);
        finished();
        return;
    }

    QPair<QString, QByteArray> decoded = qDecodeDataUrl(uri);

    if (! decoded.first.isNull()) {
        setHeader(QNetworkRequest::ContentTypeHeader, decoded.first);
        setHeader(QNetworkRequest::ContentLengthHeader, decoded.second.size());
        emit metaDataChanged();

        QByteDataBuffer list;
        list.append(decoded.second);
        decoded.second.clear(); // important because of implicit sharing!
        writeDownstreamData(list);

        finished();
        return;
    }

    // something wrong with this URI
    const QString msg = QCoreApplication::translate("QNetworkAccessDataBackend",
                                                    "Invalid URI: %1").arg(uri.toString());
    error(QNetworkReply::ProtocolFailure, msg);
    finished();
}

void QNetworkAccessDataBackend::closeDownstreamChannel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void QNetworkAccessDataBackend::closeUpstreamChannel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

bool QNetworkAccessDataBackend::waitForDownstreamReadyRead(int)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

bool QNetworkAccessDataBackend::waitForUpstreamBytesWritten(int)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

bool QNetworkAccessDataBackend::processRequestSynchronously()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

#ifndef QT_NO_BEARERMANAGEMENT
    start();
#else
    open();
#endif
    return true;
}

QT_END_NAMESPACE
