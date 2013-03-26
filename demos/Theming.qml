/*
 * Copyright 2012 Canonical Ltd.
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

Template {
    objectName: "Theming"
    title: "Theming"

    Column {
        spacing: units.gu(4)

        TemplateRow {
            title: "Custom"
            Switch {
                Component.onCompleted: checked = (Theme.currentTheme.indexOf("custom-theme") > 0)
                onCheckedChanged: {
                    if (checked)
                        Theme.loadTheme(Qt.resolvedUrl("./custom-theme.qmltheme"));
                    else
                        Theme.loadTheme("");
                }
            }
        }

        TemplateRow {
            title: "Button"

            Button {
                id: button
                text: "Call"
                Binding {
                    target: button.ItemStyle.style
                    property: "color"
                    value: "green"
                }

                CodeSnippet {
                    code: "Button {\n"+
                          "    id: button\n"+
                          "    text: \"Call\"\n"+
                          "    Binding {\n"+
                          "        target: button.ItemStyle.style\n"+
                          "        property: \"color\"\n"+
                          "        value: \"green\"\n"+
                          "    }\n"+
                          "}"
                }
            }
        }

        TemplateRow {
            title: "Custom style"

            Button {
                ItemStyle.class: "custom-button"
                text: "Call"

                CodeSnippet {
                    code: "Button {\n"+
                          "    ItemStyle.class: \"custom-button\"\n"+
                          "    text: \"Call\"\n"+
                          "}"
                }
            }
        }

        TemplateRow {
            title: "Custom style"

            Button {
                ItemStyle.name: "custom-name"
                text: "Call"

                CodeSnippet {
                    code: "Button {\n"+
                          "    ItemStyle.name: \"custom-name\"\n"+
                          "    text: \"Call\"\n"+
                          "}"
                }
            }
        }
    }
}
