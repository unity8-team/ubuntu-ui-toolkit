/*
 * Copyright 2015-2016 Canonical Ltd.
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
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)


    Component {
        id: flickableComp
        Item {
            id: item
            width: units.gu(20)
            height: units.gu(30)
            property alias flickable: freshFlickable
            property alias scrollbar: freshScrollbar
            property alias content: content
            property alias scrollbarAlignment: freshScrollbar.align

            //Don't change this to a ListView, this usecase has to be simple,
            //we don't want the difficulties added by ListView, i.e. delegates
            //size estimation, dynamic contentWidth/Height, etc
            Flickable {
                id: freshFlickable
                anchors.fill: parent
                contentHeight: content.height
                contentWidth: content.width
                clip: true
                Rectangle {
                    id: content
                    width: units.gu(40)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    color: "blue"
                }
            }
            Scrollbar {
                id: freshScrollbar
                flickableItem: parent.flickable
            }
        }
    }

    SignalSpy {
        id: signalSpy
    }
    SignalSpy {
        id: anotherSignalSpy
    }

    Flickable {
        anchors.fill: parent
        Column {
            id: column
            Repeater {
                model: layoutsModel
            }
        }
    }

    VisualItemModel {
        id: layoutsModel
        Item {
            width: childrenRect.width
            height: childrenRect.height
            Flickable {
                id: flickable_bottomAlign_anchors
            }
            Scrollbar {
                id: scrollbar_bottomAlign_anchors
                flickableItem: flickable_bottomAlign_anchors
                align: Qt.AlignBottom
            }

            Flickable {
                id: flickable_topAlign_anchors
            }
            Scrollbar {
                id: scrollbar_topAlign_anchors
                flickableItem: flickable_topAlign_anchors
                align: Qt.AlignBottom
            }

            Flickable {
                id: flickable_leadingAlign_anchors
            }
            Scrollbar {
                id: scrollbar_leadingAlign_anchors
                flickableItem: flickable_leadingAlign_anchors
                align: Qt.AlignLeading
            }

            Flickable {
                id: flickable_trailingAlign_anchors
            }
            Scrollbar {
                id: scrollbar_trailingAlign_anchors
                flickableItem: flickable_trailingAlign_anchors
                align: Qt.AlignTrailing
            }

            Flickable {
                id: defaultValuesFlickable
            }
            Scrollbar {
                id: defaultValuesScrollbar
                flickableItem: defaultValuesFlickable
            }

            //Items used for values sanity checks
            Flickable {
                id: flickableSanityCheck
                width: units.gu(30)
                height: units.gu(40)
                Rectangle {
                    id: contentSanityCheck
                    width: units.gu(20)
                    height: units.gu(30)
                    color: "yellow"
                }
            }
            Scrollbar {
                id: scrollbarSanityCheck
                flickableItem: flickableSanityCheck
            }
            //complex PageHeader example taken from the PageHeader tutorial
            //https://developer.ubuntu.com/en/blog/2016/02/24/pageheader-tutorial/
            MainView {
                id: mainView_movingHeaderTest
                width: units.gu(50)
                height: units.gu(80)
                clip: true

                property alias page: pageItem
                property alias standardHeader: standardHeaderItem
                property alias editHeader: editHeaderItem

                Page {
                    id: pageItem
                    header: standardHeaderItem

                    Flickable {
                        id: flickable_movingHeaderTest
                        anchors.fill: parent
                        //just make sure the scrollbar is scrollable
                        contentHeight: mainView_movingHeaderTest.height * 2
                        contentWidth: mainView_movingHeaderTest.width * 2
                        Label {
                            text: "Use the icons in the header."
                            visible: standardHeaderItem.visible
                        }
                    }
                    Scrollbar {
                        id: scrollbar_movingHeaderTest
                        flickableItem: flickable_movingHeaderTest
                    }

                    PageHeader {
                        id: standardHeaderItem
                        visible: pageItem.header === standardHeaderItem
                        title: "Default title"
                        flickable: flickable_movingHeaderTest
                        trailingActionBar.actions: [
                            Action {
                                iconName: "edit"
                                text: "Edit"
                                onTriggered: pageItem.header = editHeaderItem
                            }
                        ]
                    }
                    PageHeader {
                        id: editHeaderItem
                        visible: pageItem.header === editHeaderItem
                        flickable: flickable_movingHeaderTest
                        property Component delegate: Component {
                            AbstractButton {
                                id: button
                                action: modelData
                                width: label.width + units.gu(4)
                                height: parent.height
                                Rectangle {
                                    color: UbuntuColors.slate
                                    opacity: 0.1
                                    anchors.fill: parent
                                    visible: button.pressed
                                }
                                Label {
                                    anchors.centerIn: parent
                                    id: label
                                    text: action.text
                                    font.weight: text === "Confirm"
                                                 ? Font.Normal
                                                 : Font.Light
                                }
                            }
                        }
                        leadingActionBar {
                            anchors.leftMargin: 0
                            actions: Action {
                                text: "Cancel"
                                iconName: "close"
                                onTriggered: pageItem.header = standardHeaderItem
                            }
                            delegate: editHeaderItem.delegate
                        }
                        trailingActionBar {
                            anchors.rightMargin: 0
                            actions: Action {
                                text: "Confirm"
                                iconName: "tick"
                                onTriggered: pageItem.header = standardHeaderItem
                            }
                            delegate: editHeaderItem.delegate
                        }
                        extension: Toolbar {
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            trailingActionBar.actions: [
                                Action { iconName: "bookmark-new" },
                                Action { iconName: "add" },
                                Action { iconName: "edit-select-all" },
                                Action { iconName: "edit-copy" },
                                Action { iconName: "select" }
                            ]
                            leadingActionBar.actions: Action {
                                iconName: "delete"
                                text: "delete"
                                onTriggered: print("Delete action triggered")
                            }
                        }
                    }
                }
            }

        }
    }

    ScrollbarTestCase {
        name: "Scrollbar"

        function getFreshFlickable(alignment) {
            var wrapper = flickableComp.createObject(column, { scrollbarAlignment: alignment } )
            verify(wrapper !== null, "Error: dynamic item creation failed.")
            compare(wrapper.scrollbar.align, alignment, "getFreshFlickable: wrong alignment.")
            currComponent = wrapper
            return currComponent
        }
        function getMovingHeaderView() {
            var wrapper = movingHeaderHandlingItem.createObject(column)
            verify(wrapper !== null, "Error: dynamic item creation failed.")
            currComponent = wrapper
            return currComponent
        }

        function cleanup() {
            if (currComponent) {
                currComponent.destroy()
                currComponent = null
            }
            gc()
        }

        function init_data() {
            return [
                        { tag: "vertical scrollbar", alignment: Qt.AlignTrailing },
                        { tag: "horizontal scrollbar", alignment: Qt.AlignBottom }
                    ]
        }

        function clickOnStepperAndCheckNoContentPositionChange(msgPrefix, itemToClickOn, flickable, expectedContentX, expectedContentY) {
            clickInTheMiddle(itemToClickOn)
            wait(150)
            checkNoContentPositionChange(msgPrefix, flickable, expectedContentX, expectedContentY)
        }

        //NOTE: this does not test that the properties have the correct value, just that
        //their values is not out of range (so it only applies to numeric values or enums)
        function performStyleSanityCheck(scrollbar) {
            var style = scrollbar.__styleInstance
            /*****************************************************
             *      STYLING PROPERTIES                           *
             *****************************************************/
            verify(style.minimumSliderSize >= 0, "Sanity check: invalid property value.")
            verify(style.overlayOpacityWhenShown > 0, "Sanity check: invalid property value.")
            verify(style.overlayOpacityWhenHidden >= 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessSteppersStyle > 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessThumbStyle > 0, "Sanity check: invalid property value.")
            verify(style.troughThicknessIndicatorStyle > 0, "Sanity check: invalid property value.")
            verify(style.sliderRadius >= 0, "Sanity check: invalid property value.")
            verify(style.thumbThickness > 0, "Sanity check: invalid property value.")
            verify(style.indicatorThickness > 0, "Sanity check: invalid property value.")
            verify(style.scrollbarThicknessAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeInAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeOutAnimation.duration >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarFadeOutPause >= 0, "Sanity check: invalid property value.")
            verify(style.scrollbarCollapsePause >= 0, "Sanity check: invalid property value.")
            verify(style.shortScrollingRatio > 0, "Sanity check: invalid property value.")
            verify(style.longScrollingRatio > 0, "Sanity check: invalid property value.")
            verify(style.hintingStyle === 'thumb'
                   || style.hintingStyle === 'indicator', "Sanity check: invalid property value.")
            verify(style.thumbsExtremesMargin >= 0, "Sanity check: invalid property value.")

            /*****************************************************
             *      HELPER PROPERTIES                            *
             *****************************************************/
            var slider = getThumb(scrollbar)
            var trough = getTrough(scrollbar)
            verify(!!style.thumb, "Sanity check: invalid property value.")
            verify(!!style.trough, "Sanity check: invalid property value.")
            //flickable helper properties
            verify(!!style.flickableItem, "Sanity check: invalid property value.")

            /*****************************************************
             *      INTERNAL PROPERTIES AND FUNCTIONS            *
             *****************************************************/
            var thumbArea = getThumbArea(scrollbar)
            var scrollAnimation = getScrollAnimation(scrollbar)
            verify(style.nonOverlayScrollbarMargin >= 0, "Sanity check: invalid property value.")
            verify(style.touchDragStartMargin >= 0, "Sanity check: invalid property value.")
            verify(style.dragThreshold >= 0, "Sanity check: invalid property value.")
        }

        function test_bottomAlign_anchors() {
            compare(scrollbar_bottomAlign_anchors.flickableItem,
                    flickable_bottomAlign_anchors, "wrong flickableItem")
            compare(scrollbar_bottomAlign_anchors.anchors.left,
                    scrollbar_bottomAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.right,
                    scrollbar_bottomAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.bottom,
                    scrollbar_bottomAlign_anchors.flickableItem.bottom, "bottom anchor")
            //we can't check that the top anchor is not anchored
        }
        function test_topAlign_anchors() {
            compare(scrollbar_topAlign_anchors.flickableItem,
                    flickable_topAlign_anchors, "wrong flickableItem")
            compare(scrollbar_topAlign_anchors.anchors.left,
                    scrollbar_topAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_topAlign_anchors.anchors.right,
                    scrollbar_topAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_topAlign_anchors.anchors.top,
                    scrollbar_topAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_leadingAlign_anchors() {
            compare(scrollbar_leadingAlign_anchors.flickableItem,
                    flickable_leadingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_leadingAlign_anchors.anchors.left,
                    scrollbar_leadingAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.bottom,
                    scrollbar_leadingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.top,
                    scrollbar_leadingAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_trailingAlign_anchors() {
            compare(scrollbar_trailingAlign_anchors.flickableItem,
                    flickable_trailingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_trailingAlign_anchors.anchors.right,
                    scrollbar_trailingAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.bottom,
                    scrollbar_trailingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.top,
                    scrollbar_trailingAlign_anchors.flickableItem.top, "top anchor")
        }

        function test_indicatorStyleWhileFlicking_shortContent(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            compare(scrollbar.__styleInstance.veryLongContentItem, false, "Scrollable item should be short.")

            //The final "1" is a workaround to bug #1549256
            flick(flickable, 2, 2, -units.gu(10), -units.gu(10), undefined, undefined, undefined, undefined, 1)

            tryCompare(flickable, "moving", true, 5000, "Flickable not moving after simulating a flick.")

            compare(scrollbar.__styleInstance.state, "indicator", "Wrong style while flicking.")

            //we don't set it up before because the hinting feature already changes the state
            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")

            //wait for the flickable to stop
            tryCompare(flickable, "moving", false, 5000, "Flickable still moving after timeout.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after the item stopped moving.")
        }

        function test_thumbStyleWhileFlicking_veryLongContent(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            setupSignalSpy(signalSpy, scrollbar.__styleInstance, "veryLongContentItemChanged")
            setVeryLongContentItem(flickable, scrollbar.__styleInstance, false)
            signalSpy.wait()

            compare(scrollbar.__styleInstance.veryLongContentItem, true, "Very long content item not detected")

            //The final "1" is a workaround to bug #1549256
            flick(flickable, 2, 2, -units.gu(10), -units.gu(10), undefined, undefined, undefined, undefined, 1)

            tryCompare(flickable, "moving", true, 5000, "Flickable not moving after simulating a flick.")

            compare(scrollbar.__styleInstance.state, "thumb", "Wrong style while flicking a very long item")

            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")
            //wait for the flickable to stop
            tryCompare(flickable, "moving", false, 5000, "Flickable still moving after timeout.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after a the item stopped moving.")
        }

        function test_scrollingWithContentMargins(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = freshTestItem.scrollbar.__styleInstance
            var scrollbarUtils = getScrollbarUtils(scrollbar)
            //to make sure that it's only scrolling in the direction it should
            var notScrollingProperty = scrollbarUtils.propContent === "contentX" ? "contentY" : "contentX"
            addContentMargins(flickable)

            style.scrollToBeginning(false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical ? -flickable.topMargin : -flickable.leftMargin,
                                       "Wrong beginning contentY")

            style.scrollToEnd(false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical
                    ? flickable.contentHeight + flickable.bottomMargin - flickable[scrollbarUtils.propSize]
                    : flickable.contentWidth + flickable.rightMargin - flickable[scrollbarUtils.propSize],
                      "Wrong end contentY")

            //contentHeight+top+bottom is the max you can scroll anyway, so it's
            //safe to use it, and the scrollbar should go to returnToBounds() if it goes beyond
            var maxScrolling = style.totalContentSize
            style.scrollBy(-maxScrolling, false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical ? -flickable.topMargin : -flickable.leftMargin,
                                       "scrollBy: wrong contentY after scrolling to the beginning")

            style.scrollBy(maxScrolling, false)
            compare(flickable[notScrollingProperty], 0, "ContentX changed when it shouldn't")
            compare(flickable[scrollbarUtils.propContent],
                    style.isVertical
                    ? flickable.contentHeight + flickable.bottomMargin - flickable[scrollbarUtils.propSize]
                    : flickable.contentWidth + flickable.rightMargin - flickable[scrollbarUtils.propSize],
                      "scrollBy: wrong contentY after scrolling to the end")

            //we can only scroll up now
            var oldContentProp = flickable[scrollbarUtils.propContent]
            style.scrollBy(-units.gu(1), false)
            compare(flickable[scrollbarUtils.propContent],
                    oldContentProp - units.gu(1),
                    "scrollBy: wrong contentY after scrolling by +1GU")
            style.scrollBy(units.gu(1), false)
            compare(flickable[scrollbarUtils.propContent],
                    oldContentProp,
                    "scrollBy: wrong contentY after scrolling by -1GU")
        }

        function test_showSteppers(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var steppersTransition = findInListProperty(scrollbar.__styleInstance.transitions, "indicatorToThumbSteppersTransition")
            verify(steppersTransition !== null, "Could not find transition object")

            setupSignalSpy(signalSpy, scrollbar.__styleInstance, "stateChanged")
            setupSignalSpy(anotherSignalSpy, steppersTransition, "runningChanged")

            //show steppers on mouse move
            mouseMove(scrollbar, 0, 0)

            signalSpy.wait()
            compare(signalSpy.count, 1, "Mouse move doesn't trigger state change")
            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "Mouse move doesn't trigger state transition")

            compare(scrollbar.__styleInstance.state, "steppers", "Mouse move doesn't trigger steppers state.")
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var trough = getTrough(scrollbar)

            compare(trough.visible, true, "Trough not visible in steppers state")
            compare(firstStepper.visible, true, "First stepper not visible in steppers state")
            compare(secondStepper.visible, true, "Second stepper not visible in steppers state")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 2, "State transition does not complete.")
            compare(steppersTransition.running, false, "State transition does not stop.")

            if (scrollbar.__styleInstance.isVertical) {
                compare(trough.width, scrollbar.__styleInstance.troughThicknessSteppersStyle, "Wrong trough thickness in steppers style")
            } else {
                compare(trough.height, scrollbar.__styleInstance.troughThicknessSteppersStyle, "Wrong trough thickness in steppers style")
            }
        }


        function test_dragThumb(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var thumb = getThumb(scrollbar)
            var thumbArea = getThumbArea(scrollbar)
            var trough = getTrough(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var scrollbarUtils = getScrollbarUtils(scrollbar)

            addContentMargins(flickable)

            setContentPositionToTopLeft(flickable)

            triggerSteppersMode(scrollbar)

            if (style.isVertical) {
                mouseDrag(thumb, thumb.width/2, thumb.height/2, 0, trough.height)
                compare(flickable[scrollbarUtils.propContent],
                        flickable.contentHeight + flickable.bottomMargin - flickable.height,
                        "Vertical thumb mouse drag: wrong contentProp after dragging to the end")

                var sceneThumbY = thumb.mapToItem(column).y

                //Cannot use mouseDrag here, because the thumb is at the end of the trough.
                //mouseDrag uses
                //mouseMove(item, x + util.dragThreshold + 1, y + util.dragThreshold + 1, delay, button)
                //to trigger the drag, but in our case that cannot work because the scrollbar is at
                //the end of the trough, hence cannot move to its
                //bottom (vertical scrollbar) or to its right (in the case of the horiz scrollbar)
                mousePress(thumb, thumb.width/2, thumb.height/2)
                mouseMove(thumb, thumb.width/2, 0  )
                mouseMove(thumb, thumb.width/2, -sceneThumbY)
                mouseRelease(thumb, 0, 0)

                compare(flickable[scrollbarUtils.propContent], -flickable.topMargin,
                        "Vertical thumb mouse drag: wrong contentProp after dragging to the beginning")
            } else {
                mouseDrag(thumb, thumb.width/2, thumb.height/2, trough.width, 0)
                compare(flickable[scrollbarUtils.propContent],
                        flickable.contentWidth + flickable.rightMargin - flickable.width,
                        "Horizontal thumb mouse drag: wrong contentProp after dragging to the end")

                var sceneThumbX = thumb.mapToItem(column).x
                //Can't use mouseDrag here, read above to know why
                mousePress(thumb, thumb.width/2, thumb.height/2)
                mouseMove(thumb, 0, thumb.height/2  )
                mouseMove(thumb, -sceneThumbX, thumb.height/2)
                mouseRelease(thumb, 0, 0)

                compare(flickable[scrollbarUtils.propContent], -flickable.leftMargin,
                        "Horizontal thumb mouse drag: wrong contentProp after dragging to the beginning")
            }
        }

        function test_hinting_data() {
            return [
                        //ownSide here is the side the scrollable cares about, i.e. height for vert scrollbar
                        {tag: "vertical scrollbar, no scrollable side, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: false, veryLong: false },
                        {tag: "horizontal scrollbar, no scrollable side, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: false, veryLong: false },
                        {tag: "vertical scrollbar, horizontal side scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: false, veryLong: false },
                        {tag: "horizontal scrollbar, horizontal side scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: false, veryLong: false },
                        {tag: "vertical scrollbar, vertical side scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: true, veryLong: false },
                        {tag: "horizontal scrollbar, vertical side scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: true, veryLong: false },
                        {tag: "vertical scrollbar, both sides scrollable, short content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: true, veryLong: false },
                        {tag: "horizontal scrollbar, both sides scrollable, short content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: true, veryLong: false },
                        {tag: "vertical scrollbar, no scrollable side, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: false, veryLong: true },
                        {tag: "horizontal scrollbar, no scrollable side, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: false, veryLong: true },
                        {tag: "vertical scrollbar, horizontal side scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: false, veryLong: true },
                        {tag: "horizontal scrollbar, horizontal side scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: false, veryLong: true },
                        {tag: "vertical scrollbar, vertical side scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: false, scrollableVertically: true, veryLong: true },
                        {tag: "horizontal scrollbar, vertical side scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: false, scrollableVertically: true, veryLong: true },
                        {tag: "vertical scrollbar, both sides scrollable, long content item",
                            alignment: Qt.AlignTrailing, scrollableHorizontally: true, scrollableVertically: true, veryLong: true },
                        {tag: "horizontal scrollbar, both sides scrollable, long content item",
                            alignment: Qt.AlignBottom, scrollableHorizontally: true, scrollableVertically: true, veryLong: true },
                    ];
        }

        //test that the scrollbar is shown on contentHeight/width changes
        function test_hinting(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var style = freshTestItem.scrollbar.__styleInstance

            //we'll add it 1gu to simulate a change that doesn't make it become scrollable
            //so we need to make sure adding 1gu doesn't make it scrollable
            verify(flickable.width > units.gu(5), "Flickable width assumption is satisfied.")
            verify(flickable.height > units.gu(5), "Flickable height assumption is satisfied.")
            flickable.contentWidth = units.gu(2)
            flickable.contentHeight = units.gu(2)
            checkNonScrollableState(scrollbar)

            if (data.scrollableHorizontally) {
                if (data.veryLong) {
                    setVeryLongContentItem(flickable, style, true)
                    tryCompare(style, "veryLongContentItem", true, 5000, "Hinting: veryLongContentItem should be true.")
                } else {
                    flickable.contentWidth = flickable.width + units.gu(1)
                }
                if (!style.isVertical) {
                    checkHinting(scrollbar)
                    //wait for the hinting to finish
                    checkScrollableAndHidden(scrollbar)
                } else {
                    //make sure the scrollbar was not affected by the size change of the opposite side
                    checkNonScrollableState(scrollbar)
                }
            }

            if (data.scrollableVertically) {
                if (data.veryLong) {
                    setVeryLongContentItem(flickable, style, false)
                    tryCompare(style, "veryLongContentItem", true, 5000, "Hinting: veryLongContentItem should be true.")
                } else {
                    flickable.contentHeight = flickable.height + units.gu(1)
                }
                if (style.isVertical) {
                    checkHinting(scrollbar)
                    checkScrollableAndHidden(scrollbar)
                } else {
                    //Only make sure that the horiz scrollbar is not scrollable IF
                    //we didn't previously also make the horizontal side scrollable
                    if (!data.scrollableHorizontally) {
                        checkNonScrollableState(scrollbar)
                    }
                }
            }

            //try changing contentHeight
            flickable.contentHeight += units.gu(1)
            if (style.isVertical && data.scrollableVertically) {
                checkHinting(scrollbar)
                checkScrollableAndHidden(scrollbar)
            } else if (!style.isVertical && data.scrollableHorizontally) {
                compare(style.state, "hidden", "Wrong state.")
            } else {
                checkNonScrollableState(scrollbar)
            }

            flickable.contentWidth += units.gu(1)
            if (!style.isVertical && data.scrollableHorizontally) {
                checkHinting(scrollbar)
                checkScrollableAndHidden(scrollbar)
            } else if (style.isVertical && data.scrollableVertically) {
                compare(style.state, "hidden", "Wrong state.")
            } else {
                checkNonScrollableState(scrollbar)
            }
        }

        function test_minimumSliderSize(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var trough = getTrough(scrollbar)
            var thumb = getThumb(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var minSize = style.minimumSliderSize

            verify(trough !== null, "Could not find the trough.")
            verify(thumb !== null, "Could not find the thumb.")

            if (style.isVertical) {
                //ignore margins etc, just go for an upper bound
                flickable.contentHeight = flickable.height * (flickable.height / minSize) + units.gu(100)
                compare(thumb.height, minSize, "Thumb does not respect the minimum size.")
            } else {
                flickable.contentWidth = flickable.width * (flickable.width / minSize) + units.gu(100)
                compare(thumb.width, minSize, "Thumb does not respect the minimum size.")
            }
        }

        function test_scrollingRatioValuesSanityCheck() {
            var freshTestItem = getFreshFlickable(Qt.AlignTrailing)
            var style = freshTestItem.scrollbar.__styleInstance
            verify(style.shortScrollingRatio > 0, true,
                   "Short scrolling ratio must be > 0.")
            verify(style.longScrollingRatio > 0, true,
                   "Short scrolling ratio must be > 0.")
            verify(style.shortScrollingRatio <= style.longScrollingRatio, true,
                   "Short scrolling ratio is higher than long scrolling ratio.")
        }

        //        NOTE: ANY FAILURE COMING AFTER MOUSEPRESS WILL LEAVE THE MOUSE IN A DIRTY STATE!
        //        (QQuickWindow's mouseGrabberItem is not reset)
        //        OTHER TESTS WILL FAIL JUST BECAUSE THIS TEST HAS NOT CALLED MOUSERELEASE!
        //        I REPORTED https://bugreports.qt.io/browse/QTBUG-51193 TO GET THIS FIXED
        function test_resetDragWhenMouseIsVeryFarFromTheThumb(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var thumb = getThumb(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance

            compare(style.isScrollable, true, "Item is assumed to be scrollable.")
            triggerSteppersMode(scrollbar)

            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY

            //coords of the 2 mouse move used to simulate a drag
            var firstStep = units.gu(5)
            var secondStep = units.gu(10)
            var releaseCoord = units.gu(11)

            mousePress(thumb, 0, 0)
            mouseMove(thumb,
                      (style.isVertical ? 0 : firstStep),
                      (style.isVertical ? firstStep : 0))
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep),
                      (style.isVertical ? secondStep : 0))

            var newContentX = flickable.contentX
            var newContentY = flickable.contentY

            if (style.isVertical) {
                verify(newContentY !== contentYBackup, "Check that mouse drag changed contentY." )
            } else {
                verify(newContentX !== contentXBackup, "Check that mouse drag changed contentX." )
            }

            mouseMove(thumb,
                      (style.isVertical ? dragResetThreshold(scrollbar)+1 : secondStep),
                      (style.isVertical ? secondStep : dragResetThreshold(scrollbar)+1))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            //go back to the old position
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep),
                      (style.isVertical ? secondStep : 0))
            //resume the drag by moving at least 1 px (scrollbar should have 0 drag threshold)
            mouseMove(thumb,
                      (style.isVertical ? 0 : secondStep + style.dragThreshold + 1),
                      (style.isVertical ? secondStep + style.dragThreshold + 1 : 0))

            //NOTE: now the thumb has moved, adding another mouseMove here will use
            //coordinates relative to the NEW position!
            mouseMove(thumb,
                      (style.isVertical ? 0 : -(style.dragThreshold + 1)),
                      (style.isVertical ? -(style.dragThreshold + 1) : 0))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            //test that we resume dragging
            if (style.isVertical) {
                verify(newContentY !== contentYBackup, "Check that mouse drag changed contentY." )
            } else {
                verify(newContentX !== contentXBackup, "Check that mouse drag changed contentX." )
            }

            //try moving the other way
            //FIXME: I'M USING +7 BECAUSE THE LOGIC USES (mouse.x - thumbArea.x)
            //AND HERE WE START AT 5px AND THEN MOVE BY 10x + 1
            //THE LOGIC HAS TO USE A DIFFERENT CONDITION
            mouseMove(thumb,
                      (style.isVertical ? -(dragResetThreshold(scrollbar) + 7) : secondStep),
                      (style.isVertical ? secondStep : -(dragResetThreshold(scrollbar) + 7)))

            //check that it goes back to the pre-drag content position, as if the drag never started
            checkNoContentPositionChange("Mouse drag reset", flickable, contentXBackup, contentYBackup)

            mouseRelease(thumb,
                         (style.isVertical ? -(dragResetThreshold(scrollbar) + 7) : secondStep),
                         (style.isVertical ? secondStep : -(dragResetThreshold(scrollbar) + 7)))
        }

        function test_actionSteppers_data() {
            return [
                        {tag:"Content item with margins and vertical scrollbar", addContentMargins: true, alignment: Qt.AlignTrailing},
                        {tag:"Content item and vertical scrollbar", addContentMargins: false, alignment: Qt.AlignTrailing},
                        {tag:"Content item with margins and horizontal scrollbar", addContentMargins: true, alignment: Qt.AlignBottom},
                        {tag:"Content item and horizontal scrollbar", addContentMargins: false, alignment: Qt.AlignBottom}
                    ];
        }
        function test_actionSteppers(data) {
            var freshTestItem = getFreshFlickable(data.alignment)
            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar
            var trough = getTrough(scrollbar)
            var thumb = getThumb(scrollbar)
            var firstStepper = getFirstStepper(scrollbar)
            var secondStepper = getSecondStepper(scrollbar)
            var style = freshTestItem.scrollbar.__styleInstance
            var minSize = style.minimumSliderSize

            if (data.addContentMargins) {
                addContentMargins(flickable)
            }

            triggerSteppersMode(scrollbar)

            setContentPositionToTopLeft(flickable)
            var contentXBackup = flickable.contentX
            var contentYBackup = flickable.contentY
            clickOnStepperAndCheckNoContentPositionChange("First stepper", firstStepper, flickable,
                                                          contentXBackup, contentYBackup)

            //NOTE: mouseClick(secondStepper, 0, 0) would trigger firstStepper, because
            //the logic uses contains(point) that also counts the edges!
            //(the code uses if firstStepper.contains else if secondStepper.contains
            //so it stops at the first branch)
            clickInTheMiddle(secondStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, 1, "Second stepper, first click")

            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            //do it once more
            clickInTheMiddle(secondStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, 1, "Second stepper, second click")

            setContentPositionToBottomRight(flickable)
            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            clickOnStepperAndCheckNoContentPositionChange("Second stepper", secondStepper, flickable,
                                                          contentXBackup, contentYBackup)

            clickInTheMiddle(firstStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, -1, "First stepper, first click")

            contentXBackup = flickable.contentX
            contentYBackup = flickable.contentY

            //do it once more
            clickInTheMiddle(firstStepper)
            checkScrolling(flickable, contentXBackup, contentYBackup, style,
                           false, -1, "First stepper, second click")

        }

        function test_defaultStylingValues() {
            var scrollbar = defaultValuesScrollbar
            var style = scrollbar.__styleInstance

            /*****************************************************
             *      STYLING PROPERTIES                           *
             *****************************************************/
            compare(style.interactive, style.isMouseConnected || style.veryLongContentItem, "Wrong styling property default value.")
            compare(style.minimumSliderSize, units.gu(3), "Wrong styling property default value.")
            compare(style.overlay, !style.alwaysOnScrollbars, "Wrong styling property default value.")
            compare(style.overlayOpacityWhenShown, 1.0, "Wrong styling property default value.")
            compare(style.overlayOpacityWhenHidden, 0.0, "Wrong styling property default value.")
            compare(style.troughThicknessSteppersStyle, units.dp(14), "Wrong styling property default value.")
            compare(style.troughThicknessThumbStyle, units.dp(14), "Wrong styling property default value.")
            compare(style.troughThicknessIndicatorStyle, units.dp(9), "Wrong styling property default value.")
            compare(style.troughColorThumbStyle, theme.palette.normal.foreground, "Wrong styling property default value.")
            compare(style.troughColorSteppersStyle, theme.palette.normal.foreground, "Wrong styling property default value.")
            compare(style.sliderColor, theme.palette.normal.foregroundText, "Wrong styling property default value.")
            compare(style.sliderRadius, units.dp(3), "Wrong styling property default value.")
            compare(style.thumbThickness, units.gu(1), "Wrong styling property default value.")
            compare(style.indicatorThickness, units.dp(3), "Wrong styling property default value.")
            compare(style.scrollbarThicknessAnimation.duration, UbuntuAnimation.SnapDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeInAnimation.duration, UbuntuAnimation.SlowDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeOutAnimation.duration, UbuntuAnimation.SlowDuration, "Wrong styling property default value.")
            compare(style.scrollbarFadeOutPause, 3000, "Wrong styling property default value.")
            compare(style.scrollbarCollapsePause, 1000, "Wrong styling property default value.")
            compare(style.shortScrollingRatio, 0.1, "Wrong styling property default value.")
            compare(style.longScrollingRatio, 0.9, "Wrong styling property default value.")
            compare(style.hintingStyle, style.veryLongContentItem ? 'thumb' : 'indicator', "Wrong styling property default value.")
            compare(style.thumbsExtremesMargin, units.dp(4), "Wrong styling property default value.")

            /*****************************************************
             *      HELPER PROPERTIES                            *
             *****************************************************/
            var slider = getThumb(scrollbar)
            var trough = getTrough(scrollbar)
            compare(style.thumb, slider, "Wrong styling property default value.")
            compare(style.trough, trough, "Wrong styling property default value.")
            compare(style.isScrollable, scrollbar.__private.scrollable && style.pageSize > 0.0
                    && style.contentSize > 0.0 && style.totalContentSize > style.pageSize, "Wrong styling property default value.")
            compare(style.isVertical, (scrollbar.align === Qt.AlignLeading) || (scrollbar.align === Qt.AlignTrailing), "Wrong styling property default value.")
            compare(style.frontAligned, scrollbar.align === Qt.AlignLeading, "Wrong styling property default value.")
            compare(style.rearAligned, scrollbar.align === Qt.AlignTrailing, "Wrong styling property default value.")
            compare(style.topAligned, scrollbar.align === Qt.AlignTop, "Wrong styling property default value.")
            compare(style.bottomAligned, scrollbar.align === Qt.AlignBottom, "Wrong styling property default value.")

            //flickable helper properties
            compare(style.flickableItem, scrollbar.flickableItem, "Wrong styling property default value.")
            compare(style.pageSize, (style.isVertical) ? (scrollbar.flickableItem.height) : (scrollbar.flickableItem.width), "Wrong styling property default value.")
            compare(style.contentSize, (style.isVertical) ? scrollbar.flickableItem.contentHeight : scrollbar.flickableItem.contentWidth, "Wrong styling property default value.")
            compare(style.leadingContentMargin, style.isVertical ? scrollbar.flickableItem.topMargin : scrollbar.flickableItem.leftMargin, "Wrong styling property default value.")
            compare(style.trailingContentMargin, style.isVertical ? scrollbar.flickableItem.bottomMargin : scrollbar.flickableItem.rightMargin, "Wrong styling property default value.")
            compare(style.totalContentSize, style.contentSize + style.leadingContentMargin + style.trailingContentMargin, "Wrong styling property default value.")

            /*****************************************************
             *      INTERNAL PROPERTIES AND FUNCTIONS            *
             *****************************************************/
            var thumbArea = getThumbArea(scrollbar)
            var scrollAnimation = getScrollAnimation(scrollbar)
            compare(style.__recursionGuard, false, "Wrong styling property default value.")
            compare(style.__disableStateBinding, false, "Wrong styling property default value.")
            compare(style.__hinting, false, "Wrong styling property default value.")
            compare(style.draggingThumb, thumbArea.drag.active || slider.mouseDragging || slider.touchDragging, "Wrong styling property default value.")
            compare(style.thumbStyleFlag, style.veryLongContentItem && (scrollbar.flickableItem.moving || scrollAnimation.running), "Wrong styling property default value.")
            compare(style.veryLongContentItem, scrollbar.flickableItem && style.initialized
                    && ((scrollbar.flickableItem.contentHeight > scrollbar.flickableItem.height * 10)
                        || (scrollbar.flickableItem.contentWidth > scrollbar.flickableItem.width * 10)), "Wrong styling property default value.")
            compare(style.isMouseConnected, true, "Wrong styling property default value.")
            compare(style.nonOverlayScrollbarMargin, style.troughThicknessSteppersStyle, "Wrong styling property default value.")
            compare(style.useSteppers, true, "Wrong styling property default value.")
            compare(style.touchDragStartMargin, units.gu(2), "Wrong styling property default value.")
            compare(style.dragThreshold, units.dp(2), "Wrong styling property default value.")
            compare(style.alwaysOnScrollbars, scrollbar.__alwaysOnScrollbars, "Wrong styling property default value.")
        }

        //just a few tests to ensure the key variables don't get out of range values
        //NOTE: this does not test that the properties have the correct value, just that
        //their values is not out of range (so it only applies to numeric values)
        function test_sanityCheckStylingVariables() {
            flickableSanityCheck.contentHeight = contentSanityCheck.height
            flickableSanityCheck.contentWidth = contentSanityCheck.width
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.contentHeight = -contentSanityCheck.height
            flickableSanityCheck.contentWidth = -contentSanityCheck.width
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.topMargin = units.gu(1)
            flickableSanityCheck.leftMargin = units.gu(2)
            flickableSanityCheck.rightMargin = units.gu(3)
            flickableSanityCheck.bottomMargin = units.gu(4)
            performStyleSanityCheck(scrollbarSanityCheck)

            flickableSanityCheck.topMargin = -units.gu(1)
            flickableSanityCheck.leftMargin = -units.gu(2)
            flickableSanityCheck.rightMargin = -units.gu(3)
            flickableSanityCheck.bottomMargin = -units.gu(4)
            performStyleSanityCheck(scrollbarSanityCheck)

        }

        function checkScrollbarPositionRelativeToPage(scrollbar, page, expectedY, msgPrefix) {
            compare(scrollbar.mapToItem(page).y, expectedY, msgPrefix + ": Scrollbar does not start below the header.")
        }

        function test_handlingOfMovingHeader_data() {
            return [
                        { tag: "Standard header", header: standardHeaderItem },
                        { tag: "Edit header (with extensions)", header: editHeaderItem },
                        { tag: "Null header", header: null }
                    ]
        }
        function test_handlingOfMovingHeader(data) {
            var page = pageItem
            var header = data.header

            page.header = header
            compare(page.header, header, "Handling of moving header: wrong header.")
            compare(scrollbar_movingHeaderTest.__styleInstance.isVertical, true, "Scrollbar is not vertical.")
            compare(scrollbar_movingHeaderTest.__styleInstance.isScrollable, true, "Scrollbar is not scrollable.")

            //don't do the rest of the checks on the null header
            if (data.header === null) {
                checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag)
                return
            } else {
                header.flickable = flickable_movingHeaderTest
                compare(header.flickable, flickable_movingHeaderTest, "Wrong PageHeader flickable.")
                checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag)
            }

            var tmpHeaderHeight = header.height
            page.header.height += units.gu(1)
            //make sure the header actually has a different height now
            verify(scrollbar_movingHeaderTest.height !== tmpHeaderHeight, "Header height changed value.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height,
                                                 data.tag + ", after changing height")

            //link the header to another flickable than the one the scrollbar is linked to, and check
            //that scrollbar now ignores the header
            header.flickable = flickable_trailingAlign_anchors
            compare(header.flickable, flickable_trailingAlign_anchors, "Wrong PageHeader flickable.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag)

            //reassign the correct flickable and check again
            header.flickable = flickable_movingHeaderTest
            compare(header.flickable, flickable_movingHeaderTest, "Wrong PageHeader flickable.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag)

            //now link the header to a null flickable and check that scrollbar ignores the header
            header.flickable = null
            compare(header.flickable, null, "Wrong PageHeader flickable.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag)

            //reassign the correct flickable and check again
            header.flickable = flickable_movingHeaderTest
            compare(header.flickable, flickable_movingHeaderTest, "Wrong PageHeader flickable.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", scrollbar")

            header.visible = false
            compare(header.visible, false, "Header visibility did not change, should have been false.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag + ", invisible header")

            header.visible = true
            compare(header.visible, true, "Header visibility did not change, should have been true.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", visible header")

            //even if the header has opacity 0, we should still take it into account. This is a standard in
            //QtQuick, you don't ignore a component just because opacity is 0, as that is also used for animations
            header.opacity = 0.0
            compare(header.opacity, 0.0, "Header opacity did not change, should have been 0.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", 0.0 header opacity")

            header.opacity = 1.0
            compare(header.opacity, 1.0, "Header opacity did not change, should have been 1.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", 1.0 header opacity")

            page.header = standardHeaderItem
        }

        function test_findPage_data() {
            return [
                        {
                            tag: "No Page",
                            code: 'import QtQuick 2.4;
                                   import Ubuntu.Components 1.3;
                                   Item {
                                        property alias scrollbar: scrollbar
                                        Flickable { id: flickable }
                                        Scrollbar { id: scrollbar; flickableItem: flickable }
                                   }',
                            containsPage: false
                        },
                        {
                            tag: "Page parent",
                            code: 'import QtQuick 2.4;
                                   import Ubuntu.Components 1.3;
                                   Item {
                                        property alias page: page
                                        property alias scrollbar: scrollbar
                                        Page {
                                            id: page;
                                            Flickable { id: flickable }
                                            Scrollbar { id: scrollbar; flickableItem: flickable }
                                        }
                                   }',
                            containsPage: true
                        },
                        {
                            tag: "Page grandparent",
                            code: 'import QtQuick 2.4;
                                   import Ubuntu.Components 1.3;
                                   Item {
                                        property alias page: page
                                        property alias scrollbar: scrollbar
                                        Page {
                                            id: page;
                                            Item {
                                                Flickable { id: flickable }
                                                Scrollbar { id: scrollbar; flickableItem: flickable }
                                            }
                                        }
                                   }',
                            containsPage: true
                        }
                    ]
        }

        function test_findPage(data) {
            var view = Qt.createQmlObject(data.code, column, 'testItem')
            verify(view, "Dynamically created view item is valid.")
            verify(view.scrollbar, "Dynamic view contains a valid Scrollbar.")
            currComponent = view

            if (data.containsPage) {
                verify(view.page, "Dynamic view contains a valid Page.")
                verify(view.scrollbar.__private.page === view.page, "Verify that Scrollbar found the right Page.")
            } else {
                verify(view.scrollbar.__private.page === null, "Verify that Scrollbar did not find any page.")
            }
        }
    }
}
