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
import Ubuntu.Components.Private 1.3

Item {
    id: buttonStyle

    property var button: styledItem
    property real minimumWidth: button.iconName && button.text? 0 : units.gu(10)
    property real horizontalPadding: units.gu(2)
    property real iconSpacing: units.gu(1)

    /*!
      \deprecated
      The property is deprecated, use the fillColor
      and textColor properties instead.
    */
    property color defaultColor

    property color fillColor: {
        if (button.type === Button.Text) {
            return theme.palette.normal.foreground
        }
        if (button.emphasis === Button.Positive) {
            return theme.palette.normal.positive
        }
        if (button.emphasis === Button.Negative) {
            return theme.palette.normal.negative
        }
        return theme.palette.normal.foreground
    }

    property color textColor: {
        if (!button.enabled && button.type !== Button.Normal) {
            return theme.palette.normal.foregroundText
        }
        if (button.emphasis === Button.Positive) {
            return button.type !== Button.Normal
                       ? theme.palette.normal.positive
                       : theme.palette.normal.positiveText
        }
        if (button.emphasis === Button.Negative) {
            return button.type !== Button.Normal
                       ? theme.palette.normal.negative
                       : theme.palette.normal.negativeText
        }
        return theme.palette.normal.foregroundText
    }

    property color outlineColor: textColor
    property real outlineWidth: units.dp(1)
    property real outlineRadius: units.gu(1.3)

    property color overlayColor: theme.palette.normal.foregroundText

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

    // Normal mode
    UbuntuShape {
        id: background
        visible: (
            (backgroundColor.a > 0 || backgroundSource) &&
            button.type !== Button.Outline
        )
        anchors.fill: parent
        radius: "small"
        aspect: {
            if (button.pressed && button.enabled) {
                return UbuntuShape.Inset
            }
            if (button.type === Button.Text) {
                return UbuntuShape.Flat
            }
            if (button.type === Button.Normal && button.pressed) {
                return UbuntuShape.Inset
            }
            if (button.type === Button.Normal && !button.enabled) {
                return UbuntuShape.Flat
            }
            return UbuntuShape.DropShadow
        }
        opacity: (button.type !== Button.Text || button.hovered)? 1 : 0
        source: backgroundSource
        backgroundColor: fillColor
        backgroundMode: UbuntuShape.SolidColor
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
        Behavior on backgroundColor {
            ColorAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    // Outline mode
    Frame {
        id: outline
        visible: button.type === Button.Outline
        anchors.fill: parent
        thickness: outlineWidth
        radius: outlineRadius
        color: outlineColor
        opacity: button.enabled? 1 : 0.2
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
        iconInsertion: button.iconInsertion
        iconSize: units.gu(2)
        font: button.font
        spacing: iconSpacing
        transformOrigin: Item.Top
        opacity: button.enabled? 1 : 0.2
    }

    // Overlay (on hover)
    UbuntuShape {
        id: overlay
        anchors.fill: parent
        aspect: UbuntuShape.SolidColor
        backgroundMode: UbuntuShape.SolidColor
        radius: "small"
        opacity: button.hovered && !button.pressed? 1 : 0
        visible: button.type !== Button.Text
        source: backgroundSource
        backgroundColor: Qt.rgba(
            overlayColor.r,
            overlayColor.g,
            overlayColor.b,
            0.05
        )
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }
}
