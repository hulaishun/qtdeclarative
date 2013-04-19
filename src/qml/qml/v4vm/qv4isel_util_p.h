/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the V4VM module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QV4ISEL_UTIL_P_H
#define QV4ISEL_UTIL_P_H

#include "qv4runtime_p.h"
#include "qv4jsir_p.h"

namespace QQmlJS {

inline QV4::Value convertToValue(V4IR::Const *c)
{
    switch (c->type) {
    case V4IR::MissingType:
        return QV4::Value::deletedValue();
    case V4IR::NullType:
        return QV4::Value::nullValue();
    case V4IR::UndefinedType:
        return QV4::Value::undefinedValue();
    case V4IR::BoolType:
        return QV4::Value::fromBoolean(c->value != 0);
    case V4IR::NumberType: {
        int ival = (int)c->value;
        // +0 != -0, so we need to convert to double when negating 0
        if (ival == c->value && !(c->value == 0 && isNegative(c->value))) {
            return QV4::Value::fromInt32(ival);
        } else {
            return QV4::Value::fromDouble(c->value);
        }
    }
    default:
        Q_UNREACHABLE();
    }
}

} // namespace QQmlJS

#endif // QV4ISEL_UTIL_P_H
