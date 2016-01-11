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
import Ubuntu.Components 1.3

Template {
    objectName: "togglesTemplate"

    // Toggle the `checked` property of the first item found in `content`
    function toggleItem(content) {
        if (content[0] && typeof content[0].checked !== "undefined") {
            content[0].checked = !content[0].checked;
        }
    }

    TemplateSection {

        property real titleColWidth: units.gu(15)

        title: "Checkbox"
        className: "CheckBox"

        TemplateRow {
            title: i18n.tr("Unchecked")
            titleWidth: parent.titleColWidth
            onClicked: toggleItem(content)

            CheckBox {
                objectName: "checkbox_unchecked"
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")
            titleWidth: parent.titleColWidth
            onClicked: toggleItem(content)

            CheckBox {
                objectName: "checkbox_checked"
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Indeterminate")
            titleWidth: parent.titleColWidth
            onClicked: toggleItem(content)

            CheckBox {
                id: checkbox_indeterminate
                objectName: "checkbox_indeterminate"
                indeterminate: true
            }

            Label {
                visible: !checkbox_indeterminate.indeterminate
                text: 'Set to indeterminate'
                color: UbuntuColors.orange
                font.underline: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: checkbox_indeterminate.indeterminate = true
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")
            titleWidth: parent.titleColWidth

            CheckBox {
                objectName: "checkbox_disabled_unchecked"
                enabled: false
            }

            CheckBox {
                objectName: "checkbox_disabled_checked"
                enabled: false
                checked: true
            }
        }
    }


    TemplateSection {
        title: "Switch"
        className: "Switch"

        TemplateRow {
            title: i18n.tr("Unchecked")
            onClicked: toggleItem(content)

            Switch {
                objectName: "switch_unchecked"
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")
            onClicked: toggleItem(content)

            Switch {
                objectName: "switch_checked"
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Switch {
                objectName: "switch_disabled_unchecked"
                enabled: false
            }

            Switch {
                objectName: "switch_disabled_checked"
                enabled: false
                checked: true
            }
        }
    }
}
