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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

Template {
    objectName: "buttonsTemplate"

    TemplateSection {
        className: "Button"

        TemplateFlow {
            title: i18n.tr("Types")

            Button {
                objectName: "button_neutral"
                text: i18n.tr("Normal")
            }
            Button {
                objectName: "button_positive"
                text: i18n.tr("Positive")
                type: Button.Positive
            }
            Button {
                objectName: "button_negative"
                text: i18n.tr("Negative")
                type: Button.Negative
            }
            Button {
                objectName: "button_outline"
                text: i18n.tr("Outline")
                type: Button.Outline
            }
        }

        TemplateFlow {
            title: "Disabled"

            Button {
                objectName: "button_normal_disabled"
                text: i18n.tr("Neutral")
                enabled: false
            }
            Button {
                objectName: "button_positive_disabled"
                text: i18n.tr("Positive")
                type: Button.Positive
                enabled: false
            }
            Button {
                objectName: "button_negative_disabled"
                text: i18n.tr("Negative")
                type: Button.Negative
                enabled: false
            }
            Button {
                objectName: "button_outline_disabled"
                text: i18n.tr("Outline")
                type: Button.Outline
                enabled: false
            }
        }

        TemplateFlow {
            title: i18n.tr("Color")
            Button {
                id: buttonColor
                objectName: "button_color"
                action: Action {
                    text: i18n.tr("Switch (%1)").arg(shortcut)
                    shortcut: 'Ctrl+C'
                    property bool flipped
                    onTriggered: flipped = !flipped
                }
                StyleHints {
                    textColor: "white"
                    color: (
                        buttonColor.action.flipped? "#3EB34F" : "#19B6EE"
                    )
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Icon")
            Button {
                objectName: "button_icon"
                type: Button.Positive
                iconName: "call-start"
                StyleHints {
                    minimumWidth: 0
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Icon + Text")
            Button {
                objectName: "button_icon_right_text"
                text: i18n.tr("Delete")
                type: Button.Negative
                iconName: "delete"
                StyleHints {
                    minimumWidth: 0
                }
            }
            Button {
                objectName: "button_icon_left_text"
                text: i18n.tr("Call")
                iconName: "call-start"
                type: Button.Positive
                iconPosition: Button.After
                StyleHints {
                    minimumWidth: 0
                }
            }
        }
    }

    TemplateSection {
        className: "TextButton"

        TemplateFlow {
            title: i18n.tr("Text")
            TextButton {
                objectName: "textbutton_normal"
                text: i18n.tr("Normal")
            }
            TextButton {
                objectName: "textbutton_strong"
                text: i18n.tr("Strong")
                strong: true
            }
        }

        TemplateFlow {
            TextButton {
                objectName: "textbutton_normal_disabled"
                text: i18n.tr("Normal")
                enabled: false
            }
            TextButton {
                objectName: "textbutton_strong_disabled"
                text: i18n.tr("Strong")
                strong: true
                enabled: false
            }
        }
    }

    TemplateSection {
        visible: false
        className: "ComboButton"

        TemplateFlow {
            title: i18n.tr("Collapsed")
            ComboButton {
                text: "Press me"
                objectName: "combobutton_collapsed"
                width: parent.width < units.gu(30)? parent.width : units.gu(30)
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Icon")
            ComboButton {
                iconSource: "call.png"
                width: parent.width < units.gu(30)? parent.width : units.gu(30)
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }
        TemplateFlow {
            title: i18n.tr("Icon+Text")
            ComboButton {
                text: "Answer"
                iconSource: "call.png"
                width: parent.width < units.gu(30)? parent.width : units.gu(30)
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }
        TemplateFlow {
            title: i18n.tr("Expanded")
            ComboButton {
                text: "Press me"
                objectName: "combobutton_expanded"
                expanded: true
                width: parent.width < units.gu(30)? parent.width : units.gu(30)
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }
    }
}
