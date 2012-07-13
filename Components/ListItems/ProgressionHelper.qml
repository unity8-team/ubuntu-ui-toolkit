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

import QtQuick 1.1
import ".."

/*!
    \qmlclass ProgressionHelper
    \inqmlmodule UbuntuUIToolkit
    \brief The parent of all list items

    \b{This component is under heavy development.}

*/

Item {
    id: progressItem
    height: parent.height
    width: visible ? height : 0// square
//    anchors {
//        right: parent.right
//        top: parent.top
//        bottom: parent.bottom
//    }

    visible: false

    property alias split: progressionDivider.visible

    Image {
        id: progressIcon
        source: "artwork/arrow_Progression.png"
        anchors.centerIn: parent
        visible: listItem.progression
    }

    Image {
        id: progressionDivider
        visible: false
        anchors {
            top: parent.top
            bottom: parent.bottom
            //bottomMargin: listItem.__bottomSeparatorHeight
            //topMargin: listItem.__topSeparatorHeight
        }
        width: 1
        source: "artwork/divider_Vertical.png"
    }
}
