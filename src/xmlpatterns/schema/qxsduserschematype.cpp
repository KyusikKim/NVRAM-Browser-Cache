/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

/*
 * NOTE: This file is included by qxsduserschematype_p.h
 * if you need some includes, put them in qxsduserschematype_p.h (outside of the namespace)
 */

template<typename TSuperClass>
void XsdUserSchemaType<TSuperClass>::setName(const QXmlName &name)
{
    m_name = name;
}

template<typename TSuperClass>
QXmlName XsdUserSchemaType<TSuperClass>::name(const NamePool::Ptr&) const
{
    return m_name;
}

template<typename TSuperClass>
QString XsdUserSchemaType<TSuperClass>::displayName(const NamePool::Ptr &np) const
{
    return np->displayName(m_name);
}

template<typename TSuperClass>
void XsdUserSchemaType<TSuperClass>::setDerivationConstraints(const SchemaType::DerivationConstraints &constraints)
{
    m_derivationConstraints = constraints;
}

template<typename TSuperClass>
SchemaType::DerivationConstraints XsdUserSchemaType<TSuperClass>::derivationConstraints() const
{
    return m_derivationConstraints;
}
