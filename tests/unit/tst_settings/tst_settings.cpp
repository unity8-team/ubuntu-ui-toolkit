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
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickanchors_p_p.h>

class tst_Settings : public QObject
{
    Q_OBJECT
public:
    tst_Settings() { }

    QQuickView *loadTestCase(const QString &filename)
    {
        QQuickView *view = new QQuickView(0);
        view->setGeometry(0,0, UCUnits::instance().gu(40), UCUnits::instance().gu(30));

        QDir modules("../../../modules");
        Q_ASSERT(modules.exists());
        QString modulePath(modules.absolutePath());
        view->engine()->addImportPath(modulePath);

        view->setSource(QUrl::fromLocalFile(filename));
        Q_ASSERT(view->rootObject());
        view->show();
        QTest::qWaitForWindowExposed(view);
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
        QQuickView *view = loadTestCase("Settings.qml");
        QQuickItem *root = view->rootObject();
        // Run QML test case
        int i(root->metaObject()->indexOfMethod("test_defaults()"));
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);

#if 0
        // This doesn't work with Option, maybe for inheriting UnityAction
        QQuickItem *item = testItem(root, "boolFalse");
        QVERIFY(item);
        QCOMPARE(item->property("value").toBool(), false);
#endif

        // Modify some values in QML
        i = root->metaObject()->indexOfMethod("change_values()");
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);

        // Delete and re-load the view as if it was an app being restarted
        delete view;
        qDebug() << "d v";
        // FIXME: crash here
        view = loadTestCase("Settings.qml");
        root = view->rootObject();
        // Verifiy that the changed values were saved
        i = root->metaObject()->indexOfMethod("verify_values()");
        root->metaObject()->method(i).invoke(root, Qt::DirectConnection);
        qDebug() << "v_v";
    }
};

QTEST_MAIN(tst_Settings)

#include "tst_settings.moc"
