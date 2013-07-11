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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(40)
    height: units.gu(60)
    Component {
        id: popoverComponent

        Popover {
            id: popover
            Column {
                id: containerLayout
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                ListItem.Header { text: "Standard list items" }
                ListItem.Standard { text: "Do something" }
                ListItem.Standard { text: "Do something else"; showDivider: false }
//                ListItem.Header { text: "Buttons" }
//                ListItem.SingleControl {
//                    highlightWhenPressed: false
//                    control: Button {
//                        text: "Do nothing"
//                        anchors {
//                            fill: parent
//                            margins: units.gu(1)
//                        }
//                    }
//                }
//                ListItem.SingleControl {
//                    highlightWhenPressed: false
//                    control: Button {
//                        text: "Close"
//                        anchors {
//                            fill: parent
//                            margins: units.gu(1)
//                        }
//                        onClicked: PopupUtils.close(popover)
//                    }
//                }
            }
        }
    }

    Grid {
        id: grid
        anchors.fill: parent
        //anchors.margins: units.gu(0.5)
        columns: 6
        rows: 8
        spacing: units.gu(6)

        Repeater {
            model: grid.columns * grid.rows
            delegate: Button {
                id: button
                text: "open"
                onClicked: PopupUtils.open(popoverComponent, button)
            }
        }
    }
}
