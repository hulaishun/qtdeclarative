/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtQuick1/private/qdeclarativeconnections_p.h>
#include <QtQuick1/private/qdeclarativeitem_p.h>
#include <QtDeclarative/qdeclarativescriptstring.h>

class tst_qdeclarativeconnection : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeconnection();

private slots:
    void defaultValues();
    void properties();
    void connection();
    void trimming();
    void targetChanged();
    void unknownSignals_data();
    void unknownSignals();
    void errors_data();
    void errors();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativeconnection::tst_qdeclarativeconnection()
{
}

void tst_qdeclarativeconnection::defaultValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection3.qml"));
    QDeclarative1Connections *item = qobject_cast<QDeclarative1Connections*>(c.create());

    QVERIFY(item != 0);
    QVERIFY(item->target() == 0);

    delete item;
}

void tst_qdeclarativeconnection::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection2.qml"));
    QDeclarative1Connections *item = qobject_cast<QDeclarative1Connections*>(c.create());

    QVERIFY(item != 0);

    QVERIFY(item != 0);
    QVERIFY(item->target() == item);

    delete item;
}

void tst_qdeclarativeconnection::connection()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(c.create());

    QVERIFY(item != 0);

    QCOMPARE(item->property("tested").toBool(), false);
    QCOMPARE(item->width(), 50.);
    emit item->setWidth(100.);
    QCOMPARE(item->width(), 100.);
    QCOMPARE(item->property("tested").toBool(), true);

    delete item;
}

void tst_qdeclarativeconnection::trimming()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/trimming.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(c.create());

    QVERIFY(item != 0);

    QCOMPARE(item->property("tested").toString(), QString(""));
    int index = item->metaObject()->indexOfSignal("testMe(int,QString)");
    QMetaMethod method = item->metaObject()->method(index);
    method.invoke(item,
                  Qt::DirectConnection,
                  Q_ARG(int, 5),
                  Q_ARG(QString, "worked"));
    QCOMPARE(item->property("tested").toString(), QString("worked5"));

    delete item;
}

// Confirm that target can be changed by one of our signal handlers
void tst_qdeclarativeconnection::targetChanged()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/connection-targetchange.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(c.create());
    QVERIFY(item != 0);

    QDeclarative1Connections *connections = item->findChild<QDeclarative1Connections*>("connections");
    QVERIFY(connections);

    QDeclarativeItem *item1 = item->findChild<QDeclarativeItem*>("item1");
    QVERIFY(item1);

    item1->setWidth(200);

    QDeclarativeItem *item2 = item->findChild<QDeclarativeItem*>("item2");
    QVERIFY(item2);
    QVERIFY(connections->target() == item2);

    // If we don't crash then we're OK

    delete item;
}

void tst_qdeclarativeconnection::unknownSignals_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("error");

    QTest::newRow("basic") << "connection-unknownsignals.qml" << ":6:5: QML Connections: Cannot assign to non-existent property \"onFooBar\"";
    QTest::newRow("parent") << "connection-unknownsignals-parent.qml" << ":6:5: QML Connections: Cannot assign to non-existent property \"onFooBar\"";
    QTest::newRow("ignored") << "connection-unknownsignals-ignored.qml" << ""; // should be NO error
    QTest::newRow("notarget") << "connection-unknownsignals-notarget.qml" << ""; // should be NO error
}

void tst_qdeclarativeconnection::unknownSignals()
{
    QFETCH(QString, file);
    QFETCH(QString, error);

    QUrl url = QUrl::fromLocalFile(SRCDIR "/data/" + file);
    if (!error.isEmpty()) {
        QTest::ignoreMessage(QtWarningMsg, (url.toString() + error).toLatin1());
    } else {
        // QTest has no way to insist no message (i.e. fail)
    }

    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, url);
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(c.create());
    QVERIFY(item != 0);

    // check that connection is created (they are all runtime errors)
    QDeclarative1Connections *connections = item->findChild<QDeclarative1Connections*>("connections");
    QVERIFY(connections);

    if (file == "connection-unknownsignals-ignored.qml")
        QVERIFY(connections->ignoreUnknownSignals());

    delete item;
}

void tst_qdeclarativeconnection::errors_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("error");

    QTest::newRow("no \"on\"") << "error-property.qml" << "Cannot assign to non-existent property \"fakeProperty\"";
    QTest::newRow("3rd letter lowercase") << "error-property2.qml" << "Cannot assign to non-existent property \"onfakeProperty\"";
    QTest::newRow("child object") << "error-object.qml" << "Connections: nested objects not allowed";
    QTest::newRow("grouped object") << "error-syntax.qml" << "Connections: syntax error";
}

void tst_qdeclarativeconnection::errors()
{
    QFETCH(QString, file);
    QFETCH(QString, error);

    QUrl url = QUrl::fromLocalFile(SRCDIR "/data/" + file);

    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, url);
    QVERIFY(c.isError() == true);
    QList<QDeclarativeError> errors = c.errors();
    QVERIFY(errors.count() == 1);
    QCOMPARE(errors.at(0).description(), error);
}

QTEST_MAIN(tst_qdeclarativeconnection)

#include "tst_qdeclarativeconnection.moc"
