/*
 * Copyright 2012-2013 Canonical Ltd.
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

class tst_Settings : public QObject
{
    Q_OBJECT
public:
    tst_Settings() { }

    QQuickView *loadTestCase(const QString &filename, QSignalSpy **spy = 0)
    {
        QQuickView *view = new QQuickView(0);
        view->setGeometry(0,0, UCUnits::instance().gu(40), UCUnits::instance().gu(30));
        if (spy) {
            *spy = new QSignalSpy(view->engine(), SIGNAL(warnings(QList<QQmlError>)));
            (*spy)->setParent(view);
        }

        QDir modules("../../../modules");
        Q_ASSERT(modules.exists());
        QString modulePath(modules.absolutePath());
        view->engine()->addImportPath(modulePath);

        view->setSource(QUrl::fromLocalFile(filename));
        Q_ASSERT(view->rootObject());
        view->show();
        QTest::qWaitForWindowExposed(view);

        // No warnings from QML
        Q_ASSERT((*spy)->count() == 0);

        return view;
    }

    QQuickItem *testItem(QQuickItem *that, const QString &identifier)
    {
        if (that->property(identifier.toLocal8Bit()).isValid())
            return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

        return that->findChild<QQuickItem*>(identifier);
    }


private Q_SLOTS:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testCase_Settings()
    {
        /* Ensure we start afresh */
        QString xdgDataHome(QProcessEnvironment::systemEnvironment().value("XDG_DATA_HOME",
            QProcessEnvironment::systemEnvironment().value("HOME") + "/.local/share"));
        QDir testDataFolder(xdgDataHome + "/tst_settings");
        testDataFolder.remove("settings.db");

        QQuickView *view = loadTestCase("Settings.qml");
        QQuickItem *root = view->rootObject();

        /* The following doesn't work with Option, so we need to test in QML
        QQuickItem *item = testItem(settings, "boolFalse");
        QVERIFY(item);
        QCOMPARE(item->property("value").toBool(), false); */

        // Run QML test case
        int i(root->metaObject()->indexOfMethod("test_defaults()"));
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);

        // Modify some values in QML
        i = root->metaObject()->indexOfMethod("change_values()");
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);

        // Database written?
        QString databaseFile(testDataFolder.path() + QString("/settings.db"));
        QVERIFY(QFile::exists(databaseFile));

        // Delete and re-load the view as if it was an app being restarted
        delete view;

        QSignalSpy *spy;
        view = loadTestCase("Settings.qml", &spy);
        root = view->rootObject();

        // Verifiy that the changed values were saved
        i = root->metaObject()->indexOfMethod("verify_values()");
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);

        /* No warnings from QML: disabled as long as unrelated bugs aren't fixed:
        qt5/qml/QtTest/TestCase.qml:702: ReferenceError: qtest is not defined
        modules/Ubuntu/Components/MainView.qml:257: TypeError: Cannot call method 'hasOwnProperty' of null
        QCOMPARE(spy->count(), 0); */
    }
};

QTEST_MAIN(tst_Settings)

#include "tst_settings.moc"
