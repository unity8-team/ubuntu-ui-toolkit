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
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1
import Ubuntu.Components.ListItems 1.0
import QtQml.Models 2.1

Item {
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
    ListView {
        onEnabledChanged: print("enabled", enabled)
        anchors.fill: parent

        model: ListModel {
            Component.onCompleted: {
                for (var i = 0; i < 3; i++) {
                    append({label: "List item #"+i, sectionData: "Locked"});
                }
                for (i = 3; i < 11; i++) {
                    append({label: "List item #"+i, sectionData: "Limited, live move"});
                }
                for (i = 11; i < 25; i++) {
                    append({label: "List item #"+i, sectionData: "Unlimited, drag'n'drop"});
                }
            }
        }

        section {
            property: "sectionData"
            criteria: ViewSection.FullString
            delegate: Header {
                text: section
            }
        }

        ViewItems.expandedIndices: [1]
        ViewItems.onExpandedIndicesChanged: print(ViewItems.expandedIndices)

        delegate: ListItem {
            id: item
            objectName: "ListItem-" + index
            leadingActions: ListItemActions {
                actions: deleteAction
            }
            trailingActions: ListItemActions {
                actions: contextualActions
            }
            expansion.height: units.gu(20)

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

            onClicked: expansion.expanded = !expansion.expanded
        }
    }
}
