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

Item {
    objectName: "root"
    width: 100; height: 100

    Rectangle {
        id: hole
        width: 50; height: 50
        color: "blue"
        objectName: "hole"
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        objectName: "MA"
    }

    MouseFilter {
        objectName: "filter"
        anchors.fill: hole
        onPositionChanged: mouse.accepted = true
    }

    InverseMouseArea {
        objectName: "IMA"
        anchors.fill: hole
    }
}

