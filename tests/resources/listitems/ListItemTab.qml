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

import QtQuick 2.3
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0
import QtQuick.Layouts 1.1

Tab {
    title: "Highlight policy"
    ListItemActions {
        id: sharedLeading
        actions: Action {
            iconName: "delete"
            text: "Delete"
        }
    }
    ListItemActions {
        id: sharedTrailing
        actions: [
            Action {
                iconName: "search"
                text: "Search"
            },
            Action {
                iconName: "edit"
                text: "Edit"
            },
            Action {
                iconName: "email"
                text: "E-mail"
            }
        ]
    }

    Action {
        id: defaultAction
        onTriggered: {
            // open a dialog
        }
    }
    page: Page {
        Flickable {
            anchors.fill: parent
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: parent.width

                ListItem {
                    Label {
                        text: "No action, leading/trailing actions, active component added," +
                              " clicked or pressAndHold signal connected."
                        wrapMode: Text.Wrap
                        width: parent.width
                    }
                }
                ListItem {
                    Row {
                        width: parent.width
                        CheckBox {
                            id: check
                        }
                        Label {
                            text: "An active component will allow highligh when not pressed over the active component."
                            wrapMode: Text.Wrap
                            width: parent.width - check.width
                        }
                    }
                }
                ListItem {
                    action: Action {
                    }
                    Label {
                        text: "Default <b>action</b> provides highlight"
                    }
                }
                ListItem {
                    leadingActions: sharedLeading
                    trailingActions: sharedTrailing
                    Label {
                        text: "<b>leadingActions, trailingActions</b> provides highlight."
                    }
                }
            }
        }
    }
}


