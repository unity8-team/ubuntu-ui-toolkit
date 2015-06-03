/*
 * Copyright (C) 2015 Canonical Ltd.
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

MainView {
    id: mainView
    width: units.gu(120)
    height: units.gu(70)

//    Rectangle {
//        anchors.fill: parent
//        color: "white"
//    }

    SplitView {
        leftPage: Page {
            title: "Left page"
            Label {
                anchors.centerIn: parent
                text: "Hello"
            }

            head {
                actions: [
                    Action {
                        iconName: "like"
                        text: "Like"
                    },
                    Action {
                        iconName: "lock"
                        text: "Lock"
                    }
                ]
                sections.actions: [
                    Action {
                        text: "first"
                    },
                    Action {
                        text: "second"
                    },
                    Action {
                        text: "third"
                    }
                ]
            }
        }
        rightPage: Page {
            title: "Right page"
            id: rp

            Label {
                anchors.centerIn: rp
                text: "world!"
            }
            onParentChanged: print("right page parent = "+parent)
            onParentNodeChanged: print("parent node = "+parentNode)
            width: parent ? parent.width : undefined// not parentNode in this case

            head.actions: [
                Action {
                    iconName: "alarm-clock"
                    text: "Tick tock"
                },
                Action {
                    iconName: "appointment"
                    text: "Date"
                },
                Action {
                    iconName: "attachment"
                    text: "Attach"
                },
                Action {
                    iconName: "contact"
                    text: "Contact"
                }
            ]
        }
    }
}
