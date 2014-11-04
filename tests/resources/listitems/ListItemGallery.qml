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

    property string longText: "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."

    Tabs {
        Tab {
            title: "ListItemLayout"
            page: Page {
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

        Tab {
            title: "Captions"
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

        // Custom panel
        Tab {
            title: "Custom panel"
            page: Page {
                ListView {
                    anchors.fill: parent
                    model: 50
                    delegate: ListItem {
                        ListItemLayout {
                            Shape {
                                Layout.preferredWidth: units.gu(5)
                                Layout.preferredHeight: units.gu(5)
                                color: UbuntuColors.blue
                            }

                            Captions {
                                title.text: "Captions (title)"
                                subtitle.text: "Subtitle text"
                            }
                            Captions {
                                preset: "summary"
                                title.text: "Text"
                                subtitle.text: "Text"
                            }
                        }
                        trailingActions: ListItemActions {
                            actions: [
                                Action {
                                    iconName: "alarm-clock"
                                    onTriggered: print(iconName, "triggered", value)
                                },
                                Action {
                                    iconName: "camcorder"
                                    onTriggered: print(iconName, "triggered", value)
                                },
                                Action {
                                    iconName: "stock_website"
                                    onTriggered: print(iconName, "triggered", value)
                                }
                            ]
                            backgroundColor: UbuntuColors.lightGrey
                            foregroundColor: "white"

                            // DropBox's Mailbox
                            // depending on the offset dragged in, show different actions
                            // trigger the action when released
                            customPanel: Rectangle {
                                id: panel
                                property bool leadingPanel: ListItemActions.container.status == ListItemActions.Leading
                                property Item contentItem: (ListItemActions.listItem) ?
                                                               ListItemActions.listItem.contentItem : null
                                anchors {
                                    left: contentItem ? contentItem.right : undefined
                                    top: contentItem ? contentItem.top : undefined
                                    bottom: contentItem ? contentItem.bottom : undefined
                                }
                                width: contentItem ? (contentItem.width - units.gu(10)) : 0
                                color: colors[visibleAction]

                                property real slotSize: panel.width / ListItemActions.container.actions.length
                                // give a small margin so we don't jump to the next item
                                property int visibleAction: (slotSize > 0) ?
                                                                MathUtils.clamp((ListItemActions.offset - 1) / slotSize, 0, ListItemActions.container.actions.length-1) :
                                                                0
                                property var colors: [UbuntuColors.blue, UbuntuColors.lightGrey, UbuntuColors.coolGrey]

                                Item {
                                    anchors {
                                        left: parent.left
                                        top: parent.top
                                        bottom: parent.bottom
                                    }
                                    width: height
                                    Icon {
                                        width: units.gu(3)
                                        height: width
                                        anchors.centerIn: parent
                                        color: "white"
                                        name: ListItemActions.container.actions[visibleAction].iconName
                                    }
                                }

                                ListItemActions.onDraggingChanged: {
                                    if (!ListItemActions.dragging) {
                                        // snap first, then trigger
                                        ListItemActions.snapToPosition((visibleAction + 1) * slotSize);
                                        ListItemActions.container.actions[visibleAction].triggered(ListItemActions.listItemIndex)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}