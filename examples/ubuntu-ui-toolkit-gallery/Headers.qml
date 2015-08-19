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

Template {
    objectName: "headerTemplate"

    property list<Action> actionList:  [
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
        },
        Action {
            iconName: "like"
            text: "Like"
        },
        Action {
            iconName: "lock"
            text: "Lock"
        }
    ]

    property list<Action> shortActionList: [
        Action {
            iconName: "share"
            text: "Share"
        },
        Action {
            iconName: "starred"
            text: "Favorite"
        }
    ]

    TemplateSection {
        title: "Header"
        className: "Header"

        TemplateRow {
            title: i18n.tr("Title")
            Item {
                height: units.gu(10)
                width: parent.width
                Header {
                    title: i18n.tr("Hello, world")
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Back action")
            Item {
                width: parent.width
                height: units.gu(10)
                Header {
                    title: i18n.tr("Button on the left")
                    // TODO TIM: Add leading action
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Navigation panel")
            Item {
                width: parent.width
                height: units.gu(10)
                Header {
                    title: i18n.tr("Overflow on the left")
                    // TODO TIM: Add leadingActions
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Actions")
            Item {
                width: parent.width
                height: units.gu(10)
                Header {
                    title: i18n.tr("Buttons on the right")
                    // TODO TIM: add trailing actions
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Sections")
            Item {
                width: parent.width
                height: units.gu(10)
                Header {
                    title: i18n.tr("Sections under the title")
                    // TODO TIM: sections.actions: ....
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Hiding")
            Item {
                width: parent.width
                height: units.gu(25)
                clip: true
                // TODO TIM: flickable and header.

                Header {
                    z: 1000 // FIXME TIM: Make this automatic
                    title: i18n.tr("Scroll the flickable")
                    flickable: flickable
                }

                Flickable {
                    id: flickable
                    anchors.fill: parent
                    contentHeight: 2*height

                    Rectangle {
                        anchors.fill: parent
                        color: "lightgrey"

                        Label {
                            anchors {
                                top: parent.top
                                topMargin: units.gu(10)
                                horizontalCenter: parent.horizontalCenter
                            }
                            text: "Scroll to show/hide the header."
                        }
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")
            Item {
                width: parent.width
                height: units.gu(10)
                Header {
                    title: i18n.tr("Cannot trigger the actions.")
                    // TODO TIM: add actions.
                }
            }
        }
    }
}
