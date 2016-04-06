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
    objectName: "textinputsTemplate"

    // http://qt-project.org/doc/qt-5/richtext-html-subset.html
    property string richText: "<big>Lorem ipsum dolor sit amet</big>, <i>consectetur adipiscing elit.</i>" +
                              "<b>Nunc pretium iaculis risus</b>, <small>sed vehicula odio varius ac.</small>" +
                              "<u>Etiam orci lectus, bibendum in vulputate ac, tristique quis dui.</u>"

    TemplateSection {
        title: "Single line"
        className: "TextField"

        TemplateRow {
            title: i18n.tr("Standard")

            TextField {
                objectName: "textfield_standard"
                placeholderText: i18n.tr("Type me in...")
                width: parent.width
            }
        }

        TemplateRow {
            title: i18n.tr("Password")

            TextField {
                objectName: "textfield_password"
                echoMode: TextInput.Password
                text: "password"
                width: parent.width
            }
        }

        TemplateRow {
            title: i18n.tr("Numbers")

            TextField {
                objectName: "textfield_numbers"
                text: "123"
                validator: IntValidator {}
                width: parent.width
            }
        }

        TemplateRow {
            title: i18n.tr("Read-only")

            TextField {
                objectName: "textfield_read_only"
                text: i18n.tr("Nobody type here")
                readOnly: true
                width: parent.width
                horizontalAlignment: TextInput.AlignHCenter
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            TextField {
                objectName: "textfield_disabled"
                text: i18n.tr("No interaction allowed")
                enabled: false
                width: parent.width
                horizontalAlignment: TextInput.AlignRight
            }
        }
    }

    TemplateSection {
        title: "Multi-line"
        className: "TextArea"

        TemplateRow {
            title: i18n.tr("Default")

            TextArea {
                objectName: "textarea_default"
                text: loremIpsum
                width: parent.width
            }
        }

        TemplateRow {
            title: i18n.tr("Expanding")

            TextArea {
                objectName: "textarea_expanding"
                placeholderText: "Expands up to 5 lines"
                autoSize: true
                maximumLineCount: 5
                width: parent.width
            }
        }

        TemplateRow {
            title: i18n.tr("Rich Text")

            TextArea {
                objectName: "textarea_richtext"
                textFormat: TextEdit.RichText
                text: richText
                width: parent.width
            }
        }
    }
}
