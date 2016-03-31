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

    property color color: {
        // In Outline mode, the border has the same color than the text
        if (button.type === Button.Outline) {
            return textColor
        }
        if (button.type === Button.Positive) {
            return palette.positive
        }
        if (button.type === Button.Negative) {
            return palette.negative
        }
        return palette.foreground
    }

    property color textColor: {
        if (button.type === Button.Positive) {
            return palette.positiveText
        }
        if (button.type === Button.Negative) {
            return palette.negativeText
        }
        return palette.foregroundText
    }

    property real frameThickness: units.dp(1)

    property real radius: units.gu(0.6)

    property color overlayColor: palette.foregroundText

    property font defaultFont: Qt.font({
        family: "Ubuntu",
        pixelSize: FontUtils.sizeToPixels("medium"),
    })

    /*!
      The property overrides the button's default background with an item. This
      item can be used by derived styles to reuse the ButtonStyle and override
      the default coloured background with an image or any other drawing.
      The default value is null.
    */
    property Item backgroundSource: null

    width: button.width
    height: button.height
    implicitWidth: Math.max(minimumWidth, foreground.width + 2 * horizontalPadding)
    implicitHeight: units.gu(4)

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    FocusShape {
    }

    // Drop shadow
    ShapeShadow {
        visible: button.enabled
        opacity: (
            button.type === Button.Outline ||
            styledItem.keyNavigationFocus ||
            button.pressed? 0 : 1
        )
        width: parent.width
        height: parent.height
        radius: buttonStyle.radius
        style: ShapeShadow.Outer
        angle: 90
        distance: units.gu(0.1)
        size: units.gu(0.1)
        color: Qt.rgba(0, 0, 0, 0.2)
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    // Background shape
    ShapeFill {
        anchors.fill: parent
        visible: (
            button.type !== Button.Outline ||
            button.keyNavigationFocus ||
            (button.hovered && !button.pressed)
        )
        color: {
            var baseColor = buttonStyle.color
            if (button.keyNavigationFocus && button.type === Button.Outline) {
                return Qt.rgba(0, 0, 0, 0.15)
            }
            if (button.hovered && !button.pressed) {
                return button.type === Button.Outline
                    ? Qt.rgba(0, 0, 0, 0.05)
                    : Qt.tint(buttonStyle.color, Qt.rgba(0, 0, 0, 0.05))
            }
            return buttonStyle.color
        }
        radius: buttonStyle.radius
        Behavior on opacity {
            UbuntuNumberAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
        Behavior on color {
            enabled: button.type !== Button.Outline
            ColorAnimation {
                duration: UbuntuAnimation.SnapDuration
            }
        }
    }

    // Outline
    ShapeFrame {
        anchors.fill: parent
        visible: button.type === Button.Outline && !button.keyNavigationFocus
        color: buttonStyle.color
        radius: buttonStyle.radius
        thickness: frameThickness
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
        }
        text: button.text
        textColor: buttonStyle.textColor
        iconSource: button.iconSource
        iconPosition: (
            button.iconPosition !== undefined
                ? button.iconPosition
                : Button.Before
        )
        iconSize: units.gu(2)
        font: button.font || defaultFont
        spacing: iconSpacing
        transformOrigin: Item.Top
    }
}
