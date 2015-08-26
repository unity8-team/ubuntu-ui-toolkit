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

Column {
    id: group
    property ContextMenu menu
    property list<QtObject> actions
    default property alias children: group.actions
    property Component delegate: MenuItem {
        action: modelData.hasOwnProperty('shortcut') ? modelData : null
        text: modelData.text
        enabled: modelData.enabled
        onClicked: contextmenu.hide() // FIXME
        divider.visible: action == actions[group.actions.length - 1]
    }
    property string text

    Loader {
        property var modelData: Action {
            text: group.text
        }
        active: group.text !== ''
        sourceComponent: group.delegate
    }

    Repeater {
        model: actions
        delegate: group.delegate
    }
}
