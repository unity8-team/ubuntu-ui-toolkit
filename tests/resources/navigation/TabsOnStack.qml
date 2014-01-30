/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 0.1

MainView {
    id: mainView
    width: units.gu(38)
    height: units.gu(50)

    PageStack {
        id: pageStack
        Component.onCompleted: push(page0)
        Page {
            id: page0
            title: "Stack"
            Button {
                anchors.centerIn: parent
                onClicked: pageStack.push(tabsOnStack)
                text: "Press"
            }
        }
        Tabs {
            id: tabsOnStack
            Tab {
                title: "Tab 1"
                page: Page {
                    id: page1
                    Label {
                        anchors.centerIn: parent
                        text: "Press back to return to the previous tabs"
                    }
                }
            }
            Tab {
                title: "Tab 2"
                page: Page {
                    id: page2
                    Label {
                        anchors.centerIn: parent
                        text: "Page 2"
                    }
                }
            }
        }
    }
}
