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

import QtQuick 2.2
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1

MainView {
    width: units.gu(50)
    height: units.gu(100)

    applicationName: "TestSuite"
    useDeprecatedToolbar: false

    Tabs {
        Tab {
            page: Page {
                title: "ListItemLayout"
                Column {
                    width: parent.width
                    spacing: units.gu(0.5)
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
                    // vertical item
                    Label {
                        text: "Vertical layout"
                        fontSize: "large"
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
}
