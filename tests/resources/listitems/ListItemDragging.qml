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
import Ubuntu.Components.ListItems 1.0 as ListItems

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)
    useDeprecatedToolbar: false

    property bool liveDrag: true
    property bool restrictOnStart: true

    Action {
        id: deleteAction
        iconName: "delete"
    }

    property list<Action> trailingActionList: [
        Action {
            iconName: "edit"
        },
        Action {
            iconName: "camcorder"
        },
        Action {
            iconName: "stock_website"
        }
    ]

    Page {
        title: "Dragging test"

        ListView {
            anchors.fill: parent
//            ViewItems.selectMode: ViewItems.dragMode
            contentItem.objectName: "ListViewContent"
            moveDisplaced: Transition {
                UbuntuNumberAnimation {
                    properties: "y";
                }
            }

            ViewItems.onDraggingStarted: {
                if (!restrictOnStart) {
                    return;
                }

                if (event.from < 3) {
                    // do not drag first 3 items
                    event.accept = false;
                } else if (event.from >= 3 && event.from <= 10) {
                    // live drag between index 3 and 10
                    event.minimumIndex = 3;
                    event.maximumIndex = 10;
                    main.liveDrag = true
                } else {
                    // drop reorder after index 11, and forbid dragging those beyond 11
                    event.minimumIndex = 11;
                    main.liveDrag = false;
                }
            }

            ViewItems.onDraggingUpdated: {
                if (main.liveDrag || event.direction == ListItemDrag.None) {
                    // last drag, or live drag, drop it
                    print("MOVE", event.from, event.to)
                    model.move(event.from, event.to, 1);
                } else {
                    event.accept = false;
                    print("SKIP", event.toIndex);
                }
            }

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 3; i++) {
                        append({label: "List item #"+i, sectionData: "Locked"});
                    }
                    for (i = 3; i < 11; i++) {
                        append({label: "List item #"+i, sectionData: "Limited"});
                    }
                    for (i = 11; i < 25; i++) {
                        append({label: "List item #"+i, sectionData: "Unlimited"});
                    }
                }
            }

            section {
                property: "sectionData"
                criteria: ViewSection.FullString
                delegate: ListItems.Header {
                    text: section
                }
            }

            delegate: ListItem {
                id: item
                objectName: "ListItem-" + index
                color: dragging ? "#30BBBBBB" : "transparent"

                leadingActions: ListItemActions {
                    actions: deleteAction
                }
                trailingActions: ListItemActions {
                    actions: trailingActionList
                }

                Rectangle {
                    anchors.fill: parent
                    color: "#69aa69"
                }
                Label {
                    text: label + ", now @ index " + index
                }

                onPressAndHold: {
                    print("entering/leaving draggable mode")
                    ListView.view.ViewItems.dragMode = !ListView.view.ViewItems.dragMode;
                }
            }
        }
    }
}
