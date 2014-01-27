/*
 * Copyright 2013 Canonical Ltd.
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
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlContext>
#include "uctheme.h"

class tst_UCTheme : public QObject
{
    Q_OBJECT
private:
    QString m_xdgDataPath;
    QString m_modulePath;

    QQuickView *loadTest(const QString &file, QSignalSpy **spy = 0)
    {
        QQuickView *view = new QQuickView;
        if (view) {
            view->engine()->addImportPath(m_modulePath);
            if (spy) {
                (*spy) = new QSignalSpy(view->engine(), SIGNAL(warnings(QList<QQmlError>)));
                (*spy)->setParent(view);
            }
            view->setSource(QUrl::fromLocalFile(file));
            if (!view->rootObject()) {
                view->deleteLater();
                view = 0;
            } else {
                view->show();
                QTest::qWaitForWindowExposed(view);
            }
        }
        return view;
    }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testInstance();
    void testNameDefault();
    void testNameSet();
    void testCreateStyleComponent();
    void testCreateStyleComponent_data();
    void testChangeThemeAfterCompletion();
    void testChangeThemeAfterCompletionNewFunction();
    void testThemesRelativePath();
    void testThemesRelativePathWithParent();
    void testThemesRelativePathWithParentXDGDATA();
    void testThemesRelativePathWithParentNoVariablesSet();
    void testThemesRelativePathWithParentOneXDGPathSet();
};

void tst_UCTheme::initTestCase()
{
    m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
    QString modules("../../../modules");
    QVERIFY(QDir(modules).exists());
    m_modulePath = QDir(modules).absolutePath();
}

void tst_UCTheme::cleanupTestCase()
{
    qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
}

void tst_UCTheme::testInstance()
{
    QVERIFY(UCTheme::instance() == NULL);
}

void tst_UCTheme::testNameDefault()
{
    QScopedPointer<QQuickView> view(loadTest("Defaults.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);
    QCOMPARE(theme->name(), QString("Ubuntu.Components.Themes.Ambiance"));
}

void tst_UCTheme::testNameSet()
{
    QScopedPointer<QQuickView> view(loadTest("NameSet.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);
    QVERIFY(view->rootObject()->setProperty("themeName", "MyBeautifulTheme"));
    QCOMPARE(theme->name(), QString("MyBeautifulTheme"));
}

void tst_UCTheme::testCreateStyleComponent()
{
    QFETCH(QString, styleName);
    QFETCH(QString, parentName);
    QFETCH(bool, success);

    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", ".");

    QScopedPointer<QQuickView> view(loadTest("TestThemeTestStyle.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, parentName);
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), styleName, parent.data());

    QCOMPARE(component != NULL, success);
}

void tst_UCTheme::testCreateStyleComponent_data() {
    QTest::addColumn<QString>("styleName");
    QTest::addColumn<QString>("parentName");
    QTest::addColumn<bool>("success");
    QTest::newRow("Existing style") << "TestStyle.qml" << "Parent.qml" << true;
    QTest::newRow("Non existing style") << "NotExistingTestStyle.qml" << "Parent.qml" << false;
    QTest::newRow("No parent") << "TestStyle.qml" << "" << false;
}

void tst_UCTheme::testChangeThemeAfterCompletion()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    QScopedPointer<QQuickView> view(loadTest("ChangeThemeAfterCompletion.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QVERIFY(!style);
    // change the theme
    QSignalSpy nameChanged(theme, SIGNAL(nameChanged()));
    theme->setName("TestModule.TestTheme");
    QTest::waitForEvents();
    QCOMPARE(nameChanged.count(), 1);

    style = view->rootObject()->property("style").value<QQmlComponent*>();
    QEXPECT_FAIL(0, "Deprecated function used", Continue);
    QVERIFY(style);
}

void tst_UCTheme::testChangeThemeAfterCompletionNewFunction()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    QScopedPointer<QQuickView> view(loadTest("ChangeThemeAfterCompletionNewFunction.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlComponent *style = view->rootObject()->property("style").value<QQmlComponent*>();
    QVERIFY(!style);
    // change the theme
    QSignalSpy nameChanged(theme, SIGNAL(nameChanged()));
    theme->setName("TestModule.TestTheme");
    QTest::waitForEvents();
    QCOMPARE(nameChanged.count(), 1);

    style = view->rootObject()->property("style").value<QQmlComponent*>();
    QVERIFY(style);
}

void tst_UCTheme::testThemesRelativePath()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../tst_theme_engine");

    QScopedPointer<QQuickView> view(loadTest("TestThemeTestStyle.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, "Parent.qml");
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), "TestStyle.qml", parent.data());

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCTheme::testThemesRelativePathWithParent()
{
    QSKIP("https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1248982");
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "../../resources/themes:../../resources/themes/TestModule");

    QScopedPointer<QQuickView> view(loadTest("CustomTheme.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, "Parent.qml");
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), "TestStyle.qml", parent.data());

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCTheme::testThemesRelativePathWithParentXDGDATA()
{
    QSKIP("https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1248982");
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "../../resources/themes:../../resources/themes/TestModule");

    QScopedPointer<QQuickView> view(loadTest("CustomTheme.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, "Parent.qml");
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), "TestStyle.qml", parent.data());

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

void tst_UCTheme::testThemesRelativePathWithParentNoVariablesSet()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "");

    QSignalSpy *spy;
    QScopedPointer<QQuickView> view(loadTest("Defaults.qml", &spy));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, "Parent.qml");
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), "TestStyle.qml", parent.data());

    QCOMPARE(spy->count(), 1);
    QCOMPARE(component == NULL, true);
}

void tst_UCTheme::testThemesRelativePathWithParentOneXDGPathSet()
{
    qputenv("UBUNTU_UI_TOOLKIT_THEMES_PATH", "");
    qputenv("XDG_DATA_DIRS", "../tst_theme_engine");

    QScopedPointer<QQuickView> view(loadTest("TestThemeTestStyle.qml"));
    QVERIFY(view);

    UCTheme *theme = UCTheme::instance();
    QVERIFY(theme);

    QQmlEngine *engine = view->engine();
    QQmlComponent parentComponent(engine, "Parent.qml");
    QScopedPointer<QObject> parent(parentComponent.create());
    QQmlComponent* component = theme->createStyleComponent(theme->name(), "TestStyle.qml", parent.data());

    QCOMPARE(component != NULL, true);
    QCOMPARE(component->status(), QQmlComponent::Ready);
}

QTEST_MAIN(tst_UCTheme)

#include "tst_theme_enginetest.moc"
