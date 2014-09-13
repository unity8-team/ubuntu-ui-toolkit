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
import Ubuntu.Components 1.1
import QtQuick.Layouts 1.1

MainView {
    width: units.gu(50)
    height: units.gu(100)

    applicationName: "TestSuite"

    Page {
        title: "ListItemLayout test suite"
        active: true

        Flickable {
            anchors.fill: parent
            contentHeight: childrenRect.height
            Column {
                width: parent.width
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        id: layout
                        ListItemContainer {
                            id: leading
                            preset: "leading"
                            enabled: true
                            Layout.maximumWidth: units.gu(3)
                            Layout.maximumHeight: units.gu(3)
                            onClicked: trailing.visible = !trailing.visible
                            UbuntuShape {
                                width: parent.Layout.maximumWidth
                                height: parent.Layout.maximumHeight
                                color: UbuntuColors.green
                            }
                        }
                        ListItemContainer {
                            preset: "captions"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Caption"
                                    font.weight: Text.Normal
                                }
                                Label {
                                    text: "Click to hide/show leading container"
                                    fontSize: "small"
                                }
                            }
                            enabled: true
                            onClicked: leading.visible = !leading.visible
                        }
                        ListItemContainer {
                            id: trailing
                            preset: "trailing"
                            Label {
                                text: "Detail\n second line"
                                fontSize: "small"
                            }
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        ListItemContainer {
                            preset: "leading"
                            UbuntuShape {
                                width: parent.Layout.maximumWidth
                                height: parent.Layout.maximumHeight
                                color: UbuntuColors.green
                            }
                        }
                        ListItemContainer {
                            preset: "captions"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Caption"
                                    font.weight: Text.Normal
                                }
                                Label {
                                    text: "Subtitle\n third line"
                                    fontSize: "small"
                                }
                            }
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        ListItemContainer {
                            preset: "captions"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Caption"
                                    font.weight: Text.Normal
                                }
                                Label {
                                    text: "Subtitle\n third line"
                                    fontSize: "small"
                                }
                            }
                        }
                        ListItemContainer {
                            preset: "trailer"
                            Label {
                                text: "Detail\n second line"
                                fontSize: "small"
                            }
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        ListItemContainer {
                            preset: "captions"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Caption"
                                    font.weight: Text.Normal
                                }
                                Label {
                                    text: "Subtitle\n third line"
                                    fontSize: "small"
                                }
                            }
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        ListItemContainer {
                            preset: "leading"
                            UbuntuShape {
                                width: parent.Layout.maximumWidth
                                height: parent.Layout.maximumHeight
                                color: UbuntuColors.green
                            }
                        }
                        ListItemContainer {
                            preset: "captions"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Caption"
                                    font.weight: Text.Normal
                                }
                                Label {
                                    text: "Subtitle\n third line"
                                    fontSize: "small"
                                }
                            }
                        }
                        UbuntuShape {
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: units.gu(3)
                            color: UbuntuColors.blue
                        }

                        ListItemContainer {
                            preset: "trailing"
                            ColumnLayout {
                                spacing: 0
                                Label {
                                    text: "Detail text"
                                    fontSize: "small"
                                }
                                Label {
                                    text: "Subdetail text"
                                    fontSize: "small"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
