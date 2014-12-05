/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 0.1

TemplateSection {
    id: section
    property alias delegate: repeater.delegate
    property alias count: repeater.count
    property alias model: repeater.model
    property bool newListItem: false
    documentation: (newListItem) ?
                       "qml-ubuntu-components-listitems-%1.html".arg(className.toLowerCase()) :
                       "qml-ubuntu-components-%1.html".arg(className.toLowerCase())

    function itemAt(index)
    {
       return repeater.itemAt(index)
    }
    function deleteAt(index)
    {
        if (repeater.count >= 1 && repeater.model.hasOwnProperty("remove")) {
            repeater.model.remove(index);
        }
    }

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            id: repeater
            model: 4
        }
    }
}
