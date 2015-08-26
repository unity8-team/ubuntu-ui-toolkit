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

Popover {
    id: contextmenu
    objectName: 'ContextMenu'
    contentHeight: Math.max(column.childrenRect.height, units.gu(4))
    contentWidth: Math.max(column.childrenRect.width, units.gu(4))

    default property alias children: column.children

    // FIXME: styleName: "ContextMenuForegroundStyle"
    Column {
        id: column
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
    }

    // Hide the arrow
    Binding {
        target: __foreground
        property: "direction"
        value: "none"
    }
}
