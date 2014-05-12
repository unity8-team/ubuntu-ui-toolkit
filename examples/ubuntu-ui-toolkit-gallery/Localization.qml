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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    objectName: "localizationTemplate"
    property string exampleText: "Hello world"
    property var locale: Qt.locale()
    property bool forceLocales: false
    property var customLocales: [ "en_US", "de_DE", "es_ES", "es_US", "fr_FR", "fi_FI", "fa_IR", "nl_NL" ]

    // Values are strings to allow functions
    property var demoStrings: ListModel {
        ListElement { a: "i18n.language + ', ' + locale.name + ', ' + locale.nativeLanguageName"; b: "Locale" }
        ListElement { a: "i18n.tr(locale.textDirection ? 'RTL' : 'LTR')"; b: "Text direction" }
        ListElement { a: "exampleText"; b: "Untranslated" }
        ListElement { a: "i18n.tr(exampleText)"; b: "Translated" }
        ListElement { a: "i18n.dtr('ubuntu-ui-toolkit', 'Back')"; b: "Different domain" }
        ListElement { a: "new Date().toLocaleDateString(locale)"; b: "Localized Date" }
        ListElement { a: "new Date().toLocaleTimeString(locale)"; b: "Localized Time" }
        ListElement { a: "locale.standaloneDayName(locale.firstDayOfWeek)"; b: "First Day of the Week" }
        ListElement { a: "(6544323.64).toLocaleString(locale)"; b: "Localized Number" }
    }

    TemplateSection {
        title: i18n.tr("Changes")
        className: "Changes"

        Row {
            spacing: units.gu(1)

            OptionSelector {
                width: units.gu(20)
                expanded: true
                height: units.gu(15)
                model: forceLocales ? customLocales : Qt.locale().uiLanguages
                delegate: OptionSelectorDelegate {
                    text: Qt.locale(modelData).nativeLanguageName
                    subText: modelData
                    onClicked: {
                        i18n.language = modelData
                        // FIXME: language only doesn't trigger reload
                        i18n.domain = i18n.domain
                    }
                }
            }

            Column {
                Label {
                    text: i18n.tr("Custom locale names")
                }

                Switch {
                    id: force
                    onCheckedChanged: forceLocales = checked
                }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("Translations")
        className: "Translations"

        UbuntuListView {
            id: demoList
            width: units.gu(40)
            height: units.gu(40)
            model: demoStrings
            delegate: ListItem.Subtitled {
                text: eval(a)
                subText: i18n.tr(b)
                objectName: b.replace(' ', '_')
            }
        }
    }
}
