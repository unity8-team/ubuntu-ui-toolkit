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

    Page {
        title: "ListItemLayout test suite"
        active: true

        Flickable {
            anchors.fill: parent
            contentHeight: childrenRect.height
            Column {
                width: parent.width
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
                                text: "Title"
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
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle"
                            enabled: true
                            preventStealing: true
                            onClicked: print("Captions clicked");
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                    onClicked: print("ListItem clicked")
                }
                ListItem {
                    ListItemLayout {
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle"
                        }
                        Switch {
                        }
                    }
                    onClicked: print("clicked on ListItem")
                }
                ListItem {
                    ListItemLayout {
                        ImageContainer {
                            shape.image: Image { source: "images.png" }
                        }
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle text"
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
                ListItem {
                    ListItemLayout {
                        ImageContainer {
                            shape.image: Image { source: "images.png" }
                            shape.width: units.gu(3)
                            shape.height: units.gu(3)
                            enabled: true
                        }
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle text"
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
                ListItem {
                    ListItemLayout {
                        ImageContainer {
                            shape.color: UbuntuColors.blue
                        }
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle text"
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
                ListItem {
                    StandardLayout {
                        image.shape.color: UbuntuColors.blue
                        captions {
                            title.text: "Captions (title)"
                            subtitle.text: "Subtitle text"
                        }
                        details {
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
                ListItem {
                    ListItemLayout {
                        ImageContainer {
                            shape.color: UbuntuColors.blue
                        }
                        Captions {
                            title.text: "Caption (title)"
                            subtitle.text: "Subtitle text"
                        }
                        Switch {
                        }
                        Captions {
                            preset: "details"
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                    }
                }
                ListItem {
                    id: customItem
                    StandardLayout {
                        image {
                            shape.color: UbuntuColors.blue
                            enabled: true
                            onClicked: console.log("clicked over the image, Switch is not toggled")
                        }
                        captions {
                            title.text: "Captions (title)"
                            subtitle.text: "Subtitle text"
                        }
                        details {
                            title.text: "Text"
                            subtitle.text: "Text"
                        }
                        Switch {
                            id: toggle
                        }
                    }
                    Component.onCompleted: clicked.connect(toggle.clicked)
                }
            }
        }
    }
}
