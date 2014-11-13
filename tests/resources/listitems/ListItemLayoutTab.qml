/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.3
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0
import QtQuick.Layouts 1.1

Tab {
    title: "ListItemLayout"
    page: Page {
        Flickable {
            anchors.fill: parent
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: parent.width
                Header {
                    text: "Horizontal layouts"
                }
                ListItem {
                    ListItemLayout {
                        ColumnLayout {
                            spacing: units.gu(0.5)
                            Layout.alignment: Qt.AlignVCenter
                            Layout.maximumHeight: parent.height
                            Layout.maximumWidth: parent.width
                            Layout.preferredWidth: childrenRect.width
                            Layout.fillWidth: true
                            Label {
                                text: "Horizontal layout"
                                fontSize: "large"
                            }
                            Label {
                                text: "Subtitle"
                                fontSize: "small"
                            }
                        }
                        Switch {
                        }
                    }
                }
                ListItem {
                    ListItemLayout {
                        Label {
                            text: "Click to toggle checkbox"
                        }
                        CheckBox {
                            id: checkbox
                        }
                    }
                    onClicked: checkbox.clicked()
                }

                // vertical item
                Header {
                    text: "Vertical layouts"
                }
                Rectangle {
                    width: units.gu(40)
                    height: units.gu(30)

                    ListItemLayout {
                        direction: Qt.Vertical
                        UbuntuShape {
                            width: units.gu(5)
                            height: width
                            color: UbuntuColors.blue
                        }
                        Switch {
                        }
                        UbuntuShape {
                            width: units.gu(5)
                            height: width
                            color: UbuntuColors.blue
                        }
                    }
                }
            }
        }
    }
}
