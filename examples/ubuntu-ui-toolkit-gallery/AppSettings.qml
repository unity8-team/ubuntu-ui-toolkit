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

Template {
    TemplateSection {
        className: "Settings"

        TemplateRow {
            title: i18n.tr("Boolean")

            Settings {
                group: "setting_boolean"
                Option {
                    id: optionVibrate
                    name: "vibrate"
                    defaultValue: false
                    text: i18n.tr("Vibrate")
                }
            }

            Label {
                text: optionVibrate.value
            }

            Switch {
                action: optionVibrate
                checked: optionVibrate.value
            }
        }

        TemplateRow {
            title: i18n.tr("String")
            Settings {
                group: "settings_string"
                Option {
                    id: optionHomepage
                    name: "homepage"
                    defaultValue: "http://www.duckduckgo.com"
                    text: i18n.tr("Homepage")
                }
            }

            Label {
                text: optionHomepage.value
            }

            TextField {
                action: optionHomepage
                text: optionHomepage.value
            }
        }

        TemplateRow {
            title: i18n.tr("Integer")
            Settings {
                group: "settings_int"
                Option {
                    id: optionYearOfBirth
                    name: "yeahOfBirth"
                    defaultValue: 1975.2
                    text: i18n.tr("Year of Birth")
                }
            }

            Label {
                text: optionYearOfBirth.value
            }

            TextField {
                action: optionYearOfBirth
                text: optionYearOfBirth.value
            }
        }
    }
}
