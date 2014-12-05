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

import QtQuick 2.3
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0
import QtQuick.Layouts 1.1

Tab {
    title: "ListItem actions"
    Action {
        id: deleteAction
        iconName: "delete"
        text: "Delete"
    }
    Action {
        id: searchAction
        iconName: "search"
        text: "Search"
    }
    Action {
        id: editAction
        iconName: "edit"
        text: "Edit"
    }
    Action {
        id: emailAction
        iconName: "email"
        text: "E-mail"
    }
    Action {
        id: homeAction
        iconName: "go-home"
        text: "Home"
    }

    page: Page {
        Flickable {
            anchors.fill: parent
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: parent.width

                Header { text: "leadingActions" }
                ListItem {
                    leadingActions: ListItemActions {
                        actions: [deleteAction]
                    }
                    Label { text: "Swipe to the right to reveal actions" }
                }
                ListItem {
                    leadingActions: ListItemActions {
                        actions: [deleteAction, homeAction]
                    }
                    Label { text: "Swipe to the right to reveal actions" }
                }
                Header { text: "trailingActions" }
                ListItem {
                    trailingActions: ListItemActions {
                        actions: [deleteAction]
                    }
                    Label { text: "Swipe to the left to reveal actions" }
                }
                ListItem {
                    trailingActions: ListItemActions {
                        actions: [deleteAction, homeAction]
                    }
                    Label { text: "Swipe to the left to reveal actions" }
                }
                Header { text: "trailingActions and leadingActions" }
                ListItem {
                    leadingActions: ListItemActions {
                        actions: [deleteAction]
                    }
                    trailingActions: ListItemActions {
                        actions: [searchAction, editAction, emailAction, homeAction]
                    }

                    Label {
                        text: "Swipe to the right to reveal leading actions, to the left to reveal trailing actions"
                        width: parent.width
                        wrapMode: Text.Wrap
                    }
                }
                ListItem {
                    leadingActions: ListItemActions {
                        actions: [deleteAction]
                    }
                    trailingActions: ListItemActions {
                        actions: [searchAction, editAction, emailAction, homeAction]
                        delegate: Column {
                            width: height + units.gu(2.5)
                            Icon {
                                width: units.gu(3)
                                height: width
                                name: action.iconName
                                color: "blue"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Label {
                                text: action.text
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    Label {
                        text: "Swipe to the right to reveal leading actions, to the left to reveal trailing actions with custom delegate"
                        width: parent.width
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }
}
