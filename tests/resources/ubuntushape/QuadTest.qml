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
 */

import QtQuick 2.4
import QtQuick.Controls 1.4 as QQC
import QtQuick.Controls.Styles 1.4 as QQCS
import QtQuick.Dialogs 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

Rectangle {  // FIXME(loicm) Replace with Item.
    id: root

    readonly property color backgroundColor: "#dddddd"
    readonly property int fontSize: 15
    readonly property int spacing: 15
    readonly property real controlsWidth: 150.0
    readonly property variant shapes: [ Quad.Squircle, Quad.Circle ]
    readonly property color borderColor: Qt.rgba(0.0, 0.0, 0.0, 0.75)
    readonly property color canvasColor: Qt.rgba(1.0, 1.0, 1.0, 1.0)
    readonly property real defaultQuadWidth: 256.0
    readonly property real defaultQuadHeight: 256.0

    width: 1120
    height: 730
    color: backgroundColor  // FIXME(loicm) Replace with window clear color.
    focus: true

    // Common dialog for all color pickers.
    ColorDialog {
        id: colorDialog

        readonly property variant colorType: {
            "Fill" : 0,
            "DropShadow" : 1,
            "InnerShadow" : 2,
            "Border" : 3
        }
        property variant colors: [
            Qt.rgba(0.91, 0.33, 0.1, 1),
            Qt.rgba(0, 0, 0, 1),
            Qt.rgba(0, 0, 0, 1),
            Qt.rgba(1, 1, 1, 1)
        ]
        property variant liveColors: [ colors[0], colors[1], colors[2], colors[3] ]
        property int currentColorType: -1

        title: "Pick a color"
        showAlphaChannel: true
        modality: Qt.ApplicationModal

        onCurrentColorChanged: {
            liveColors[currentColorType] = currentColor;
            liveColors = liveColors;  // Notify does not work on array elements.
        }
        onAccepted: {
            var c = liveColors[currentColorType];
            colors[currentColorType] = Qt.rgba(c.r, c.g, c.b, c.a);
            colors = colors;  // Notify does not work on array elements.
        }
        onRejected: {
            liveColors[currentColorType] = colors[currentColorType];
            liveColors = liveColors;  // Notify does not work on array elements.
        }
    }

    // Canvas options title.
    Text {
        id: canvasOptionsText
        x: spacing; y: spacing
        font.pixelSize: fontSize * 1.1; renderType: Text.NativeRendering; color: "black"
        font.weight: Font.Bold;
        text: "Canvas"
    }

    // Help.
    Text {
        id: helpText
        x: 2.0 * spacing; y: canvasOptionsText.y + canvasOptionsText.height + spacing
        font.pixelSize: fontSize * 1.0; renderType: Text.NativeRendering; color: "#222222";
        width: quadParametersGrid.width; wrapMode: Text.WordWrap
        font.italic: true
        text: "Mouse wheel to zoom in and out, mouse press and move to pan."
        // TODO(loicm) Add color picking.
        // text: "Hold down Ctrl and hover pixels to show color values. Mouse wheel to zoom in and " +
        //       "out, mouse press and move to pan."
    }

    // Canvas options grid.
    Grid {
        id: canvasOptionsGrid
        x: root.spacing + root.spacing; y: helpText.y + helpText.height + root.spacing
        columns: 2; columnSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter

        // Zoom factor.
        Text {
            width: longestText.width
            height: shapeComboBox.height
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Zoom factor"
        }
        Text {
            height: shapeComboBox.height
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: zoomPan.zoomFactor.toFixed(1) + " x"
        }

        // Zoom grid.
        Text {
            height: shapeComboBox.height
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Zoom grid"
        }
        QQC.CheckBox {
            id: zoomGridCheckBox
            height: shapeComboBox.height
            checked: true
        }
    }

    // Quad properties title.
    Text {
        id: quadPropertiesText
        x: spacing; y: canvasOptionsGrid.y + canvasOptionsGrid.height + spacing
        font.pixelSize: fontSize * 1.1; renderType: Text.NativeRendering; color: "black"
        font.weight: Font.Bold
        text: "Quad"
    }

    // Quad properties grid.
    Grid {
        id: quadParametersGrid
        x: root.spacing + root.spacing; y: quadPropertiesText.y + quadPropertiesText.height + root.spacing
        columns: 3; columnSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter

        // Shape.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Shape"
        }
        QQC.ComboBox {
            id: shapeComboBox
            width: controlsWidth
            model: [ "Squircle", "Circle" ]
            style: QQCS.ComboBoxStyle {
                font.pixelSize: fontSize; renderType: Text.NativeRendering
            }
        }
        Text {
            width: 55
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: " "
        }

        // Width.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Width"
        }
        QQC.Slider {
            id: widthSlider
            width: controlsWidth
            minimumValue: 0.0; value: defaultQuadWidth; maximumValue: 512.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: widthSlider.value.toFixed(0) + " px"
        }

        // Height.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Height"
        }
        QQC.Slider {
            id: heightSlider
            width: controlsWidth
            minimumValue: 0.0; value: defaultQuadHeight; maximumValue: 512.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: heightSlider.value.toFixed(0) + " px"
        }

        // Radius.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Radius"
        }
        QQC.Slider {
            id: radiusSlider
            width: controlsWidth
            // FIXME(loicm) Fix rendering at small radii!
            // minimumValue: 0.0; maximumValue: 256.0; stepSize: 1.0
            minimumValue: 5.0; value: 5.0; maximumValue: 256.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: radiusSlider.value.toFixed(0) + " px"
        }

        // Fill color.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Fill color"
        }
        QQC.Button {
            width: controlsWidth
            height: shapeComboBox.height
            style: QQCS.ButtonStyle {
                background: Item {
                    Rectangle {
                        anchors.fill: parent
                        border.width: 1; border.color: borderColor; radius: 3
                        color: colorDialog.colors[colorDialog.colorType["Fill"]]
                    }
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: fontSize; renderType: Text.NativeRendering
                        color: lightness(colorDialog.colors[colorDialog.colorType["Fill"]]) > 0.5 ? "black" : "white"
                        text: colorDialog.colors[colorDialog.colorType["Fill"]]
                    }
                }
            }
            onClicked: {
                colorDialog.color = quad.fillColor;
                colorDialog.currentColorType = colorDialog.colorType["Fill"];
                colorDialog.open();
            }
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: (colorDialog.colors[colorDialog.colorType["Fill"]].a * 100).toFixed(0) + " %"
        }

        // Grid row separator hack.
        Text { text: " "; height: spacing } Text { text: " " } Text { text: " " }

        // Drop shadow size.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Drop shadow size"
        }
        QQC.Slider {
            id: dropShadowSizeSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 128.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: dropShadowSizeSlider.value.toFixed(0) + " px"
        }

        // Drop shadow distance.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Drop shadow distance"
        }
        QQC.Slider {
            id: dropShadowDistanceSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 256.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: dropShadowDistanceSlider.value.toFixed(0) + " px"
        }

        // Drop shadow angle.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Drop shadow angle"
        }
        QQC.Slider {
            id: dropShadowAngleSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 360.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: dropShadowAngleSlider.value.toFixed(0) + "°"
        }

        // Drop shadow color.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Drop shadow color"
        }
        QQC.Button {
            width: controlsWidth
            height: shapeComboBox.height
            style: QQCS.ButtonStyle {
                background: Item {
                    Rectangle {
                        anchors.fill: parent
                        border.width: 1; border.color: borderColor; radius: 3
                        color: colorDialog.colors[colorDialog.colorType["DropShadow"]]
                    }
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: fontSize; renderType: Text.NativeRendering
                        color: lightness(colorDialog.colors[colorDialog.colorType["DropShadow"]]) > 0.5 ? "black" : "white"
                        text: colorDialog.colors[colorDialog.colorType["DropShadow"]]
                    }
                }
            }
            onClicked: {
                colorDialog.color = quad.dropShadowColor;
                colorDialog.currentColorType = colorDialog.colorType["DropShadow"];
                colorDialog.open();
            }
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: (colorDialog.colors[colorDialog.colorType["DropShadow"]].a * 100).toFixed(0) + " %"
        }

        // Grid row separator hack.
        Text { text: " "; height: spacing } Text { text: " " } Text { text: " " }

        // Inner shadow size.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Inner shadow size"
        }
        QQC.Slider {
            id: innerShadowSizeSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 128.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: innerShadowSizeSlider.value.toFixed(0) + " px"
        }

        // Inner shadow distance.
        Text {
            id: longestText
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Inner shadow distance"
        }
        QQC.Slider {
            id: innerShadowDistanceSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 256.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: innerShadowDistanceSlider.value.toFixed(0) + " px"
        }

        // Inner shadow angle.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Inner shadow angle"
        }
        QQC.Slider {
            id: innerShadowAngleSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 360.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: innerShadowAngleSlider.value.toFixed(0) + "°"
        }

        // Inner shadow color.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Inner shadow color"
        }
        QQC.Button {
            width: controlsWidth
            height: shapeComboBox.height
            style: QQCS.ButtonStyle {
                background: Item {
                    Rectangle {
                        anchors.fill: parent
                        border.width: 1; border.color: borderColor; radius: 3
                        color: colorDialog.colors[colorDialog.colorType["InnerShadow"]]
                    }
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: fontSize; renderType: Text.NativeRendering
                        color: lightness(colorDialog.colors[colorDialog.colorType["InnerShadow"]]) > 0.5 ? "black" : "white"
                        text: colorDialog.colors[colorDialog.colorType["InnerShadow"]]
                    }
                }
            }
            onClicked: {
                colorDialog.color = quad.innerShadowColor;
                colorDialog.currentColorType = colorDialog.colorType["InnerShadow"];
                colorDialog.open();
            }
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: (colorDialog.colors[colorDialog.colorType["InnerShadow"]].a * 100).toFixed(0) + " %"
        }

        // Grid row separator hack.
        Text { text: " "; height: spacing } Text { text: " " } Text { text: " " }

        // Border size.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Border size"
        }
        QQC.Slider {
            id: borderSizeSlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 256.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: borderSizeSlider.value.toFixed(0) + " px"
        }

        // Border color.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Border color"
        }
        QQC.Button {
            width: controlsWidth
            height: shapeComboBox.height
            style: QQCS.ButtonStyle {
                background: Item {
                    Rectangle {
                        anchors.fill: parent
                        border.width: 1; border.color: borderColor; radius: 3
                        color: colorDialog.colors[colorDialog.colorType["Border"]]
                    }
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: fontSize; renderType: Text.NativeRendering
                        color: lightness(colorDialog.colors[colorDialog.colorType["Border"]]) > 0.5 ? "black" : "white"
                        text: colorDialog.colors[colorDialog.colorType["Border"]]
                    }
                }
            }
            onClicked: {
                colorDialog.color = quad.borderColor;
                colorDialog.currentColorType = colorDialog.colorType["Border"];
                colorDialog.open();
            }
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: (colorDialog.colors[colorDialog.colorType["Border"]].a * 100).toFixed(0) + " %"
        }

        // Grid row separator hack.
        Text { text: " "; height: spacing } Text { text: " " } Text { text: " " }

        // Opacity.
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: "Opacity"
        }
        QQC.Slider {
            id: opacitySlider
            width: controlsWidth
            minimumValue: 0.0; maximumValue: 100.0; value: 100.0; stepSize: 1.0
        }
        Text {
            font.pixelSize: fontSize; renderType: Text.NativeRendering; color: "black"
            text: opacitySlider.value.toFixed(0) + " %"
        }
    }

    // Canvas.
    Item {
        x: Math.round(spacing + quadParametersGrid.width + 2 * spacing); y: Math.round(spacing)
        width: Math.ceil(root.width - quadParametersGrid.width - 4 * spacing)
        height: Math.ceil(root.height - 2 * spacing)

        ZoomPan {
            id: zoomPan
            anchors.fill: scene
            scene: scene
            area: mouseArea
            zoomGrid: zoomGridCheckBox.checked
            backgroundColor: canvasColor
        }
        Item {
            id: scene
            anchors.fill: parent
            clip: zoomPan.zoomFactor <= 1.0
            Quad {
                id: quad
                x: Math.round((parent.width - width) / 2.0)
                y: Math.round((parent.height - height) / 2.0)
                width: widthSlider.value; height: heightSlider.value
                shape: shapes[shapeComboBox.currentIndex]
                radius: radiusSlider.value
                fillColor: colorDialog.liveColors[colorDialog.colorType["Fill"]]
                dropShadowSize: dropShadowSizeSlider.value
                dropShadowDistance: dropShadowDistanceSlider.value
                dropShadowAngle: dropShadowAngleSlider.value
                dropShadowColor: colorDialog.liveColors[colorDialog.colorType["DropShadow"]]
                innerShadowSize: innerShadowSizeSlider.value
                innerShadowDistance: innerShadowDistanceSlider.value
                innerShadowAngle: innerShadowAngleSlider.value
                innerShadowColor: colorDialog.liveColors[colorDialog.colorType["InnerShadow"]]
                borderSize: borderSizeSlider.value
                borderColor: colorDialog.liveColors[colorDialog.colorType["Border"]]
                opacity: opacitySlider.value * 0.01
            }
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            enabled: true; hoverEnabled: true
            acceptedButtons: Qt.AllButtons
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0); border.width: 1.0; border.color: Qt.rgba(0, 0, 0, 0.5)
        }
    }

    function lightness(color) {
        // Blend with background.
        var r = color.r + ((1.0 - color.a) * backgroundColor.r);
        var g = color.g + ((1.0 - color.a) * backgroundColor.g);
        var b = color.b + ((1.0 - color.a) * backgroundColor.b);
        // Compute lightness.
        return (Math.max(r, Math.max(g, b)) + Math.min(r, Math.min(g, b))) / 2.0;
    }
}
