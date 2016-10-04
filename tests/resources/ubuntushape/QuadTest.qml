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
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

Item {
    id: root
    width: 800
    height: 500
    focus: true

    property variant shapes: [ "Quad.Squircle", "Quad.Circle" ]

    property string textOverlayString:
        "shape ............... (a): " + shapes[quad.shape] + "\n" +
        "radius ............ (z/Z): " + quad.radius.toFixed(2) + "\n" +
        "color ............... (e): " + quad.color + "\n\n" +
        "dropShadowSize .... (r/R): " + quad.dropShadowSize.toFixed(2) + "\n" +
        "dropshadowDistance  (t/T): " + quad.dropShadowDistance.toFixed(2) + "\n" +
        "dropshadowAngle ... (y/Y): " + quad.dropShadowAngle.toFixed(2) + "\n" +
        "dropshadowColor ..... (u): " + quad.dropShadowColor + "\n\n" +
        "innerShadowSize ... (i/I): " + quad.innerShadowSize.toFixed(2) + "\n" +
        "innershadowDistance (o/O): " + quad.innerShadowDistance.toFixed(2) + "\n" +
        "innershadowAngle .. (p/P): " + quad.innerShadowAngle.toFixed(2) + "\n" +
        "innershadowColor .... (q): " + quad.innerShadowColor + "\n\n" +
        "frameThickness .... (s/S): " + quad.frameThickness.toFixed(2) + "\n" +
        "frameSpace ........ (d/D): " + quad.frameSpace.toFixed(2) + "\n" +
        "frameColor .......... (f): " + quad.frameColor + "\n\n" +
        "opacity ........... (g/G): " + quad.opacity.toFixed(2)

    Item {
        id: scene
        anchors.fill: parent
        Quad {
            id: quad
            anchors.fill: parent
            anchors.leftMargin: 300
            anchors.margins: 100
            visible: true
        }
    }

    ZoomPan {
        id: zoomPan
        anchors.fill: parent
        scene: scene
        area: mouseArea
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: true
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
    }

    Text {
        id: textOverlay
        width: 200
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 14
        font.weight: Font.Bold
        color: "black"
        text: textOverlayString
    }

    Keys.onPressed: {
        var shift = event.modifiers & Qt.ShiftModifier;
        if (event.key == Qt.Key_A) {
            quad.shape = (quad.shape + 1) % 2;
        } else if (event.key == Qt.Key_Z) {
            quad.radius = quad.radius + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_E) {
            quad.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_R) {
            quad.dropShadowSize = quad.dropShadowSize + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_T) {
            quad.dropShadowDistance = quad.dropShadowDistance + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_Y) {
            quad.dropShadowAngle = quad.dropShadowAngle + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_U) {
            quad.dropShadowColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_I) {
            quad.innerShadowSize = quad.innerShadowSize + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_O) {
            quad.innerShadowDistance = quad.innerShadowDistance + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_P) {
            quad.innerShadowAngle = quad.innerShadowAngle + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_Q) {
            quad.innerShadowColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_S) {
            quad.frameThickness = quad.frameThickness + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_D) {
            quad.frameSpace = quad.frameSpace + (shift ? 0.25 : -0.25);
        } else if (event.key == Qt.Key_F) {
            quad.frameColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_G) {
            quad.opacity = quad.opacity + (shift ? 0.02 : -0.02);
        }
    }
}
