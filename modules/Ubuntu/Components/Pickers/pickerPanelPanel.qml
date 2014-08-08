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

import QtQuick 2.2
import QtQuick.Window 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0

Rectangle {
    property alias picker: picker
    property alias date: picker.date
    property alias pickerMode: picker.mode
    property string callerProperty
    property Item caller

    signal closed()

    id: panel
    // no additional styling is needed
    color: Theme.palette.normal.overlay
    width: parent.width
    height: Qt.inputMethod.keyboardRectangle.height > 0 ? Qt.inputMethod.keyboardRectangle.height : units.gu(26)
    y: parent.height

    ThinDivider { anchors.bottom: parent.top }
    DatePicker {
        id: picker
        anchors {
            fill: panel
            margins: units.gu(2)
        }
        Binding {
            target: caller
            property: callerProperty
            when: callerProperty != undefined
            value: picker.date
        }
    }

    InverseMouseArea {
        anchors.fill: parent
        onPressed: panel.state = ''
    }

    Component.onCompleted: state = 'opened'

    states: [
        State {
            name: 'opened'
            PropertyChanges {
                target: panel
                y: parent.height - height
            }
        }
    ]
    transitions: [
        Transition {
            from: ''
            to: 'opened'
            UbuntuNumberAnimation {
                target: panel
                property: 'y'
            }
        },
        Transition {
            from: 'opened'
            to: ''
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: panel
                    property: 'y'
                }
                ScriptAction {
                    script: {
                        closed();
                        panel.destroy();
                    }
                }
            }
        }
    ]
}
