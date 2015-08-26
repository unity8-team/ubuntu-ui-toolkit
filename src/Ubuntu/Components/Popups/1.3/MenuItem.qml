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
    id: menuitem
    contentItem.anchors {
         leftMargin: units.gu(2)
         rightMargin: units.gu(2)
    }
    width: row.spacing + units.gu(25) + hotkey.width + units.gu(4)
    height: units.gu(4)

    color: '#ffffff'
    opacity: selected || hovered ? 0.08 : 1.0
    Mouse.onEntered: hovered = true
    Mouse.onExited: hovered = false
    property bool hovered
    property string text
    enabled: action ? action.enabled : true

    Row {
        id: row
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: shortcut.left
        spacing: units.gu(1)

        Icon {
            id: icon
            name: action ? action.iconName : ''
            width: visible ? units.gu(2) : 0
            height: width
            opacity: label.opacity
            visible: name != ''
        }

        Label {
            id: label
            text: action ? action.text : menuitem.text
            color: '#333333'
            opacity: menuitem.enabled ? 1.0 : 0.3
            elide: Text.ElideRight
            width: units.gu(25) - icon.width
        }
    }

    Row {
        id: shortcut
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        opacity: label.opacity

        Label {
            id: hotkey
            property string shortcut: action && action.shortcut ? action.shortcut : ''
            text: hotkey.shortcut.replace('Ctrl+', '^').replace('Shift+', '⇧')
            color: label.color
            visible: shortcut != ''
            width: units.gu(5)
        }

        Icon {
            id: chevron
            name: 'chevron'
            width: units.gu(2)
            height: width
            visible: !action
        }
    }
}
