/*
 * Copyright 2014 Canonical Ltd.
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
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Action {
        id: stockAction
        iconName: "starred"
        property var param
        onTriggered: param = value
    }
    ListItemActions {
        id: leading
        actions: [
            Action {
                iconName: "delete"
                property var param
                onTriggered: param = value
            },
            Action {
                iconName: "edit"
                property var param
                onTriggered: param = value
            },
            Action {
                iconName: "camcorder"
                property var param
                onTriggered: param = value
            }
        ]
    }
    ListItemActions {
        id: trailing
        backgroundColor: leading.backgroundColor
        foregroundColor: leading.foregroundColor
        actions: [
            stockAction,
        ]
        delegate: Item {
            objectName: "custom_delegate"
        }
    }
    ListItemActions {
        id: actionsDefault
    }

    Column {
        width: parent.width
        ListItem {
            id: defaults
            width: parent.width
        }
        ListItem {
            id: testItem
            width: parent.width
            color: "blue"
            leadingActions: leading
            trailingActions: ListItemActions {
                actions: leading.actions
            }
            Item {
                id: bodyItem
                anchors.fill: parent
            }
        }
        ListItem {
            id: controlItem
            Button {
                id: button
                anchors.centerIn: parent
                text: "Button"
            }
        }

        ListView {
            id: listView
            width: parent.width
            height: units.gu(28)
            clip: true
            model: 10
            delegate: ListItem {
                objectName: "listItem" + index
                width: parent.width
                leadingActions: leading
                trailingActions: trailing
            }
        }
        Flickable {
            id: testFlickable
            width: parent.width
            height: units.gu(28)
            ListView {
                id: nestedListView
                width: parent.width
                height: units.gu(28)
                clip: true
                model: 10
                delegate: ListItem {
                    objectName: "listItem" + index
                    leadingActions: leading
                }
            }
        }
    }

    UbuntuTestCase {
        name: "ListItemAPI"
        when: windowShown

        SignalSpy {
            id: pressedSpy
            signalName: "pressedChanged"
            target: testItem
        }

        SignalSpy {
            id: clickSpy
            signalName: "clicked"
            target: testItem;
        }

        SignalSpy {
            id: actionSpy
            signalName: "triggered"
        }

        SignalSpy {
            id: xChangeSpy
            signalName: "xChanged"
        }

        SignalSpy {
            id: interactiveSpy
            signalName: "interactiveChanged"
        }

        SignalSpy {
            id: draggingSpy
            signalName: "draggingChanged"
        }

        function waitReboundCompletion(item) {
            var prevX;
            tryCompareFunction(function() { var b = prevX == item.contentItem.x; prevX = item.contentItem.x; return b; }, true, 1000);
        }

        function panelItem(actionList) {
            return findInvisibleChild(actionList, "ListItemPanel")
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
            waitForRendering(main);
        }

        function cleanup() {
            testItem.action = null;
            testItem.selected = false;
            testItem.selectable = false;
            waitForRendering(testItem, 200);
            pressedSpy.clear();
            clickSpy.clear();
            actionSpy.clear();
            xChangeSpy.clear();
            interactiveSpy.target = null;
            interactiveSpy.clear();
            draggingSpy.clear();
            pressAndHoldSpy.clear();
            buttonSpy.clear();
            listView.interactive = true;
            // tap on the first item to make sure we are rebounding all
            mouseClick(defaults, 0, 0);
            // make sure all events are processed
            wait(200);
        }

        function test_0_defaults() {
            verify(defaults.contentItem !== null, "Defaults is null");
            compare(defaults.color, "#000000", "Transparent by default");
            compare(defaults.highlightColor, Theme.palette.selected.background, "Theme.palette.selected.background color by default")
            compare(defaults.pressed, false, "Not pressed buy default");
            compare(defaults.divider.visible, true, "divider is visible by default");
            compare(defaults.divider.leftMargin, units.gu(2), "divider's left margin is 2GU");
            compare(defaults.divider.rightMargin, units.gu(2), "divider's right margin is 2GU");
            compare(defaults.divider.colorFrom, "#000000", "colorFrom differs.");
            fuzzyCompare(defaults.divider.colorFrom.a, 0.14, 0.01, "colorFrom alpha differs");
            compare(defaults.divider.colorTo, "#ffffff", "colorTo differs.");
            fuzzyCompare(defaults.divider.colorTo.a, 0.07, 0.01, "colorTo alpha differs");

            compare(actionsDefault.delegate, null, "ListItemActions has no delegate set by default.");
            compare(actionsDefault.actions.length, 0, "ListItemActions has no actions set.");
            compare(actionsDefault.backgroundColor, Qt.rgba(0, 0, 0, 0), "default background color is transparent");
            compare(actionsDefault.foregroundColor, "#000000", "default foregroundColor must be black");

            compare(actionsDefault.ListItemActions.container, actionsDefault, "The attached container points to the actions list");
            compare(actionsDefault.ListItemActions.listItem, null, "No attached ListItem by default");
            compare(actionsDefault.ListItemActions.listItemIndex, -1, "No attached ListItem index by default");
            compare(actionsDefault.ListItemActions.offset, 0, "No attached offset set by default");
            compare(actionsDefault.ListItemActions.status, ListItemActions.Disconnected, "The attached status is disconnected");
            compare(actionsDefault.ListItemActions.dragging, false, "The attached dragging is false");
        }

        function test_children_in_content_item() {
            compare(bodyItem.parent, testItem.contentItem, "Content is not in the right holder!");
        }

        function test_pressedChanged_on_click() {
            mousePress(testItem, testItem.width / 2, testItem.height / 2);
            pressedSpy.wait();
            mouseRelease(testItem, testItem.width / 2, testItem.height / 2);
        }
        function test_pressedChanged_on_tap() {
            TestExtras.touchPress(0, testItem, centerOf(testItem));
            pressedSpy.wait();
            TestExtras.touchRelease(0, testItem, centerOf(testItem));
            // local cleanup, wait few msecs to suppress double tap
            wait(400);
        }

        function test_clicked_on_mouse() {
            clickSpy.target = testItem;
            mouseClick(testItem, testItem.width / 2, testItem.height / 2);
            clickSpy.wait();
        }
        function test_clicked_on_tap() {
            clickSpy.target = testItem;
            TestExtras.touchClick(0, testItem, centerOf(testItem));
            clickSpy.wait();
        }

        function test_mouse_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            mousePress(listItem, listItem.width / 2, 0);
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                mouseMove(listItem, listItem.width / 2, dy);
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            mouseRelease(listItem, listItem.width / 2, dy);
        }
        function test_touch_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            TestExtras.touchPress(0, listItem, Qt.point(listItem.width / 2, 5));
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                TestExtras.touchMove(0, listItem, Qt.point(listItem.width / 2, dy));
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            // cleanup, wait few milliseconds to avoid dbl-click collision
            TestExtras.touchRelease(0, listItem, Qt.point(listItem.width / 2, dy));
        }

        function test_background_height_change_on_divider_visible() {
            // make sure the testItem's divider is shown
            testItem.divider.visible = true;
            verify(testItem.contentItem.height < testItem.height, "ListItem's background height must be less than the item itself.");
            testItem.divider.visible = false;
            compare(testItem.contentItem.height, testItem.height, "ListItem's background height must be the same as the item itself.");
            testItem.divider.visible = true;
        }

        function test_touch_tug_actions_data() {
            var item = findChild(listView, "listItem0");
            return [
                {tag: "Trailing, mouse", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: true},
                {tag: "Leading, mouse", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: true},
                {tag: "Trailing, touch", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: false},
                {tag: "Leading, touch", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: false},
            ];
        }
        function test_touch_tug_actions(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 400);
            if (data.positiveDirection) {
                verify(data.item.contentItem.x > 0, data.tag + " actions did not show up");
            } else {
                verify(data.item.contentItem.x < 0, data.tag + " actions did not show up");
            }

            // dismiss
            if (data.mouse) {
                mouseClick(main, 1, 1);
            } else {
                TestExtras.touchClick(0, main, Qt.point(1, 1));
            }
            waitForRendering(data.item, 400);
        }

        // make sure this is executed as one of the last tests due to requirement to have the panelItem created
        function test_attached_dragging_data() {
            var item = findChild(listView, "listItem0");
            return [
                {tag: "Trailing", item: item, pos: centerOf(item), dx: -units.gu(20), actionList: item.trailingActions},
                {tag: "Leading", item: item, pos: centerOf(item), dx: units.gu(20), actionList: item.leadingActions},
            ];
        }
        function test_attached_dragging(data) {
            listView.positionViewAtBeginning();
            draggingSpy.target = data.actionList.ListItemActions;
            flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            waitForRendering(data.item, 400);
            draggingSpy.wait(500);
            compare(draggingSpy.count, 2, "The dragging hadn't been changed twice.");

            // dismiss
            mouseClick(main, 1, 1);
            waitForRendering(data.item, 400);
        }

        function test_attached_listitem_data() {
            var item = findChild(listView, "listItem3");
            return [
                {tag: "Trailing", item: item, pos: centerOf(item), dx: -units.gu(20), actionList: item.trailingActions, index: 3},
                {tag: "Leading", item: item, pos: centerOf(item), dx: units.gu(20), actionList: item.leadingActions, index: 3},
            ];
        }
        function test_attached_listitem(data) {
            listView.positionViewAtBeginning();
            flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            waitForRendering(data.item, 400);
            compare(data.actionList.ListItemActions.listItem, data.item, "The attached listItem differs from the actual item using the list.");
            compare(data.actionList.ListItemActions.listItemIndex, data.index, "The attached listItem index is wrong.");
            verify(data.actionList.ListItemActions.status != ListItemActions.Disconnected, "The attached status is wrong.");

            // dismiss
            mouseClick(main, 1, 1);
            waitForRendering(data.item, 400);
        }

        function test_rebound_when_pressed_outside_or_clicked_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Click on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Click on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0.contentItem, mouse: true},
                {tag: "Tap on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Tap on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0.contentItem, mouse: false},
            ];
        }
        function test_rebound_when_pressed_outside_or_clicked(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 400);
            verify(data.item.contentItem.x != 0, "The component wasn't tugged!");
            // dismiss
            if (data.mouse) {
                mouseClick(data.clickOn, centerOf(data.clickOn).x, centerOf(data.clickOn).y);
            } else {
                TestExtras.touchClick(0, data.clickOn, centerOf(data.clickOn));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return data.item.contentItem.x; }, 0, 1000);
        }

        function test_listview_not_interactive_while_tugged_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Trailing", view: listView, item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Leading", view: listView, item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: true},
                {tag: "Trailing", view: listView, item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Leading", view: listView, item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: false},
            ];
        }
        function test_listview_not_interactive_while_tugged(data) {
            listView.positionViewAtBeginning();
            interactiveSpy.target = data.view;
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 800);
            // interactive is reset when mouse is released, so we must check its state while tugged
            interactiveSpy.wait();
//            compare(listView.interactive, false, "The ListView is still interactive!");
            // dismiss
            if (data.mouse) {
                mouseClick(data.clickOn, centerOf(data.clickOn).x, centerOf(data.clickOn).y);
            } else {
                TestExtras.touchClick(0, data.clickOn, centerOf(data.clickOn));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return listView.interactive; }, true, 1000);
        }

        function test_selecting_action_rebounds_data() {
            var item0 = findChild(listView, "listItem0");
            return [
                {tag: "With mouse", item: item0, pos: centerOf(item0), dx: units.gu(20), actions: item0.leadingActions, select: "list_option_0", mouse: true},
                {tag: "With touch", item: item0, pos: centerOf(item0), dx: units.gu(20), actions: item0.leadingActions, select: "list_option_0", mouse: false},
            ]
        }
        function test_selecting_action_rebounds(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 800);
            var selectedOption = findChild(panelItem(data.actions), data.select);
            verify(selectedOption, "Cannot select option " + data.select);
            // dismiss
            if (data.mouse) {
                mouseClick(selectedOption, centerOf(selectedOption).x, centerOf(selectedOption).y);
            } else {
                TestExtras.touchClick(0, selectedOption, centerOf(selectedOption));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return data.item.contentItem.x; }, 0, 1000);
        }

        function test_custom_trailing_delegate() {
            listView.positionViewAtBeginning();
            var item = findChild(listView, "listItem0");
            flick(item, centerOf(item).x, centerOf(item).y, -units.gu(20), 0);
            verify(panelItem(trailing), "Panel is not visible");
            var custom = findChild(panelItem(trailing), "custom_delegate");
            verify(custom, "Custom delegate not in use");
            // cleanup
            mouseClick(main, 0, 0);

        }

        // execute as last so we make sure we have the panel created
        function test_snap_data() {
            verify(panelItem(testItem.leadingActions), "Panel had not been created!");
            var action = findChild(panelItem(testItem.leadingActions), "list_option_0");
            verify(action, "Options not accessible");
            var actionSize = action.width;
            return [
                {tag: "Snap back leading, mouse", item: testItem.contentItem, dx: actionSize / 2 - 10, list: testItem.leadingActions, snap: false, mouse: true},
                {tag: "Snap back leading, touch", item: testItem.contentItem, dx: actionSize / 2 - 10, list: testItem.leadingActions, snap: false, mouse: false},
                {tag: "Snap in leading, mouse", item: testItem.contentItem, dx: actionSize / 2 + 10, list: testItem.leadingActions, snap: true, mouse: true},
                {tag: "Snap in leading, touch", item: testItem.contentItem, dx: actionSize / 2 + 10, list: testItem.leadingActions, snap: true, mouse: false},

                {tag: "Snap back trailing, mouse", item: testItem.contentItem, dx: -(actionSize / 2 - 10), list: testItem.trailingActions, snap: false, mouse: true},
                {tag: "Snap back trailing, touch", item: testItem.contentItem, dx: -(actionSize / 2 - 10), list: testItem.trailingActions, snap: false, mouse: false},
                {tag: "Snap in trailing, mouse", item: testItem.contentItem, dx: -(actionSize / 2 + 10), list: testItem.trailingActions, snap: true, mouse: true},
                {tag: "Snap in trailing, touch", item: testItem.contentItem, dx: -(actionSize / 2 + 10), list: testItem.trailingActions, snap: true, mouse: false},
            ];
        }
        function test_snap(data) {
            if (data.mouse) {
                flick(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, centerOf(data.item), Qt.point(data.dx, 0));
            }

            waitForRendering(data.item, 800);
            if (data.snap) {
                verify(data.item.x != 0, "Not snapped to be visible");
            } else {
                verify(data.item.x == 0, "Not snapped back");
            }

            // cleanup
            if (data.mouse) {
                mouseClick(data.item, centerOf(data.item).x, centerOf(data.item).y);
            } else {
                TestExtras.touchClick(0, data.item, centerOf(data.item));
            }
            waitForRendering(data.item, 800);
        }

        function test_verify_action_value_data() {
            return [
                {tag: "Undefined", item: testItem, result: undefined},
                {tag: "Index 0", item: findChild(listView, "listItem0"), result: 0},
                {tag: "Index 1", item: findChild(listView, "listItem1"), result: 1},
                {tag: "Index 2", item: findChild(listView, "listItem2"), result: 2},
                {tag: "Index 3", item: findChild(listView, "listItem3"), result: 3},
            ];
        }
        function test_verify_action_value(data) {
            var option = findChild(panelItem(data.item.leadingActions), "list_option_0");
            verify(option, "actions panel cannot be reached");
            // we test the last action, as we tug the first action on leading, which means teh alst will be accessible
            var len = data.item.leadingActions.actions.length;
            var action = data.item.leadingActions.actions[len - 1];
            actionSpy.target = action;
            actionSpy.clear();
            // tug actions in
            flick(data.item.contentItem, centerOf(data.item.contentItem).x, centerOf(data.item.contentItem).y, option.width, 0);
            waitForRendering(data.item.contentItem, 800);

            // select the option
            mouseClick(data.item, centerOf(option).x, centerOf(option).y);
            waitForRendering(data.item.contentItem, 800);

            // check the action param
            actionSpy.wait();
            compare(action.param, data.result, "Action parameter differs");
        }

        SignalSpy {
            id: panelItemSpy
            signalName: "onXChanged"
        }

        function test_disabled_item_locked_data() {
            var item0 = findChild(listView, "listItem0");
            return [
                // drag same amount as height is
                {tag: "Simple item, leading", item: testItem, enabled: false, dx: testItem.height},
                {tag: "Simple item, trailing", item: testItem, enabled: false, dx: -testItem.height},
                {tag: "ListView item, leading", item: item0, enabled: false, dx: item0.height},
                {tag: "ListView item, trailing", item: item0, enabled: false, dx: -item0.height},
            ];
        }
        function test_disabled_item_locked(data) {
            var oldEnabled = data.item.enabled;
            panelItemSpy.clear();
            panelItemSpy.target = data.item;
            data.item.enabled = false;
            // tug
            flick(data.item.contentItem, centerOf(data.item.contentItem).x, centerOf(data.item.contentItem).y, data.dx, 0);
            compare(panelItemSpy.count, 0, "Item had been tugged despite being disabled!");
            // check opacity
            fuzzyCompare(data.item.opacity, 0.5, 0.1, "Disabled item must be 50% transparent");
            //cleanup
            data.item.enabled = oldEnabled;
        }

        function test_toggle_selectable_data() {
            return [
                {tag: "When not selected", selected: false},
                {tag: "When selected", selected: true},
            ]
        }
        function test_toggle_selectable(data) {
            xChangeSpy.target = testItem.contentItem;
            testItem.selectable = true;
            waitForRendering(testItem.contentItem, 800);
            testItem.selected = data.selected;
            xChangeSpy.wait();
        }

        function test_no_tug_when_selectable() {
            xChangeSpy.target = null;
            testItem.selectable = true;
            // wait till animation to selection mode ends
            waitReboundCompletion(testItem);

            // try to tug leading
            xChangeSpy.target = testItem.contentItem;
            xChangeSpy.clear();
            compare(xChangeSpy.count, 0, "Wrong signal count!");
            flick(testItem.contentItem, centerOf(testItem.contentItem).x, centerOf(testItem.contentItem).y, testItem.contentItem.width / 2, 0);
            compare(xChangeSpy.count, 0, "No tug allowed when in selection mode");
        }

        SignalSpy {
            id: pressAndHoldSpy
            signalName: "pressAndHold"
        }
        SignalSpy {
            id: buttonSpy
            signalName: "clicked"
            target: button
        }
        function test_pressandhold_suppress_click() {
            var center = centerOf(testItem);
            pressAndHoldSpy.target = testItem;
            clickSpy.target = testItem;
            clickSpy.clear();
            mouseLongPress(testItem, center.x, center.y);
            mouseRelease(testItem, center.x, center.y);
            pressAndHoldSpy.wait();
            compare(clickSpy.count, 0, "Click must be suppressed when long pressed");
        }

        function test_click_on_button_suppresses_listitem_click() {
            buttonSpy.target = button;
            clickSpy.target = controlItem;
            mouseClick(button, centerOf(button).x, centerOf(button).y);
            buttonSpy.wait();
            compare(clickSpy.count, 0, "ListItem clicked() must be suppressed");
        }

        function test_ListItemActions_status_data() {
            var drag = testItem.contentItem.width / 2;
            return [
                {tag:"Leading", item: testItem, dx: drag, list: testItem.leadingActions, expectedStatus: ListItemActions.Leading},
                {tag:"Trailing", item: testItem, dx: -drag, list: testItem.trailingActions, expectedStatus: ListItemActions.Trailing},
            ];
        }
        function test_ListItemActions_status(data) {
            var testItem = data.item.contentItem;
            flick(testItem, centerOf(testItem).x, centerOf(testItem).y, data.dx, 0);
            waitForRendering(testItem, 800);
            compare(data.list.ListItemActions.status, data.expectedStatus, "Status on the option list differs.");
            compare(data.list.ListItemActions.listItem, data.item, "connectedItem is not the tugged item.");
        }

        function test_listitem_blockks_ascendant_flickables() {
            var testItem = findChild(nestedListView, "listItem0");
            verify(testItem, "Cannot find test item");
            interactiveSpy.target = testFlickable;
            // tug leading
            flick(testItem, centerOf(testItem).x, centerOf(testItem).y, testItem.width / 2, 0);
            waitForRendering(testItem, 800);
            // check if interactive got changed
            interactiveSpy.wait();
        }

        function test_action_type_set() {
            stockAction.parameterType = Action.None;
            compare(stockAction.parameterType, Action.None, "No parameter type for stockAction!");
            testItem.action = stockAction;
            compare(stockAction.parameterType, Action.Integer, "No parameter type for stockAction!");
        }

        function test_action_triggered_on_clicked() {
            testItem.action = stockAction;
            actionSpy.target = stockAction;
            clickSpy.target = testItem;
            mouseClick(testItem, centerOf(testItem).x, centerOf(testItem).y);
            clickSpy.wait();
            actionSpy.wait();
        }

        function test_action_suppressed_on_longpress() {
            testItem.action = stockAction;
            actionSpy.target = stockAction;
            clickSpy.target = testItem;
            pressAndHoldSpy.target = testItem;
            mouseLongPress(testItem, centerOf(testItem).x, centerOf(testItem).y);
            mouseRelease(testItem, centerOf(testItem).x, centerOf(testItem).y);
            pressAndHoldSpy.wait();
            compare(clickSpy.count, 0, "Click must be suppressed.");
            compare(actionSpy.count, 0, "Action triggered must be suppressed");
        }

        // keep these as last ones so we make sure the panel has been created by the previous swipes
        function test_x_backgroundColor_change() {
            // change panel color for the leading and observe the trailing panelItem color change
            leading.backgroundColor = UbuntuColors.blue;
            compare(findChild(panelItem(leading), "panel_background").color, UbuntuColors.blue, "leading panelItem color differs");
            compare(findChild(panelItem(trailing), "panel_background").color, UbuntuColors.blue, "trailing panelItem color has not been set");
        }
        function test_x_foregroundColor_change() {
            // change panel color for the leading and observe the trailing panelItem color change
            leading.foregroundColor = UbuntuColors.green;
            compare(findChild(panelItem(leading), "action_icon").color, UbuntuColors.green, "leading panelItem color differs");
        }
    }
}