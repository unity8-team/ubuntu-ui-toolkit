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
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: root

    property alias text: label.text
    property alias textColor: label.color
    property alias iconSource: icon.source
    property real iconSize
    property real spacing
    property alias font: label.font
    property int iconInsertion
    property bool hasIcon: iconSource != ""
    property bool hasText: text != ""
    property bool hasBoth: hasIcon && hasText

    state: hasBoth && iconInsertion === Button.After? "right" : "left"
    implicitWidth: (
        (hasText? label.width : 0) +
        (hasBoth? spacing : 0) +
        (hasIcon? icon.width : 0)
    )
    implicitHeight: Math.max(icon.height, label.height)

    Icon {
        id: icon
        visible: hasIcon
        anchors.verticalCenter: parent.verticalCenter
        width: iconSize
        height: iconSize
        color: textColor
    }

    Label {
        id: label
        visible: hasText
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
    }

    states: [
        State {
            name: "left"
            AnchorChanges {
                target: icon
                anchors.left: root.left
            }
            AnchorChanges {
                target: label
                anchors.left: hasIcon? icon.right : root.left
            }
            PropertyChanges {
                target: label
                anchors.leftMargin: hasIcon? units.gu(1) : 0
            }
        },
        State {
            name: "right"
            AnchorChanges {
                target: icon
                anchors.right: root.right
            }
            AnchorChanges {
                target: label
                anchors.right: icon.left
            }
            PropertyChanges {
                target: label
                anchors.rightMargin: units.gu(1)
            }
        }
    ]
}
