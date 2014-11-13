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

import QtQuick 2.0
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1

Tab {
    title: "Captions"
    property string longText: "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."

    page: Page {
        Column {
            width: parent.width
            ListItem {
                ListItemLayout {
                    Captions {
                        title.text: "Text"
                        subtitle.text: "subtitle"
                    }
                    Captions {
                        preset: "summary"
                        title.text: "Text"
                        subtitle.text: "Text"
                    }
                }
            }
            ListItem {
                height: units.gu(9)
                ListItemLayout {
                    Captions {
                        id: main
                        title.text: "Caption (title) click on captions and"
                        subtitle.text: longText
                        MouseArea {
                            anchors.fill: parent
                            onClicked: print("Captions clicked");
                        }
                    }

                    Captions {
                        preset: "summary"
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
                        Component.onCompleted: subtitle.Layout.alignment = Qt.AlignRight
                    }
                    Switch {
                    }
                }
                onClicked: print("clicked on ListItem")
            }
            Rectangle {
                width: units.gu(40)
                height: units.gu(30)

                ListItemLayout {
                    direction: Qt.Vertical
                    Captions {
                        title.text: "Caption: " + longText
                        subtitle.text: "Subtitle: " + longText
                        layout {
                            data: [
                                Label {
                                    text: "Third line"
                                }
                            ]
                            spacing: units.gu(1)
                        }
                    }
                    Switch {
                    }
                    Captions {
                        preset: "summary"
                        title.text: "Text"
                        subtitle.text: "Text"
                    }
                }
            }
        }
    }
}
