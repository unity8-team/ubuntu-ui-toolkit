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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1


Rectangle {
    id: root
    width: units.gu(50)
    height: units.gu(80)
    color: "lightgrey"

    Item {
        id: main
        anchors.fill: parent

        Label {
            anchors {
                fill: parent
                margins: units.gu(5)
            }
            text: "Lorem ipsum dolor sit amet, ut ullamcorper, wisi lectus, duis mattis ullamcorper nullam adipiscing, at odio laboriosam vitae, cras elit lorem sapien ullamcorper in. Dolor ac scelerisque elementum. Blandit quis nam sit lorem erat ut, eget cursus donec vivamus duis, pellentesque vel in pellentesque mauris natoque. Vulputate ultricies sed, risus molestie mauris, suscipit egestas wisi adipiscing tortor leo suspendisse, arcu a nec cum a pede eget, turpis vivamus turpis sagittis. Nonummy urna, semper feugiat recusandae, pellentesque erat molestie duis odio hac, est urna."
            wrapMode: Text.Wrap
        }

        MouseArea {
            id: dragArea
            drag.target: dragArea
            width: units.gu(15)
            height: units.gu(8)
            x: parent.width/3
            y: parent.width/2
        }

    }

    // colorRect and magnifier are outside of main, because otherwise they also get
    // rendered into the texture that is used in magnifier (giving a recursion).
    Rectangle {
        id: colorRect
        color: "yellow"
        width: dragArea.width
        height: dragArea.height
        x: dragArea.x
        y: dragArea.y
    }

    Magnifier {
        id: magnifier
        sourceItem: main
        anchors.fill: colorRect
        scaleFactor: scaleSlider.value
    }

    Slider {
        id: scaleSlider
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        minimumValue: 1.0
        maximumValue: 2.0
        value: 1.0
        live: true
    }
}
