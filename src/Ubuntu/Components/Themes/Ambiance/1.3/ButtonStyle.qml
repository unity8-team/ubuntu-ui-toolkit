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
    id: buttonStyle

    property var button: styledItem
    property real minimumWidth: button.iconName && button.text? 0 : units.gu(10)
    property real horizontalPadding: units.gu(2)
    property real iconSpacing: units.gu(1)

    property QtObject palette: (
        button.enabled? theme.palette.normal : theme.palette.disabled
    )

    property color fillColor: {
        if (button.type === Button.Text) {
            return palette.foreground // hover effect only
        }
        if (button.emphasis === Button.Positive) {
            return palette.positive
        }
        if (button.emphasis === Button.Negative) {
            return palette.negative
        }
        return palette.foreground
    }

    property color textColor: {
        if (button.emphasis === Button.Positive) {
            return button.type !== Button.Normal
                       ? palette.backgroundPositiveText
                       : palette.positiveText
        }
        if (button.emphasis === Button.Negative) {
            return button.type !== Button.Normal
                       ? palette.backgroundNegativeText
                       : palette.negativeText
        }
        return palette.foregroundText
    }

    property color outlineColor: textColor
    property real outlineWidth: units.dp(1)
    property real shapeRadius: units.gu(0.6)

    property color overlayColor: palette.foregroundText

    property font defaultFont: Qt.font({
        family: "Ubuntu",
        pixelSize: FontUtils.sizeToPixels("medium"),
    })

    property Gradient defaultGradient
    property real buttonFaceOffset: 0
    property bool stroke: false

    /*!
      The property overrides the button's default background with an item. This
      item can be used by derived styles to reuse the ButtonStyle and override
      the default coloured background with an image or any other drawing.
      The default value is null.
    */
    property Item backgroundSource: null

    width: button.width
    height: button.height
    implicitWidth: Math.max( minimumWidth, foreground.width + 2 * horizontalPadding)
    implicitHeight: units.gu(4)

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    // Background
    Frame {
        id: background
        visible: background.color.a > 0 || !!backgroundSource
        anchors.fill: parent
        radius: shapeRadius
        thickness: (
            button.type === Button.Outline
             ? outlineWidth
             : Math.max(width, height)
        )
        opacity: (button.type !== Button.Text || button.hovered)? 1 : 0
        color: button.type === Button.Outline? outlineColor : fillColor
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    // Foreground (text and icon)
    ButtonForeground {
        id: foreground
        anchors {
            centerIn: parent
            horizontalCenterOffset: buttonFaceOffset
        }
        text: button.text
        textColor: buttonStyle.textColor
        iconSource: button.iconSource
        iconPosition: button.iconPosition
        iconSize: units.gu(2)
        font: button.font
        spacing: iconSpacing
        transformOrigin: Item.Top
    }

    // Overlay (on hover)
    Frame {
        anchors.fill: parent
        radius: shapeRadius
        opacity: button.hovered && !button.pressed? 1 : 0
        visible: button.type !== Button.Text
        color: Qt.rgba(overlayColor.r, overlayColor.g, overlayColor.b, 0.05)
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }
}
