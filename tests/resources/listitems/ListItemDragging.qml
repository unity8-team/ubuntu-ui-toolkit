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
    width: units.gu(50)
    height: units.gu(50)
    useDeprecatedToolbar: false

    Page {
        title: "Dragging test"
        ListView {
            anchors.fill: parent
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

            ListItem.onDraggingStarted: print("DRAG started")
            ListItem.onDraggingUpdated: {
                print("DRAG updated")
                model.move(drag.from, drag.to, 1);
            }

//            MouseArea {
//                anchors {
//                    top: parent.top
//                    bottom: parent.bottom
//                    right: parent.right
//                }
//                width: units.gu(5)
//                enabled: parent.ListItem.dragMode
//                onPressed: parent.interactive = false
//                onReleased: parent.interactive = true
//                onMouseYChanged: print("INDEX=", parent.indexAt(mouseX, mouseY + parent.contentY - parent.originY))
//            }

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
                Rectangle {
                    anchors.fill: parent
                    color: "tan"
                }

                leadingActions: ListItemActions {
                    actions: Action {
                        iconName: "delete"
                    }
                }

                Label {
                    text: modelData
                }
                Button {
                    text: "Press button which is long enough"
                    anchors.centerIn: parent
                }

                onPressAndHold: {
                    print("entering draggable mode")
                    ListView.view.ListItem.dragMode = true;
                }
                onDraggingChanged: print("dragging", dragging)
            }
        }
    }
}


//Rectangle {
//    width: units.gu(70);  height: units.gu(50)
//    color: "#222222"

//    Component {
//        id: cellDelegate
//        Rectangle { border.width: 1
//            id: main
//            width: grid.cellWidth; height: grid.cellHeight
//            Icon {
//                id: item; parent: loc
//                x: main.x + 5; y: main.y + 5
//                width: main.width - 10; height: main.height - 10;
//                name: icon
//                Rectangle {
//                    anchors.fill: parent;
//                    border.color: "#326487"; border.width: 6
//                    color: "transparent"; radius: 5
//                    visible: item.state == "active"
//                }
////                Behavior on x { enabled: item.state != "active"; NumberAnimation { duration: 400; easing.type: Easing.OutBack } }
////                Behavior on y { enabled: item.state != "active"; NumberAnimation { duration: 400; easing.type: Easing.OutBack } }
//                SequentialAnimation on rotation {
//                    NumberAnimation { to:  2; duration: 60 }
//                    NumberAnimation { to: -2; duration: 120 }
//                    NumberAnimation { to:  0; duration: 60 }
//                    running: loc.currentId != -1 && item.state != "active"
//                    loops: Animation.Infinite; alwaysRunToEnd: true
//                }
//                states: State {
//                    name: "active"; when: loc.currentId == gridId
//                    PropertyChanges { target: item; x: loc.mouseX - width/2; y: loc.mouseY - height/2; scale: 0.5; z: 10 }
//                }
//                transitions: Transition { NumberAnimation { property: "scale"; duration: 200} }
//            }
//        }
//    }


//    GridView {
//        id: grid
//        interactive: false
//        anchors {
//            topMargin: 60; bottomMargin: 60
//            leftMargin: 140; rightMargin: 140
//            fill: parent
//        }
//        cellWidth: units.gu(18); cellHeight: units.gu(18);
//        model: ListModel {
//            id: icons
//            ListElement { icon: "active-call"; gridId: 0}
//            ListElement { icon: "add-to-call"; gridId: 1}
//            ListElement { icon: "add-to-playlist"; gridId: 2}
//            ListElement { icon: "alarm-clock"; gridId: 3}
//            ListElement { icon: "appointment-new"; gridId: 4}
//            ListElement { icon: "appointment"; gridId: 5}
//            ListElement { icon: "attachment"; gridId: 6}
//            ListElement { icon: "contact-group"; gridId: 7}
//            ListElement { icon: "insert-image"; gridId: 8}
//        }
//        delegate: cellDelegate
//        MouseArea {
//            property int currentId: -1                       // Original position in model
//            property int newIndex                            // Current Position in model
//            property int index: grid.indexAt(mouseX, mouseY) // Item underneath cursor
//            id: loc
//            anchors.fill: parent
//            onPressAndHold: currentId = icons.get(newIndex = index).gridId
//            onReleased: currentId = -1
//            onPositionChanged:
//                if (loc.currentId != -1 && index != -1 && index != newIndex)
//                    icons.move(newIndex, newIndex = index, 1)
//        }
//    }
//}
