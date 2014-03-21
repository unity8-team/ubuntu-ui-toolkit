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
import Ubuntu.Components 0.1

Item {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    MouseArea {
        objectName: "MouseArea"
        anchors.fill: parent
        onClicked: color.color = "green"
    }

    Rectangle {
        id: color
        anchors.fill: parent
        anchors.margins: units.gu(10)
        color: "blue"
        InverseMouseArea {
            anchors.fill: parent
            onClicked: color.color = "red"
        }
    }
}
