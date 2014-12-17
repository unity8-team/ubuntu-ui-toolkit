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

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)
    useDeprecatedToolbar: false

    property bool liveDrag: true

    Page {
        title: "Dragging test"
        ListView {
            anchors.fill: parent
            ListItem.dragMode: true
            ListItem.selectMode: ListItem.dragMode
            contentItem.objectName: "ListViewContent"
            moveDisplaced: Transition {
                id: displacedTransition
                NumberAnimation {
                    id: moveTransition;
                    properties: "y";
                    duration: UbuntuAnimation.FastDuration;
                    easing: UbuntuAnimation.StandardEasing
                }
            }

//            ListItem.onDraggingStarted: {
//                if (event.from < 3) {
//                    // do not drag first 3 items
//                    event.accept = false;
//                } else if (event.from >= 3 && event.from <= 10) {
//                    // live drag between index 3 and 10
//                    event.minimumIndex = 3;
//                    event.maximumIndex = 10;
//                    main.liveDrag = true
//                } else {
//                    // drop reorder after index 11, and forbid dragging those beyond 11
//                    event.minimumIndex = 11;
//                    main.liveDrag = false;
//                }
//            }

            ListItem.onDraggingUpdated: {
                if (main.liveDrag || event.direction == ListItemDrag.None) {
                    // last drag, or live drag, drop it
                    model.move(event.from, event.to, 1);
                } else {
                    event.accept = false;
                }
            }

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 25; i++) {
                        append({label: "List item #"+i})
                    }
                }
            }

            delegate: ListItem {
                objectName: "ListItem-" + index
                color: dragging ? "blue" : "transparent"

                leadingActions: ListItemActions {
                    actions: Action {
                        iconName: "delete"
                    }
                }

                Label {
                    text: modelData
                }

                onPressAndHold: {
                    print("entering/leaving draggable mode")
                    ListView.view.ListItem.dragMode = !ListView.view.ListItem.dragMode;
                }
            }
        }
    }
}
