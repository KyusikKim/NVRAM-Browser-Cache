
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
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "modelmenu.h"

#include <QtCore/QAbstractItemModel>
#include <qdebug.h>

ModelMenu::ModelMenu(QWidget * parent)
    : QMenu(parent)
    , m_maxRows(7)
    , m_firstSeparator(-1)
    , m_maxWidth(-1)
    , m_hoverRole(0)
    , m_separatorRole(0)
    , m_model(0)
{
    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
}

bool ModelMenu::prePopulated()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return false;
}

void ModelMenu::postPopulated()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

}

void ModelMenu::setModel(QAbstractItemModel *model)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_model = model;
}

QAbstractItemModel *ModelMenu::model() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_model;
}

void ModelMenu::setMaxRows(int max)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_maxRows = max;
}

int ModelMenu::maxRows() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_maxRows;
}

void ModelMenu::setFirstSeparator(int offset)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_firstSeparator = offset;
}

int ModelMenu::firstSeparator() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_firstSeparator;
}

void ModelMenu::setRootIndex(const QModelIndex &index)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_root = index;
}

QModelIndex ModelMenu::rootIndex() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_root;
}

void ModelMenu::setHoverRole(int role)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_hoverRole = role;
}

int ModelMenu::hoverRole() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_hoverRole;
}

void ModelMenu::setSeparatorRole(int role)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    m_separatorRole = role;
}

int ModelMenu::separatorRole() const
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    return m_separatorRole;
}

Q_DECLARE_METATYPE(QModelIndex)
void ModelMenu::aboutToShow()
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (QMenu *menu = qobject_cast<QMenu*>(sender())) {
        QVariant v = menu->menuAction()->data();
        if (v.canConvert<QModelIndex>()) {
            QModelIndex idx = qvariant_cast<QModelIndex>(v);
            createMenu(idx, -1, menu, menu);
            disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
            return;
        }
    }

    clear();
    if (prePopulated())
        addSeparator();
    int max = m_maxRows;
    if (max != -1)
        max += m_firstSeparator;
    createMenu(m_root, max, this, this);
    postPopulated();
}

void ModelMenu::createMenu(const QModelIndex &parent, int max, QMenu *parentMenu, QMenu *menu)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    if (!menu) {
        QString title = parent.data().toString();
        menu = new QMenu(title, this);
        QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));
        menu->setIcon(icon);
        parentMenu->addMenu(menu);
        QVariant v;
        v.setValue(parent);
        menu->menuAction()->setData(v);
        connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
        return;
    }

    int end = m_model->rowCount(parent);
    if (max != -1)
        end = qMin(max, end);

    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
    connect(menu, SIGNAL(hovered(QAction*)), this, SLOT(hovered(QAction*)));

    for (int i = 0; i < end; ++i) {
        QModelIndex idx = m_model->index(i, 0, parent);
        if (m_model->hasChildren(idx)) {
            createMenu(idx, -1, menu);
        } else {
            if (m_separatorRole != 0
                && idx.data(m_separatorRole).toBool())
                addSeparator();
            else
                menu->addAction(makeAction(idx));
        }
        if (menu == this && i == m_firstSeparator - 1)
            addSeparator();
    }
}

QAction *ModelMenu::makeAction(const QModelIndex &index)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QAction *action = makeAction(icon, index.data().toString(), this);
    QVariant v;
    v.setValue(index);
    action->setData(v);
    return action;
}

QAction *ModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QFontMetrics fm(font());
    if (-1 == m_maxWidth)
        m_maxWidth = fm.width(QLatin1Char('m')) * 30;
    QString smallText = fm.elidedText(text, Qt::ElideMiddle, m_maxWidth);
    return new QAction(icon, smallText, parent);
}

void ModelMenu::triggered(QAction *action)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        emit activated(idx);
    }
}

void ModelMenu::hovered(QAction *action)
{
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif

    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        QString hoveredString = idx.data(m_hoverRole).toString();
        if (!hoveredString.isEmpty())
            emit hovered(hoveredString);
    }
}

