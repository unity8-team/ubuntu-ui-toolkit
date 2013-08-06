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
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    TemplateSection {
        className: "Button"

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                objectName: "button_text"
                text: i18n.tr("Call")
            }
        }

        TemplateRow {
            title: i18n.tr("Color")

            Button {
                objectName: "button_color"
                text: i18n.tr("Call")
                gradient: UbuntuColors.greyGradient
            }
        }

        TemplateRow {
            title: i18n.tr("Icon")

            Button {
                objectName: "button_iconsource"
                iconSource: "call.png"
            }

            Button {
                objectName: "button_iconsource_right_text"
                width: units.gu(15)
                text: i18n.tr("Call")
                iconSource: "call.png"
                iconPosition: "right"
            }

            Button {
                objectName: "button_iconsource_left_text"
                width: units.gu(15)
                text: i18n.tr("Call")
                iconSource: "call.png"
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Button {
                objectName: "button_text_disabled"
                text: i18n.tr("Call")
                enabled: false
            }
        }

        TemplateRow {
            title: i18n.tr("Standard ComboButton")

            ComboButton {
                objectName: "combo_button_standard"
                text: i18n.tr("Buy this App")
                secondaryActions: [
                    i18n.tr("Add to Wishlist"),
                    i18n.tr("Remind me later"),
                    i18n.tr("Play the demo"),
                ]
                width: units.gu(30)
            }
        }

        TemplateRow {
            title: i18n.tr("ComboButton with bells and whistles")

            ComboButton {
                objectName: "combo_button_custom"
                id: comboButtonCustom
                text: i18n.tr("Decline")
                secondaryActions: [
                    i18n.tr("Sorry, I missed your call, try now"),
                    i18n.tr("I'm running late, I'm on my way"),
                    i18n.tr("Custom…")
                ]
                delegate: Button {
                // delegate: ListItem.Standard {
                    text: modelData
                    iconSource: Qt.resolvedUrl("call.png")
                    // icon: Qt.resolvedUrl("call.png")
                    gradient: UbuntuColors.greyGradient
                    onClicked: comboButtonCustom.clicked(modelData)
                    width: comboButtonCustom.width
                }
                width: units.gu(30)
            }
        }
    }
}
