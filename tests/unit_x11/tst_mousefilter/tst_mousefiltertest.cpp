/*
 * Copyright 2012 Canonical Ltd.
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

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QEvent>

#include "ucmousefilter.h"
#include "inversemouseareatype.h"
#include <private/qquickevents_p_p.h>
#include <private/qquickmousearea_p.h>

class tst_mouseFilterTest : public QObject
{
    Q_OBJECT
    
public:
    tst_mouseFilterTest() {}

private:
    QString m_modulePath;
    bool insideClick;

    QQuickView * loadTest(const QString &file)
    {
        QQuickView *view = new QQuickView;
        view->engine()->addImportPath(m_modulePath);

        view->setSource(QUrl::fromLocalFile(file));
        if (!view->rootObject()) {
            delete view;
            view = 0;
        } else {
            view->show();
            QTest::qWaitForWindowExposed(view);
        }
        return view;
    }

    void pressAndHold(QWindow *view, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QPoint &point)
    {
        QTest::mousePress(view, button, modifiers, point);
        QTest::qWait(900);
    }

protected Q_SLOTS:
    void filterClick(UCMouseEvent *event)
    {
        insideClick = event->inside();
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        m_modulePath = QDir(modules).absolutePath();
    }

    void cleanupTestCase()
    {
    }

    void testCase_Defaults()
    {
        QQuickView *view = loadTest("Defaults.qml");
        QVERIFY(view);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject());
        QVERIFY(filter);
        QCOMPARE(filter->acceptedButtons(), Qt::LeftButton);
        QCOMPARE(filter->pressAndHoldDelay(), 800);
        delete view;
    }

    void testCase_FilterPressed()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        maSpy.clear(); filterSpy.clear();
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

    void testCase_FilterReleased()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(released(UCMouseEvent*)));

        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        maSpy.clear(); filterSpy.clear();
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

    void testCase_FilterClickedInside()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));
        QObject::connect(filter, SIGNAL(clicked(UCMouseEvent*)), this, SLOT(filterClick(UCMouseEvent*)));

        insideClick = false;
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);
        QCOMPARE(insideClick, true);

        delete view;
    }

    void testCase_FilterClickedOutside()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));
        QObject::connect(filter, SIGNAL(clicked(UCMouseEvent*)), this, SLOT(filterClick(UCMouseEvent*)));

        insideClick = false;
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);
        QCOMPARE(insideClick, false);

        delete view;
    }

    void testCase_FilterPressAndHold()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

        // outside
        pressAndHold(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);
        // discard press event
        QTest::mouseRelease(view, Qt::LeftButton, 0, QPoint(20, 20));

        imaSpy.clear(); filterSpy.clear();
        // inside
        pressAndHold(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);
        // discard press event
        QTest::mouseRelease(view, Qt::LeftButton, 0, QPoint(20, 20));

        delete view;
    }

    void testCase_FilterDoubleClick()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(doubleClicked(UCMouseEvent*)));

        // outside
        QTest::mouseDClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);

        // inside
        imaSpy.clear(); filterSpy.clear();
        QTest::mouseDClick(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

    void testCase_FilterEntered()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(entered()));
        QSignalSpy imaSpy(ima, SIGNAL(entered()));
        QSignalSpy filterSpy(filter, SIGNAL(entered()));

        // outside
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 0);

        imaSpy.clear();
        // inside
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

    void testCase_FilterExited()
    {
        QQuickView *view = loadTest("Filter.qml");
        QVERIFY(view);

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(exited()));
        QSignalSpy imaSpy(ima, SIGNAL(exited()));
        QSignalSpy filterSpy(filter, SIGNAL(exited()));

        // outside
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 0);

        imaSpy.clear();
        // inside
        QTest::mouseClick(view, Qt::LeftButton, 0, QPoint(50, 50));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

    void testCase_MouseFilterInWindow()
    {
        QQuickView *view = loadTest("FilterInWindow.qml");
        QVERIFY(view);
        QList<QQuickWindow*> windowList = view->rootObject()->findChildren<QQuickWindow*>("window");
        QVERIFY(windowList.count());
        QQuickWindow *window = windowList[0];

        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
        QVERIFY(ma);
        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
        QVERIFY(ima);
        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
        QVERIFY(filter);
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

        QTest::mouseClick(window, Qt::LeftButton, 0, QPoint(30, 30));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(filterSpy.count(), 1);

        maSpy.clear(); filterSpy.clear();
        QTest::mouseClick(window, Qt::LeftButton, 0, QPoint(20, 20));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(filterSpy.count(), 1);

        delete view;
    }

//    void testCase_DoNotPropagateEvents()
//    {
//        eventCleanup.clear();
//        InverseMouseAreaType *area = testArea("InverseMouseAreaDoNotPropagateEvents.qml");
//        QVERIFY(area);
//        // connect pressed signal to capture mouse object
//        QObject::connect(area, SIGNAL(pressed(QQuickMouseEveQWidgetnt*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
//        QTest::waitForEvents();
//        QVERIFY(eventCleanup.isEmpty());
//    }


//    void testCase_PropagateEvents()
//    {
//        eventCleanup.clear();
//        InverseMouseAreaType *area = testArea("InverseMouseAreaPropagateEvents.qml");
//        QVERIFY(area);
//        // connect pressed signal to capture mouse object
//        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
//        QTest::waitForEvents();
//        QVERIFY(eventCleanup.isEmpty());
//    }

//    void testCase_sensingAreaError()
//    {
//        InverseMouseAreaType *area = testArea("SensingAreaError.qml");
//        QVERIFY(area);

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 20));
//        QTest::waitForEvents();
//        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
//    }

//    void testCase_InverseMouseAreInWindow()
//    {
//        InverseMouseAreaType *area = testArea("InverseMouseAreaInWindow.qml");
//        QVERIFY(area);
//        quickView->show();

//        QList<QQuickWindow *> l = quickView->rootObject()->findChildren<QQuickWindow*>("isawindow");
//        QVERIFY(l.count());

//        QTest::mouseClick(l[0], Qt::LeftButton, 0, QPoint(20, 10));
//        QTest::waitForEvents();
//        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
//    }

//    void testCase_OverlappedMouseArea()
//    {
//        InverseMouseAreaType *area = testArea("OverlappedMouseArea.qml");
//        QVERIFY(area);
//        quickView->show();

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 10));
//        QTest::waitForEvents();
//        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("MA"));
//    }

//    void testCase_InverseMouseAreaOnTop()
//    {
//        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTop.qml");
//        QVERIFY(area);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma1 = quickView->rootObject()->findChild<QQuickItem*>("MA1");
//        QVERIFY(ma1);
//        QQuickItem *ma2 = quickView->rootObject()->findChild<QQuickItem*>("MA2");
//        QVERIFY(ma2);

//        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy ma1Spy(ma1, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

//        // click in the top rectangle
//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 0);
//        QCOMPARE(ma2Spy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        // click in the second rectangle
//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 0);
//        QCOMPARE(ma2Spy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        // click in teh button
//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 85));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 0);
//        QCOMPARE(ma2Spy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);

//        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QCOMPARE(imaDSpy.count(), 1);
//        imaDSpy.clear();

//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QCOMPARE(imaDSpy.count(), 1);
//        imaDSpy.clear();
//    }

//    void testCase_InverseMouseAreaOnTopNoAccept()
//    {
//        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTopNoAccept.qml");
//        QVERIFY(area);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma1 = quickView->rootObject()->findChild<QQuickItem*>("MA1");
//        QVERIFY(ma1);
//        QQuickItem *ma2 = quickView->rootObject()->findChild<QQuickItem*>("MA2");
//        QVERIFY(ma2);

//        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy ma1Spy(ma1, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 1);
//        QCOMPARE(ma2Spy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        ma1Spy.clear(); imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 0);
//        QCOMPARE(ma2Spy.count(), 1);
//        QCOMPARE(imaSpy.count(), 1);
//        ma2Spy.clear(); imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 80));
//        QTest::waitForEvents();
//        QCOMPARE(ma1Spy.count(), 0);
//        QCOMPARE(ma2Spy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);

//        // double click should not reach inverse mouse area as onPressed did not accept the events
//        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QCOMPARE(imaDSpy.count(), 0);
//        imaDSpy.clear();

//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QCOMPARE(imaDSpy.count(), 0);
//        imaDSpy.clear();
//    }

//    void testCase_InverseMouseAreaOnTopTopmost()
//    {
//        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTop.qml");
//        QVERIFY(area);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);
//        area->setProperty("topmostItem", true);

//        QQuickItem *ma2 = quickView->rootObject()->findChild<QQuickItem*>("MA2");
//        QVERIFY(ma2);

//        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QTest::waitForEvents();
//        QCOMPARE(ma2Spy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QTest::waitForEvents();
//        QCOMPARE(ma2Spy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 80));
//        QTest::waitForEvents();
//        QCOMPARE(ma2Spy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);

//        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
//        QCOMPARE(imaDSpy.count(), 1);
//        imaDSpy.clear();

//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
//        QCOMPARE(imaDSpy.count(), 1);
//        imaDSpy.clear();
//    }

//    void testCase_InverseMouseAreaSignals()
//    {
//        InverseMouseAreaType *area = testArea("InverseMouseAreaSignals.qml");
//        QVERIFY(area);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QSignalSpy pressSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy releaseSpy(area, SIGNAL(released(QQuickMouseEvent*)));
//        QSignalSpy clickSpy(area, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy enteredSpy(area, SIGNAL(entered()));
//        QSignalSpy exitedSpy(area, SIGNAL(exited()));

//        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(5, 5));
//        QCOMPARE(pressSpy.count(), 1);
//        QCOMPARE(releaseSpy.count(), 1);
//        QCOMPARE(clickSpy.count(), 1);
//        QCOMPARE(enteredSpy.count(), 1);
//        QCOMPARE(exitedSpy.count(), 1);

//        QSignalSpy doubleClickSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(5, 5));
//        QCOMPARE(doubleClickSpy.count(), 1);
//    }

//    void testCase_InverseMouseAreaNormalEventStack()
//    {
//        InverseMouseAreaType *ima = testArea("InverseMouseAreaNormalEventStack.qml");
//        QVERIFY(ima);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
//        QVERIFY(ma);

//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(maSpy.count(), 0);

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(maSpy.count(), 1);
//        maSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(maSpy.count(), 0);
//    }

//    void testCase_InverseMouseAreaTopmost()
//    {
//        InverseMouseAreaType *ima = testArea("InverseMouseAreaTopmostItem.qml");
//        QVERIFY(ima);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
//        QVERIFY(ma);

//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaDblClick(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(maSpy.count(), 0);

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(maSpy.count(), 0);

//        imaSpy.clear();
//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(maSpy.count(), 0);

//        QTest::mouseDClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
//        QCOMPARE(imaDblClick.count(), 1);
//    }

//    void testCase_InverseMouseAreaSensingArea()
//    {
//        InverseMouseAreaType *ima = testArea("InverseMouseAreaSensingArea.qml");
//        QVERIFY(ima);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
//        QVERIFY(ma);

//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        maSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//    }

//    void testCase_InverseMouseAreaSensingAreaChange()
//    {
//        InverseMouseAreaType *ima = testArea("InverseMouseAreaSensingArea.qml");
//        QVERIFY(ima);
//        quickView->show();
//        QTest::qWaitForWindowExposed(quickView);

//        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
//        QVERIFY(ma);

//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        maSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        maSpy.clear();

//        ima->setProperty("sensingArea", QVariant());

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        imaSpy.clear();

//        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//    }

};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefiltertest.moc"
