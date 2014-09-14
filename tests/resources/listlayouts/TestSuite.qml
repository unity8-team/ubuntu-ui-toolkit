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
                        FramedImage {
                            id: leading2
                            frame.width: units.gu(3)
                            frame.height: units.gu(3)
                            iconName: "edit"
                            enabled: true
                            onClicked: trailing2.state = trailing2.state === "hidden" ? "" : "hidden"
                        }

                        Captions {
                            title.text: "Caption"
                            subtitle.text: "Click to hide/show leading container"
                            enabled: true
                            onClicked: leading2.visible = !leading2.visible
                        }
                        Captions {
                            id: trailing2
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                            states: State {
                                name: "hidden"
                                PropertyChanges {
                                    target: trailing2
                                    visible: false
                                }
                            }
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        FramedImage {
                            color: UbuntuColors.red
                            image: Image { source: "images.png" }
                        }

                        Captions {
                            title.text: "Caption"
                            subtitle.text: "Subtitle\n third line"
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        Captions {
                            title.text: "Caption"
                            subtitle.text: "Subtitle\n third line"
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        Captions {
                            title.text: "Caption"
                            subtitle.text: "Subtitle\n third line"
                        }
                        ListItemContainer {
                            preset: "trailing"
                            Switch {}
                        }
                    }
                }
                Rectangle { border { color: UbuntuColors.lightGrey; width: 1 }
                    width: parent.width; height: units.gu(7)
                    ListItemLayout {
                        ListItemContainer {
                            preset: "leading"
                        }
                        Captions {
                            title.text: "Caption with empty details"
                            subtitle.text: "Subtitle\n third line"
                        }
                        Captions {
                            preset: "details"
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
                        Captions {
                            title.text: "Caption"
                        }
                        UbuntuShape {
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: units.gu(3)
                            color: UbuntuColors.blue
                        }

                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
            }
        }
    }
}
