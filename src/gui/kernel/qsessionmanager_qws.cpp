
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
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <qsessionmanager.h>

#ifndef QT_NO_SESSIONMANAGER

QT_BEGIN_NAMESPACE

class QSessionManagerPrivate : public QObjectPrivate
{
public:
    QSessionManagerPrivate(QSessionManager *m, const QString &id,
                           const QString &key);

    QStringList restartCommand;
    QStringList discardCommand;
    const QString sessionId;
    const QString sessionKey;
    QSessionManager::RestartHint restartHint;
};

QSessionManagerPrivate::QSessionManagerPrivate(QSessionManager*,
                                               const QString &id,
                                               const QString &key)
    : QObjectPrivate(), sessionId(id), sessionKey(key)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

QSessionManager::QSessionManager(QApplication *app, QString &id, QString &key)
    : QObject(*new QSessionManagerPrivate(this, id, key), app)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QSessionManager);
    d->restartHint = RestartIfRunning;
}

QSessionManager::~QSessionManager()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

QString QSessionManager::sessionId() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QSessionManager);
    return d->sessionId;
}

QString QSessionManager::sessionKey() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QSessionManager);
    return d->sessionKey;
}


bool QSessionManager::allowsInteraction()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

bool QSessionManager::allowsErrorInteraction()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

void QSessionManager::release()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void QSessionManager::cancel()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void QSessionManager::setRestartHint(QSessionManager::RestartHint hint)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QSessionManager);
    d->restartHint = hint;
}

QSessionManager::RestartHint QSessionManager::restartHint() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QSessionManager);
    return d->restartHint;
}

void QSessionManager::setRestartCommand(const QStringList &command)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QSessionManager);
    d->restartCommand = command;
}

QStringList QSessionManager::restartCommand() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QSessionManager);
    return d->restartCommand;
}

void QSessionManager::setDiscardCommand(const QStringList &command)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(QSessionManager);
    d->discardCommand = command;
}

QStringList QSessionManager::discardCommand() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_D(const QSessionManager);
    return d->discardCommand;
}

void QSessionManager::setManagerProperty(const QString &name,
                                         const QString &value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_UNUSED(name);
    Q_UNUSED(value);
}

void QSessionManager::setManagerProperty(const QString &name,
                                         const QStringList &value)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    Q_UNUSED(name);
    Q_UNUSED(value);
}

bool QSessionManager::isPhase2() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

void QSessionManager::requestPhase2()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][tid %lu] %s:%d:%s", QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

QT_END_NAMESPACE

#endif // QT_NO_SESSIONMANAGER
