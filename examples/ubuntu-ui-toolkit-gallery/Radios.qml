/*
 * Copyright 2016 Canonical Ltd.
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
    objectName: "radiosTemplate"

    TemplateSection {
        title: "Radio"
        className: "Radio"

        Column {

            Row {
                height: Math.max(radio1.height, label1.height) + units.gu(1)
                spacing: units.gu(1)
                RadioButton {
                    id: radio1
                    // exclusiveGroup: mygroup
                    anchors.verticalCenter: parent.verticalCenter
                    checked: true
                }
                Label {
                    id: label1
                    text: 'First choice'
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                height: Math.max(radio2.height, label2.height) + units.gu(1)
                spacing: units.gu(1)
                RadioButton {
                    id: radio2
                    // exclusiveGroup: mygroup
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: label2
                    text: 'Second choice'
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                height: Math.max(radio3.height, label3.height) + units.gu(1)
                spacing: units.gu(1)
                RadioButton {
                    id: radio3
                    // exclusiveGroup: mygroup
                    enabled: false
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: label3
                    text: 'Third choice'
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                height: Math.max(radio4.height, label4.height) + units.gu(1)
                spacing: units.gu(1)
                RadioButton {
                    id: radio4
                    // exclusiveGroup: mygroup
                    enabled: false
                    checked: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: label4
                    text: 'Fourth choice'
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
