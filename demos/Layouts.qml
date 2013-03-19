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
        layouts: [
            Layout {
                name: "one"
                when: root.width > 600
                Item {
                    anchors.fill: parent
                    Layout.item: "rectangle"
                }
                Rectangle {
                    anchors.fill: parent
                    color: "green"
                }
            },
            Layout {
                name: "two"
                when: root.width <= 600
            }
        ]

        Rectangle {
            width: 200
            height: 100
            color: "red"
            LayoutManager.itemName: "rectangle"
        }
    }
}
