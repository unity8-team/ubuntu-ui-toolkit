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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Rectangle {
    id: menu
    border.color: UbuntuColors.lightGrey
    border.width: 1
    radius: units.gu(1)
    width: units.gu(10)
    height: units.gu(10)
    property alias model: list.model

    signal aboutToClose()

    InverseMouseArea {
        anchors.fill: parent
        topmostItem: true
        sensingArea: menu.parent
        acceptedButtons: Qt.LeftButton
        onClicked: menu.aboutToClose()
    }

    ListView {
        id: list
        anchors.fill: parent
        model: 10
        clip: true
        delegate: Label {
            text: modelData
        }
    }
}

