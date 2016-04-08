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
 */

// FIXME(loicm) Clean up code to test texture sharing.

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

Item {
    id: root
    width: 600
    height: 600
    focus: true

    property variant styles: [ "Drop", "Inner" ]
    property variant shapes: [ "Squircle", "Circle" ]

    property string textOverlayString:
        "style      (t): " + styles[dropShadow.style] + "\n" +
        "shape      (y): " + shapes[dropShadow.shape] + "\n" +
        "size     (s/S): " + dropShadow.size.toFixed(1) + "\n" +
        "radius   (r/R): " + dropShadow.radius.toFixed(1) + "\n" +
        "angle    (a/A): " + dropShadow.angle.toFixed(2) + "\n" +
        "distance (d/D): " + dropShadow.distance.toFixed(2) + "\n" +
        "color      (c): " + dropShadow.color + "\n" +
        "opacity  (o/O): " + dropShadow.opacity.toFixed(2) + "\n"

    Item {
        id: scene
        anchors.fill: parent

        ShapeShadow {
            id: dropShadow
            anchors.fill: parent
            anchors.margins: 200.0
            width: 100
            height: 100
            visible: true
        }
        ShapeFill {
            id: fill
            anchors.fill: parent
            anchors.margins: 200.0
            color: "white"
            radius: dropShadow.radius
            shape: dropShadow.shape
            visible: false
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

    property real toto: 0.0
    property real fillA: 1.0

    Keys.onPressed: {
        var shift = event.modifiers & Qt.ShiftModifier;
        if (event.key == Qt.Key_T) {
            dropShadow.style = (dropShadow.style + 1) % 2;
        } else if (event.key == Qt.Key_Y) {
            dropShadow.shape = (dropShadow.shape + 1) % 2;
        } else if (event.key == Qt.Key_S) {
            dropShadow.size = dropShadow.size + (shift ? 1.0 : -1.0);
        } else if (event.key == Qt.Key_R) {
            dropShadow.radius = dropShadow.radius + (shift ? 1.0 : -1.0);
        } else if (event.key == Qt.Key_C) {
            dropShadow.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
            fillA = fillA + (shift ? 0.1 : -0.1);
            if (fillA > 1.0) fillA = 1.0;
            else if (fillA < 0.0) fillA = 0.0;
            fill.color = Qt.rgba(1.0, 0.0, 0.0, fillA);
        } else if (event.key == Qt.Key_O) {
            dropShadow.opacity = dropShadow.opacity + (shift ? 0.02 : -0.02);
            fill.opacity = fill.opacity + (shift ? 0.02 : -0.02);
        // }
        // else if (event.key == Qt.Key_A) {
        //     fill.visible = !fill.visible;
        } else if (event.key == Qt.Key_V) {
            dropShadow.visible = !dropShadow.visible;
        }

        else if (event.key == Qt.Key_A) {
            toto = toto + (shift ? 1.0 : -1.0);
            dropShadow.angle = toto;
        } else if (event.key == Qt.Key_D) {
            dropShadow.distance = dropShadow.distance + (shift ? 1.0 : -1.0);
        }
    }
}
