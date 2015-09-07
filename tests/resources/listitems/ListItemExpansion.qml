/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Action {
        id: deleteAction
        iconName: "delete"
    }
    property list<Action> contextualActions: [
        Action {
            iconName: "edit"
        },
        Action {
            iconName: "share"
        },
        Action {
            iconName: "stock_website"
        }
    ]

    Page {
        title: "Expansion"
        Column {
            anchors.fill: parent
            spacing: units.dp(4)
            UbuntuListView {
                id: listView
                onEnabledChanged: print("enabled", enabled)
                width: parent.width
                height: parent.height / 2
                clip: true
                ViewItems.onExpandedIndicesChanged: print(ViewItems.expandedIndices)
                ViewItems.expansionFlags: ViewItems.CollapseOnOutsidePress

                model: ListModel {
                    Component.onCompleted: {
                        for (var i = 0; i < 25; i++) {
                            append({label: "List item #"+i});
                        }
                    }
                }

                delegate: ListItem {
                    id: item
                    objectName: "ListItem-" + index
                    leadingActions: ListItemActions {
                        actions: deleteAction
                    }
                    trailingActions: ListItemActions {
                        actions: contextualActions
                    }
                    expansion {
                        height: units.gu(15)
                        content: UbuntuListView {
                            model: 5
                            delegate: Button {
                                height: units.gu(7)
                                width: parent.width - units.gu(5)
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: i18n.tr("Sub-item #%1").arg(modelData)
                                onClicked: {
                                    item.expansion.expanded = false;
                                }
                            }
                        }
                    }
                    expansion.overlapListItem: true

                    RowLayout {
                        anchors {
                            fill: parent
                            margins: units.gu(0.5)
                            leftMargin: anchors.rightMargin
                            rightMargin: units.gu(2)
                        }
                        Captions {
                            id: captions
                            title.text: label
                            subtitle.text: "from index #" + index
                        }
                    }

                    onPressAndHold: {
                        print("expand/collapse")
                        expansion.expanded = !expansion.expanded
                    }
                }
            }
            ListItem {
                Label { text: "Standalone ListItem" }
                expansion {
                    height: units.gu(15)
                    content: Rectangle {
                        color: UbuntuColors.darkGrey
                        width: parent.width
                        height: units.gu(8)
                        radius: units.gu(1)
                    }
                }
                onPressAndHold: expansion.expanded = !expansion.expanded;
            }
            ListItem {
                Label { text: "Other Standalone ListItem" }
                expansion {
                    height: units.gu(15)
                    overlapListItem: true
                }
                onPressAndHold: expansion.expanded = !expansion.expanded;
            }

            Button {
                text: "Set ListView.ViewItems.expandedIndices"
                onClicked: listView.ViewItems.expandedIndices = [0, 1, 2]
            }
        }
    }
}
