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
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    width: 200; height: 200

    property bool hasOSK: QuickUtils.inputMethodProvider !== ""

    TextArea {
        id: textArea
        SignalSpy {
            id: signalSpy
            target: parent
        }

        property int keyPressData
        property int keyReleaseData
        Keys.onPressed: keyPressData = event.key
        Keys.onReleased: keyReleaseData = event.key
    }

    TextArea {
        id: colorTest
        color: colorTest.text.length < 4 ? "#0000ff" : "#00ff00"
    }

    ListItem.Empty {
        id: listItem
        height: 200
        anchors.left: parent.left

        anchors.right: parent.right
        SignalSpy {
            id: listItemSpy
            signalName: "clicked"
            target: listItem
        }

        TextArea {
            id: input
            anchors.fill: parent
            Component.onCompleted: forceActiveFocus()
        }
    }

    Item {
        TextArea {
            id: t1
            objectName: "t1"
        }
        TextArea {
            id: t2
            objectName: "t2"
        }
    }

    TestCase {
        name: "TextAreaAPI"
        when: windowShown

        function test_1_activate() {
            textArea.forceActiveFocus();
            compare(textArea.activeFocus, true, "TextArea is active");
        }

        // TextEdit shared properties
        function test_0_cursorDelegate() {
            compare((textArea.cursorDelegate!=null),true,"TextArea.cursorDelegate is not null")
        }

        function test_zz_length() {
            textArea.text = "a\nb\tc d"
            compare(textArea.length, 7, "TextArea.length is 7 plain text characters")
            textArea.textFormat = TextEdit.RichText
            textArea.text = "<p>a\n<b>b</b>\tc d</p>"
            compare(textArea.length, 7, "TextArea.length is 7 characters despite HTML markup")
        }

        function test_zz_lineCount() {
            console.log('ref err here')
            textArea.text = "first"
            compare(textArea.lineCount, 1, "1 line as expected")
            // autosize reacts on lineCount
            textArea.autoSize = true
         /* FIXME
            textArea.text = "first\nsecond"
            compare(textArea.lineCount, 2, "2 lines as expected")
          */
            // the current height is that of 2 lines
            var heightOfTwoLines = textArea.height
            textArea.maximumLineCount = 2
            textArea.text = "first\nsecond\nthird"
         /* FIXME
            compare(textArea.lineCount, 3, "3 lines as expected")
          */
            compare(textArea.height, heightOfTwoLines, 'still sized within maximum of 2')
        }

        function test_1_mouseSelectionMode() {
            compare(textArea.mouseSelectionMode, TextEdit.SelectWords,"TextArea.mouseSelectionMode is SelectWords")
        }

        function test_0_selectByMouse() {
            compare(textArea.selectByMouse,true,"TextArea.selectByMouse is true")
        }

        function test_0_textFormat() {
            compare(textArea.textFormat, TextEdit.PlainText,"TextArea.textFormat is same as TextEdit.textFormat")
        }

        function test_0_wrapMode() {
            compare(textArea.wrapMode,TextEdit.Wrap,"TextArea.wrapMode is TextEdit.Wrap")
        }

    // TextArea specific properties
        function test_1_highlighted() {
            compare(textArea.highlighted, textArea.focus, "highlighted is the same as focused");
        }

        function test_1_contentHeight() {
            compare(textArea.contentHeight>0,true,"contentHeight over 0 units on default")
            var newValue = 200;
            textArea.contentHeight = newValue;
            compare(textArea.contentHeight,newValue,"set/get");
        }

        function test_1_contentWidth() {
            compare(textArea.contentWidth,units.gu(30),"contentWidth is 30 units on default")
            var newValue = 200;
            textArea.contentWidth = newValue;
            compare(textArea.contentWidth,newValue,"set/get");
        }

        function test_1_placeholderText() {
            compare(textArea.placeholderText,"","placeholderText is '' on default")
            var newValue = "Hello Placeholder";
            textArea.placeholderText = newValue;
            compare(textArea.placeholderText,newValue,"set/get");
        }

        function test_1_autoSize() {
            compare(textArea.autoSize,false,"TextArea.autoSize is set to false");
            var newValue = true;
            textArea.autoSize = newValue;
            compare(textArea.autoSize, newValue,"set/get");
        }

        function test_1_baseUrl() {
            expectFail("","TODO")
            compare(textArea.baseUrl,"tst_textarea.qml","baseUrl is QML file instantiating the TextArea item on default")
        }

        function test_1_displayText() {
            compare(textArea.displayText,"","displayText is '' on default")
            var newValue = "Hello Display Text";
            try {
                textArea.displayText = newValue;
                compare(false,true,"Unable to set value as it is read-only");
            } catch (e) {
                compare(true,true,"Unable to set value as it is read-only");
            }

        }

        function test_1_popover() {
            compare(textArea.popover, undefined, "Uses default popover");
        }

        function test_1_maximumLineCount() {
            compare(textArea.maximumLineCount,5,"maximumLineCount is 0 on default")
            var newValue = 10;
            textArea.maximumLineCount = newValue;
            compare(textArea.maximumLineCount,newValue,"set/get");
        }

        function test_0_visible() {
            textArea.visible = false;
            compare(textArea.activeFocus, false, "TextArea is inactive");
        }

    // functions
        function test_copy() {
            textArea.copy();
        }

        function test_cut() {
            Clipboard.clear();
            textArea.readOnly = false;
            textArea.text = "test text";
            textArea.cursorPosition = textArea.text.indexOf("text");
            textArea.selectWord();
            textArea.cut();
            compare(textArea.text, "test ", "Text cut properly");
            compare(Clipboard.data.text, "text", "Clipboard contains cut text");
            // we should have the "text" only ones
            var plainTextCount = 0;
            for (var i in Clipboard.data.formats) {
                if (Clipboard.data.formats[i] === "text/plain")
                    plainTextCount++;
            }
            compare(plainTextCount, 1, "Clipboard is correct");
        }

        function test_deselect() {
            textArea.deselect();
        }

        function test_getFormattedText() {
            textArea.getFormattedText(0,0);
        }

        function test_getText() {
            textArea.getText(0,0);
        }

        function test_insert() {
            textArea.insert(0,"Hello");
        }

        function test_isRightToLeft() {
            textArea.isRightToLeft(0,0);
        }

        function test_moveCursorSelection() {
            textArea.moveCursorSelection(0,0);
        }

        function test_paste() {
            textArea.readOnly = false;
            textArea.text = "test";
            textArea.cursorPosition = textArea.text.length;
            textArea.paste(" text");
            compare(textArea.text, "test text", "Data pasted");
        }

        function test_positionAt() {
            textArea.positionAt(0,1);
        }

        function test_positionToRectangle() {
            textArea.positionToRectangle(0);
        }

        function test_redo() {
            textArea.redo();
        }

        function test_remove() {
            textArea.remove(0,0);
        }

        function test_select() {
            textArea.select(0,0);
        }

        function test_selectAll() {
            textArea.selectAll();
        }

        function test_selectWord() {
            textArea.selectWord();
        }

        function test_undo() {
            textArea.undo();
        }


    // signals
        function test_linkActivated() {
            signalSpy.signalName = "linkActivated";
            compare(signalSpy.valid,true,"linkActivated signal exists")
        }

    // filters
        function test_keyPressAndReleaseFilter() {
            textArea.visible = true;
            textArea.forceActiveFocus();
            textArea.readOnly = false;
            textArea.keyPressData = 0;
            textArea.keyReleaseData = 0;
            keyClick(Qt.Key_T, Qt.NoModifier, 100);
            compare(textArea.keyPressData, Qt.Key_T, "Key press filtered");
            compare(textArea.keyReleaseData, Qt.Key_T, "Key release filtered");
        }

        function test_TextAreaInListItem_EnterCaptured() {
            input.forceActiveFocus();
            input.textFormat = TextEdit.PlainText;
            input.text = "";
            keyClick(Qt.Key_T);
            keyClick(Qt.Key_E);
            keyClick(Qt.Key_S);
            keyClick(Qt.Key_T);
            keyClick(Qt.Key_Enter);
            compare(input.text, "test\n", "Keys");
        }
        function test_TextAreaInListItem_EnterDoesNotProduceClick() {
            input.forceActiveFocus();
            input.textFormat = TextEdit.PlainText;
            input.text = "";
            listItemSpy.clear();
            keyClick(Qt.Key_Enter);
            tryCompare(listItemSpy, "count", 0, 100);
        }

        function test_colorCollisionOnDelegate() {
            // fixes bug lp:1169601
            colorTest.text = "abc";
            compare(colorTest.color, "#0000ff", "Color when text length < 4");
            colorTest.text = "abcd";
            compare(colorTest.color, "#00ff00", "Color when text length >= 4");
        }

        function test_OneActiveFocus() {
            t1.focus = true;
            compare(t1.activeFocus, true, "T1 has activeFocus");
            compare(t2.activeFocus, false, "T1 has activeFocus");
            t2.focus = true;
            compare(t1.activeFocus, false, "T1 has activeFocus");
            compare(t2.activeFocus, true, "T1 has activeFocus");
        }

        function test_OSK_ShownWhenNextTextAreaIsFocused() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the first TextArea");
            t2.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown for the second TextArea");
        }

        function test_RemoveOSKWhenFocusLost() {
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            t1.focus = true;
            compare(Qt.inputMethod.visible, true, "OSK is shown when TextArea gains focus");
            t1.focus = false;
            compare(Qt.inputMethod.visible, false, "OSK is hidden when TextArea looses focus");
        }

        function test_ReEnabledInput() {
            textArea.forceActiveFocus();
            textArea.enabled = false;
            compare(textArea.enabled, false, "textArea is disabled");
            compare(textArea.focus, true, "textArea is focused");
            compare(textArea.activeFocus, false, "textArea is not active focus");
            compare(Qt.inputMethod.visible, false, "OSK removed");

            textArea.enabled = true;
            compare(textArea.enabled, true, "textArea is enabled");
            compare(textArea.focus, true, "textArea is focused");
            compare(textArea.activeFocus, true, "textArea is active focus");
            if (!hasOSK)
                expectFail("", "OSK can be tested only when present");
            compare(Qt.inputMethod.visible, true, "OSK shown");
        }

        // make it to b ethe last test case executed
        function test_zz_TextareaInListItem_RichTextEnterCaptured() {
            textArea.text = "a<br />b";
            textArea.textFormat = TextEdit.RichText;
            input.forceActiveFocus();
            input.textFormat = TextEdit.RichText;
            input.text = "ab";
            input.cursorPosition = 1;
            keyClick(Qt.Key_Return);
            compare(input.text, textArea.text, "Formatted text split");
        }
    }
}
