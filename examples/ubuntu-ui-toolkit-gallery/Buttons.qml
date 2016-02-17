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
            title: i18n.tr("Standard")

            Button {
                objectName: "button_fill_neutral"
                text: i18n.tr("Neutral")
            }
            Button {
                objectName: "button_fill_positive"
                text: i18n.tr("Positive")
                emphasis: Button.Positive
            }
            Button {
                objectName: "button_fill_negative"
                text: i18n.tr("Negative")
                emphasis: Button.Negative
            }
            Button {
                objectName: "button_fill_disabled"
                text: i18n.tr("Disabled")
                enabled: false
            }
        }

        TemplateFlow {
            title: i18n.tr("Text")
            Button {
                objectName: "button_text_neutral"
                text: i18n.tr("Neutral")
                type: Button.Text
            }
            Button {
                objectName: "button_text_positive"
                text: i18n.tr("Positive")
                type: Button.Text
                emphasis: Button.Positive
            }
            Button {
                objectName: "button_text_negative"
                text: i18n.tr("Negative")
                type: Button.Text
                emphasis: Button.Negative
            }
            Button {
                objectName: "button_text_disabled"
                text: i18n.tr("Disabled")
                type: Button.Text
                emphasis: Button.Negative
                enabled: false
            }
        }

        TemplateFlow {
            title: i18n.tr("Outline")
            Button {
                objectName: "button_stroke"
                text: i18n.tr("Neutral")
                type: Button.Outline
            }
            Button {
                objectName: "button_stroke_positive"
                text: i18n.tr("Positive")
                type: Button.Outline
                emphasis: Button.Positive
            }
            Button {
                objectName: "button_stroke_negative"
                text: i18n.tr("Negative")
                type: Button.Outline
                emphasis: Button.Negative
            }
            Button {
                objectName: "button_stroke_disabled"
                text: i18n.tr("Disabled")
                type: Button.Outline
                emphasis: Button.Negative
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
                    fillColor: (
                        buttonColor.action.flipped? "#3EB34F" : "#19B6EE"
                    )
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Icon")
            Button {
                objectName: "button_icon"
                emphasis: Button.Positive
                iconName: "call-start"
                StyleHints {
                    minimumWidth: 0
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Icon+Text")
            Button {
                objectName: "button_icon_right_text"
                text: i18n.tr("Delete")
                emphasis: Button.Negative
                iconName: "delete"
                type: Button.Text
                StyleHints {
                    minimumWidth: 0
                }
            }
            Button {
                objectName: "button_icon_left_text"
                text: i18n.tr("Call")
                iconName: "call-start"
                emphasis: Button.Positive
                iconPosition: Button.After
                StyleHints {
                    minimumWidth: 0
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Disabled")

            Button {
                objectName: "button_text_disabled"
                text: i18n.tr("Call")
                enabled: false
            }
        }
    }

    TemplateSection {
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
