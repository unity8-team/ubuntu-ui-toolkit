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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0

Popover {
    property alias picker: picker
    property alias date: picker.date
    property alias pickerMode: picker.mode
    property string callerProperty

    signal closed()

    contentWidth: frame.width
    contentHeight: frame.height

    Item {
        id: frame
        width: picker.width + units.gu(4)
        height: picker.height + units.gu(4)
        DatePicker {
            id: picker
            anchors.centerIn: parent
            Binding {
                target: caller
                property: callerProperty
                when: callerProperty != undefined
                value: picker.date
            }
        }
    }
    Component.onDestruction: closed()
}
