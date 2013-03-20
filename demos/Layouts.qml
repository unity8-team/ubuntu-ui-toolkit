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

import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    id: root
    width: 600
    height: 400

    LayoutManager {
        anchors.fill: parent
        onLayoutChanged: print("layout", layout)
        layouts: [
            Layout {
                name: "one"
                when: root.width > 600
                Rectangle {
                    anchors.fill: parent
                    opacity: 0.4
                    color: "green"
                }
                Item {
                    anchors { bottom: parent.bottom; right: parent.right }
                    Layout.item: "rectangle"
                    width: childrenRect.width
                    height: childrenRect.height
                }
            },
            Layout {
                name: "two"
                when: root.width <= 600
                Item {
                    anchors.fill: parent
                    opacity: 0.2
                    Layout.item: "box"
                }
            }
        ]

        Rectangle {
            width: 200
            height: 100
            color: (root.width % 20 <= 10) ? "red" : "yellow"
            LayoutManager.itemName: "rectangle"
        }

        Rectangle {
            x: 100
            y: 200
            width: 300
            height: 200
            color: "blue"
        }

        Rectangle {
            x: 300
            y: 100
            width: 200
            height: 200
            color: "purple"
            LayoutManager.itemName: "box"
        }
    }
}
