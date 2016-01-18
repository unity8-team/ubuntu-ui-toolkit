/*
 * Copyright 2014-2016 Canonical Ltd.
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
import Ubuntu.Components 1.3

Item {
    id: testMain
    width: units.gu(40)
    height: units.gu(71)

    Column {
        spacing: units.gu(1)
        Label {
            text: "Text fields are awesome"
            width: parent.width
            height: units.gu(10)
            verticalAlignment: Text.AlignVCenter
        }
        TextField {
            id: textField
            text: "This is a single line text input called TextField."
        }
        Label {
            text: "Text areas are even more amazing"
            width: parent.width
            height: units.gu(5)
            verticalAlignment: Text.AlignVCenter
        }
        TextArea {
            id: textArea
            text: "This is a multiline text input component called TextArea. It supports fix size as well as auto-expanding behavior. The content is scrollable only if it exceeds the visible area."
        }
        Flickable {
            id: outerFlicker
            width: parent.width
            height: units.gu(20)
            clip: true
            contentWidth: autoSizeTextArea.width
            contentHeight: autoSizeTextArea.height
            TextArea {
                id: autoSizeTextArea
                autoSize: true
                maximumLineCount: 0
                text: "1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1"
            }
        }
        TextField {
            id: emptyTextField
            text: ""
        }
        TextArea {
            id: emptyTextArea
            text: ""
        }
    }

    UbuntuTestCase {
        name: "TextInputTouchTests"
        when: windowShown

        SignalSpy {
            id: popupSpy
            signalName: "pressAndHold"
        }
        SignalSpy {
            id: flickerSpy
            signalName: "movementEnded"
        }

        function guPoint(x, y) {
            return Qt.point(units.gu(x), units.gu(y));
        }

        function findCaret(input) {
            var cursor = findChild(input, "textCursor");
            if (cursor.caret)
                return cursor.caret;
            return null;
        }

        function allTheTextComponents() {
            return [
                {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
                {tag: "Empty TextField", input: emptyTextField},
                {tag: "Empty TextArea", input: emptyTextArea},
            ];
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
        }

        function init() {
            textField.cursorPosition = 0;
            textArea.cursorPosition = 0;
            emptyTextField.cursorPosition = 0;
            emptyTextArea.cursorPosition = 0;
        }
        function cleanup() {
            textField.cursorPosition = 1;
            textArea.cursorPosition = 1;
            textField.focus = false;
            textArea.focus = false;
            emptyTextField.focus = false;
            emptyTextArea.focus = false;
            autoSizeTextArea.focus = false;
            popupSpy.target = null;
            popupSpy.clear();
            flickerSpy.target = null;
            flickerSpy.clear();
        }

        function test_no_caret_on_focus_data() {
            return allTheTextComponents();
        }
        function test_no_caret_on_focus(data) {
            data.input.focus = true;
            waitForRendering(data.input);
            var cursor = findChild(data.input, "textCursor");
            verify(cursor, "Cursor not accessible");
            compare(cursor.caret.visible, false, "Caret visible after focus!");
        }


        function test_no_caret_on_click_data() {
            return allTheTextComponents();
        }
        function test_no_caret_on_click(data) {
            mouseClick(data.input, centerOf(data.input));
            waitForRendering(data.input);
            // FIXME: verify(data.input.activeFocus, "No focus after mouse click!");
            var positionCaret = findCaret(data.input);
            verify(positionCaret, "No caret is set");
            compare(positionCaret.visible, false, "Caret visible after mouse click!");
        }

        function test_caret_on_touch_data() {
            return [
                {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
            ];
        }
        function test_caret_on_touch(data) {
            TestExtras.touchPress(0, data.input, centerOf(data.input));
            compare(data.input.focus, false, "Focus before releasing finger!");
            waitForRendering(data.input);
            TestExtras.touchRelease(0, data.input, centerOf(data.input));
            waitForRendering(data.input);
            verify(data.input.activeFocus, "No focus after tap!");
            var positionCaret = findCaret(data.input);
            verify(positionCaret, "No caret is set");
            verify(positionCaret.visible, "Caret not visible after tap!");
        }

        function test_select_text_on_doubletap_data() {
            return [
                // FIXME: {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
            ];
        }
        function test_select_text_on_doubletap(data) {
            data.input.focus = true;
            TestExtras.touchDoubleClick(0, data.input, guPoint(1, 1));
            waitForRendering(data.input);
            verify(data.input.selectedText !== "", "No text selected!");
        }

        function test_longtap_when_inactive_has_no_effect_data() {
            return allTheTextComponents();
        }
        function test_longtap_when_inactive_has_no_effect(data) {
            TestExtras.touchLongPress(0, data.input, guPoint(1, 1));
            waitForRendering(data.input);
            verify(!data.input.focus, "Text input must not get focused");
            verify(data.input.selectedText === "", "There shouldn't be any text selected");
            // cleanup
            TestExtras.touchRelease(0, data.input, guPoint(1, 1));
        }

        function test_select_text_longtap_when_active_data() {
            return [
                // FIXME: {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
            ];
        }
        function test_select_text_longtap_when_active(data) {
            if(TestExtras.openGLflavor() == "opengles2" &&
               TestExtras.cpuArchitecture() != "arm")
                skip("This test doesn't pass with OpenGLES other than arm");
            TestExtras.touchClick(0, data.input, centerOf(data.input));
            waitForRendering(data.input);
            verify(data.input.activeFocus, "No focus after tap!");
            var positionCaret = findCaret(data.input);
            verify(positionCaret, "No caret is set");
            verify(positionCaret.visible, "Caret not visible after tap!");

            popupSpy.target = findChild(data.input, "input_handler");
            TestExtras.touchLongPress(0, data.input, guPoint(1, 1));
            waitForRendering(data.input);
            popupSpy.wait();
            verify(data.input.selectedText !== "", "There should be text selected!");

            // cleanup
            TestExtras.touchRelease(0, data.input, guPoint(1, 1));
            // dismiss popover
            TestExtras.touchClick(0, testMain, 0, 0);
        }

        function test_longtap_when_empty_data() {
            return [
                {tag: "TextField", input: emptyTextField},
                {tag: "TextArea", input: emptyTextArea},
            ];
        }
        function test_longtap_when_empty(data) {
            TestExtras.touchClick(0, data.input, centerOf(data.input));
            wait(500);
            verify(data.input.activeFocus, "No focus after tap!");

            popupSpy.target = findChild(data.input, "input_handler");
            TestExtras.touchLongPress(0, data.input, guPoint(1, 1));
            waitForRendering(data.input);
            popupSpy.wait();
            compare(popupSpy.count, 1, "Copy/paste popup should be displayed.");

            // cleanup
            TestExtras.touchRelease(0, data.input, guPoint(1, 1));
            // dismiss popover
            TestExtras.touchClick(0, testMain, 0, 0);
            waitForRendering(data.input);
        }

        function test_long_tap_on_selected_text_data() {
            return [
                // FIXME: {tag: "TextField", input: textField},
                {tag: "TextArea", input: textArea},
            ];
        }
        function test_long_tap_on_selected_text(data) {
            data.input.focus = true;
            data.input.selectWord();
            var selectedText = data.input.selectedText;
            verify(selectedText !== "", "No text selected!");

            popupSpy.target = findChild(data.input, "input_handler");
            TestExtras.touchLongPress(0, data.input, guPoint(4, 2));
            waitForRendering(data.input);
            popupSpy.wait();
            compare(data.input.selectedText, selectedText, "Text selection should be the same!");

            // cleanup
            TestExtras.touchRelease(0, data.input, guPoint(2, 2));
            // dismiss popover
            TestExtras.touchClick(0, testMain, guPoint(0, 0));
        }

        function test_drag_cursor_handler_data() {
            return [
                {tag: "TextField", input: textField, delta: guPoint(10, 0)},
                {tag: "TextArea", input: textArea, delta: guPoint(10, 4)},
            ];
        }
        function test_drag_cursor_handler(data) {
            data.input.focus = true;
            data.input.cursorPosition = 0;
            var positionCaret = findCaret(data.input);
            verify(positionCaret, "No caret is set");
            var cursorPosition = data.input.cursorPosition;
            compare(positionCaret.visible, false, "Caret visible after focus!");

            TestExtras.touchClick(0, data.input, centerOf(data.input));
            waitForRendering(data.input, 500);
            verify(positionCaret.visible, "Caret not visible after tap!");

            TestExtras.touchDrag(0, positionCaret, centerOf(positionCaret), data.delta);
            waitForRendering(data.input, 500);
            verify(cursorPosition !== data.input.cursorPosition, "Cursor not moved!");
        }

        function test_select_text_by_dragging_cursor_handler_data() {
            return [
                {tag: "TextField", input: textField, initialCursorPosition: 0, cursorName: "selectionEnd", delta: guPoint(10, 0)},
                {tag: "TextArea", input: textArea, initialCursorPosition: 0, cursorName: "selectionEnd", delta: guPoint(10, 5)},
                // FIXME: {tag: "TextField(end)", input: textField, initialCursorPosition: 48, cursorName: "selectionStart", delta: guPoint(-10, 0)},
                // FIXME: {tag: "TextArea(end)", input: textArea, initialCursorPosition: 50, cursorName: "selectionStart", delta: guPoint(-20, -5)},
            ];
        }
        function test_select_text_by_dragging_cursor_handler(data) {
            TestExtras.touchClick(0, data.input, centerOf(data.input));
            waitForRendering(data.input);
            verify(data.input.activeFocus, "No focus after tap!");
            var positionCaret = findCaret(data.input);
            verify(positionCaret, "No caret is set");
            verify(positionCaret.visible, "Caret not visible after tap!");

            data.input.cursorPosition = data.initialCursorPosition;
            data.input.selectWord();
            verify(data.input.selectedText !== "", "No word selected initially!");
            var selectedText = data.input.selectedText;

            var caret = findChild(data.input, "input_handler")[data.cursorName + "Cursor"];
            verify(caret, "Caret \"" + data.cursorName + "\" cannot be found!");
            verify(caret.visible, "Caret not visible after tap!");

            TestExtras.touchDrag(0, caret, centerOf(caret), data.delta);
            verify(data.input.selectedText !== "", "Selection cleared!");
            // FIXME: verify(data.input.selectedText != selectedText, "Selection did not change");
        }

        function test_z_scroll_when_tap_dragged_data() {
            return [
                {tag: "TextField", input: textField, withSelectedText: false, from: guPoint(2, 2), delta: guPoint(10, 0)},
                {tag: "TextArea", input: textArea, withSelectedText: false, from: guPoint(2, 2), delta: guPoint(10, 4)},
                {tag: "TextField(selected)", input: textField, withSelectedText: true, from: guPoint(2, 2), delta: guPoint(10, 0)},
                {tag: "TextArea(selected)", input: textArea, withSelectedText: true, from: guPoint(2, 2), delta: guPoint(10, 4)},
            ];
        }
        function test_z_scroll_when_tap_dragged(data) {
            data.input.focus = true;
            data.input.cursorPosition = data.input.text.length;
            flickerSpy.target = findChild(data.input, "input_scroller");
            var selectedText = "";
            if (data.withSelectedText) {
                data.input.selectWord();
                selectedText = data.input.selectedText;
            }
            waitForRendering(data.input, 200);
            TestExtras.touchDrag(0, data.input, data.from, data.delta);
            waitForRendering(data.input, 200);
            flickerSpy.wait();
            compare(selectedText, data.input.selectedText, "Text selection differs!");
        }

        function test_0_drag_autosizing_textarea_drags_parent_flickable_data() {
            return [
                {tag: "when inactive", focused: false },
            ];
        }
        function test_0_drag_autosizing_textarea_drags_parent_flickable(data) {
            flickerSpy.target = outerFlicker;
            autoSizeTextArea.focus = data.focused;
            var editor = findChild(autoSizeTextArea, "text_input");
            TestExtras.touchDrag(0, editor, guPoint(0, 0), guPoint(0, 40));
            flickerSpy.wait();
        }
    }
}
