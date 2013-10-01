/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
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
#ifndef QV4INTERNALCLASS_H
#define QV4INTERNALCLASS_H

#include <QHash>
#include <QVector>
#include "qv4global_p.h"

QT_BEGIN_NAMESPACE

namespace QV4 {

struct String;
struct ExecutionEngine;
struct Object;
struct Identifier;

struct PropertyHashData;
struct PropertyHash
{
    struct Entry {
        const Identifier *identifier;
        uint index;
    };

    PropertyHashData *d;

    inline PropertyHash();
    inline PropertyHash(const PropertyHash &other);
    inline ~PropertyHash();

    void addEntry(const Entry &entry, int classSize);
    uint lookup(const Identifier *identifier) const;

private:
    PropertyHash &operator=(const PropertyHash &other);
};

struct PropertyHashData
{
    PropertyHashData(int numBits);
    ~PropertyHashData() {
        free(entries);
    }

    QBasicAtomicInt refCount;
    int alloc;
    int size;
    int numBits;
    PropertyHash::Entry *entries;
};

inline PropertyHash::PropertyHash()
{
    d = new PropertyHashData(3);
}

inline PropertyHash::PropertyHash(const PropertyHash &other)
{
    d = other.d;
    d->refCount.ref();
}

inline PropertyHash::~PropertyHash()
{
    if (!d->refCount.deref())
        delete d;
}

struct InternalClassTransition
{
    union {
        Identifier *id;
        Object *prototype;
    };
    int flags;
    enum { ProtoChange = 0x100 };

    bool operator==(const InternalClassTransition &other) const
    { return id == other.id && flags == other.flags; }
};
uint qHash(const QV4::InternalClassTransition &t, uint = 0);

struct InternalClass {
    ExecutionEngine *engine;
    Object *prototype;
    PropertyHash propertyTable; // id to valueIndex
    QVector<String *> nameMap;

    QVector<PropertyAttributes> propertyData;

    typedef InternalClassTransition Transition;
    QHash<Transition, InternalClass *> transitions; // id to next class, positive means add, negative delete

    InternalClass *m_sealed;
    InternalClass *m_frozen;

    uint size;

    InternalClass *changePrototype(Object *proto);
    InternalClass *addMember(StringRef string, PropertyAttributes data, uint *index = 0);
    InternalClass *addMember(String *string, PropertyAttributes data, uint *index = 0);
    InternalClass *changeMember(String *string, PropertyAttributes data, uint *index = 0);
    void removeMember(Object *object, Identifier *id);
    uint find(const StringRef string);
    uint find(const String *s);

    InternalClass *sealed();
    InternalClass *frozen();

    void destroy();
    void markObjects();

private:
    friend struct ExecutionEngine;
    InternalClass(ExecutionEngine *engine) : engine(engine), prototype(0), m_sealed(0), m_frozen(0), size(0) {}
    InternalClass(const InternalClass &other);
};

}

QT_END_NAMESPACE

#endif
