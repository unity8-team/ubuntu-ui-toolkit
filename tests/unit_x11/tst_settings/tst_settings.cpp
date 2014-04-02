/*
 * Copyright 2012-2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "ucunits.h"
#include "uctestcase.h"

class tst_Settings : public QObject
{
    Q_OBJECT
public:
    tst_Settings() { }

private Q_SLOTS:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testCase_Settings_data()
    {
        QTest::addColumn<QString>("flags");
        QTest::addColumn<bool>("boolFalse");
        QTest::addColumn<bool>("boolTrue");
        QTest::addColumn<QString>("stringEmpty");
        QTest::addColumn<QString>("stringUrl");
        QTest::addColumn<int>("intMax");
        QTest::addColumn<int>("intZero");
        QTest::newRow("Defaults") << "reset"
            << false << true << "" << "http://www.ubuntu.com" << 0 << 65535;
        QTest::newRow("Double-check") << ""
            << false << true << "" << "http://www.ubuntu.com" << 0 << 65535;
        QTest::newRow("Changes") << "change"
            << true << false << "worldOfPain" << "http://example.com" << 333 << 666;
        QTest::newRow("Verify") << ""
            << true << false << "worldOfPain" << "http://example.com" << 333 << 666;
        QTest::newRow("Components") << "components"
            << true << false << "worldOfPain" << "http://example.com" << 333 << 666;
    }

    void testCase_Settings()
    {
        QString xdgDataHome(QProcessEnvironment::systemEnvironment().value("XDG_DATA_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.local/share"));
        QDir testDataFolder(xdgDataHome + "/tst_settings");
        QString databaseFilename("settings.db");

        QFETCH(QString, flags);
        if (flags.contains("reset"))
            QVERIFY(testDataFolder.remove(databaseFilename));

        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("Settings.qml"));
        QObject *settings = testCase->findItem<QObject*>("settings");
        QVERIFY(settings);

        QTest::qWait(1000);

        if (flags.contains("change")) {
            QFETCH(bool, boolFalse);
            QFETCH(bool, boolTrue);
            QFETCH(QString, stringEmpty);
            QFETCH(QString, stringUrl);
            QFETCH(int, intMax);
            QFETCH(int, intZero);
            testCase->findItem<QObject*>("boolFalse")->setProperty("value", boolFalse);
            testCase->findItem<QObject*>("boolTrue")->setProperty("value", boolTrue);
            testCase->findItem<QObject*>("stringEmpty")->setProperty("value", stringEmpty);
            testCase->findItem<QObject*>("stringUrl")->setProperty("value", stringUrl);
            testCase->findItem<QObject*>("intMax")->setProperty("value", intMax);
            testCase->findItem<QObject*>("intZero")->setProperty("value", intZero);
        } else if (flags.contains("components")) {
            QTEST(testCase->findItem<QObject*>("boolFalseComponent")->property("checked").toBool(), "boolFalse");
            QTEST(testCase->findItem<QObject*>("boolTrueComponent")->property("checked").toBool(), "boolTrue");
            QTEST(testCase->findItem<QObject*>("stringEmptyComponent")->property("text").toString(), "stringEmpty");
            QTEST(testCase->findItem<QObject*>("stringUrlComponent")->property("text").toString(), "stringUrl");
            QTEST(testCase->findItem<QObject*>("intMaxComponent")->property("text").toInt(), "intMax");
            QTEST(testCase->findItem<QObject*>("intZeroComponent")->property("text").toInt(), "intZero");
        } else {
            QTEST(testCase->findItem<QObject*>("boolFalse")->property("value").toBool(), "boolFalse");
            QTEST(testCase->findItem<QObject*>("boolTrue")->property("value").toBool(), "boolTrue");
            QTEST(testCase->findItem<QObject*>("stringEmpty")->property("value").toString(), "stringEmpty");
            QTEST(testCase->findItem<QObject*>("stringUrl")->property("value").toString(), "stringUrl");
            QTEST(testCase->findItem<QObject*>("intMax")->property("value").toInt(), "intMax");
            QTEST(testCase->findItem<QObject*>("intZero")->property("value").toInt(), "intZero");
        }

        // Database written?
        QString databaseFile(testDataFolder.path() + QString("/") + databaseFilename);
        QVERIFY(QFile::exists(databaseFile));

        QTest::qWait(1000);
    }
};

QTEST_MAIN(tst_Settings)

#include "tst_settings.moc"
