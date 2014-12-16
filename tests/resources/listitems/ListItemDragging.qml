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
                    duration: UbuntuAnimation.BriskDuration;
                    easing: UbuntuAnimation.StandardEasing
                }
            }

            ListItem.onDraggingStarted: {
                if (drag.from == 0) {
                    drag.directions = DragEvent.Downwards;
                } else if (drag.from == count - 1) {
                    drag.direction = DragEvent.Upwards;
                } else {
                    drag.direction = DragEvent.None;
                    drag.accept = false;
                }
            }

            ListItem.onDraggingUpdated: {
                if (main.liveDrag || drag.direction == DragEvent.None) {
                    // last drag, or live drag, drop it
                    model.move(drag.from, drag.to, 1);
                } else {
                    drag.accept = false;
                }
            }

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 25; i++) {
                        append({label: "List item #"+i})
                    }
                }
            }
//            model: ["a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","x","y","z"]

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
                    print("entering draggable mode")
                    ListView.view.ListItem.dragMode = true;
                }
            }
        }
    }
}
