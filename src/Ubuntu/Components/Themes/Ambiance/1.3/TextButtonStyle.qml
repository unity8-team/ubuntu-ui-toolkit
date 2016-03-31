/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Pierre Bertet <pierre.bertet@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

Item {
    id: root

    property var button: styledItem
    property real minWidth: units.gu(10)
    property real hPadding: units.gu(2)
    property QtObject palette: (
        button.enabled? theme.palette.normal : theme.palette.disabled
    )
    property color bgColor: palette.foreground
    property color textColor: palette.foregroundText
    property real radius: units.gu(0.6)
    property font font: Qt.font({
        family: "Ubuntu",
        weight: button.strong? Font.Normal : Font.Light,
        pixelSize: FontUtils.sizeToPixels("medium"),
    })

    width: button.width
    height: button.height
    implicitWidth: Math.max(minWidth, foreground.width + 2 * hPadding)
    implicitHeight: units.gu(4)

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    FocusShape {
    }

    // Background (on hover)
    ShapeFill {
        id: background
        visible: background.color.a > 0
        anchors.fill: parent
        radius: root.radius
        opacity: button.hovered? 1 : 0
        color: root.bgColor
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    Label {
        id: foreground
        anchors.centerIn: parent
        text: button.text
        elide: Text.ElideRight
        color: root.textColor
        font: root.font
    }
}
