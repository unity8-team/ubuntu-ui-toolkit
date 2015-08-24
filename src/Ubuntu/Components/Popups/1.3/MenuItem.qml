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
import Ubuntu.Components 1.3

ListItem {
    contentItem.anchors {
         leftMargin: units.gu(2)
         rightMargin: units.gu(2)
    }
    width: units.gu(20) // FIXME: label.implicitWidth
    height: units.gu(4)

    color: '#ffffff'
    opacity: selected || hovered ? 0.08 : 1.0
    Mouse.onEntered: hovered = true
    Mouse.onExited: hovered = false
    property bool hovered
    enabled: action.enabled

    Row {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: shortcut.left
        spacing: units.gu(1)

        Icon {
            name: action.iconName
            width: units.gu(2)
            height: width
            opacity: label.opacity
        }

        Label {
            id: label
            text: action.text
            color: '#333333'
            opacity: action.enabled ? 1.0 : 0.3
        }
    }

    Label {
        id: shortcut
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        text: "^S" // action.shortcut
        color: label.color
        opacity: label.opacity
    }
}
