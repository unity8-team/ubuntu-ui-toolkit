/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: 400
    height: 600

    Action {
        id: action
    }
    Action {
        id: other
        shortcut: 'Ctrl+G'
    }

    // need a manager to activate global context
    ActionManager{}
    // shortcuts need an active action to trigger
    ActionContext{
        id: context
        active: true
        actions: [action, other]
    }

    UbuntuTestCase {
        id: testCase
        name: "Shortcuts"
        when: windowShown

        function initTestCase() {
        }

        function init() {
        }

        SignalSpy {
            id: spy
            signalName: 'triggered'
            target: action
        }

        function test_shortcut_triggered_data() {
            return [
                { tag: 'Multiple modifiers and letter', shortcut: 'Ctrl+Shift+Alt+A', key: Qt.Key_A, mod: Qt.ControlModifier + Qt.ShiftModifier + Qt.AltModifier, active: true, xfail: false },
                { tag: 'Multiple modifiers and letter, xfail', shortcut: 'Ctrl+Shift+Alt+A', key: Qt.Key_A, mod: Qt.ControlModifier + Qt.ShiftModifier + Qt.AltModifier, active: false, xfail: true },
                { tag: 'Modifier and letter', shortcut: 'Ctrl+A', key: Qt.Key_A, mod: Qt.ControlModifier, active: true, xfail: false },
                { tag: 'Modifier and letter, xfail', shortcut: 'Ctrl+A', key: Qt.Key_A, mod: Qt.ControlModifier, active: false, xfail: true },
                { tag: 'Single letter', shortcut: 'E', key: Qt.Key_E, mod: Qt.NoModifier, active: true, xfail: false },
                { tag: 'Single letter, xfail', shortcut: 'E', key: Qt.Key_E, mod: Qt.NoModifier, active: false, xfail: true },
                { tag: 'StandardKey', shortcut: StandardKey.Copy, key: Qt.Key_C, mod: Qt.ControlModifier, active: true, xfail: false },
                { tag: 'StandardKey, xfail', shortcut: StandardKey.Copy, key: Qt.Key_C, mod: Qt.ControlModifier, active: false, xfail: true }
            ];
        }
        function test_shortcut_triggered(data) {
            context.active = data.active;
            action.shortcut = data.shortcut;
            spy.clear();
            keyClick(data.key, data.mod);
            if (data.xfail) {
                expectFailContinue(data.tag, "Signal should not be triggered");
            }
            spy.wait(400);
        }

        function test_shortcut_invalid_data() {
            return [
                { tag: 'Typo', shortcut: 'Ctr+F' },
                { tag: 'Number', shortcut: 1234567890 }
            ];
        }
        function test_shortcut_invalid(data) {
            ignoreWarning(warningFormat(27, 5, 'QML Action: Invalid shortcut: '));
            action.shortcut = data;
        }

        function test_shortcut_duplicate() {
            ignoreWarning(warningFormat(30, 5, 'QML Action: Ambiguous shortcut: Ctrl+G'));
            action.shortcut = other.shortcut;
            keyClick(Qt.Key_G, Qt.ControlModifier);
        }
    }
}
