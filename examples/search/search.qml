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
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1

/*!
    \brief In-app search.
*/

MainView {
    id: mainView
    objectName: "mainView"
    applicationName: "search"
    automaticOrientation: true
    width: units.gu(40)
    height: units.gu(70)

    Action {
        id: searchAction
        iconSource: Qt.resolvedUrl("image://gicon/search")
        text: i18n.tr("Search")
        keywords: i18n.tr("Find")
        onTriggered: {
            tabs.selectedTabIndex = 1 // Search
        }
    }
    Action {
        id: quitAction
        text: i18n.tr("Quit")
        keywords: i18n.tr("Close")
        onTriggered: {
            Qt.quit()
        }
    }
    actions: [ quitAction ]

    Tabs {
        id: tabs

    Tab { title: tabInbox.title; Page {
        id: tabInbox
        title: i18n.tr("Inbox")
        actions: [ searchAction ]

        MessageList {
            id: messagesInbox
            model: emails
        }
    } }

    Tab { title: tabSearch.title; Page {
        id: tabSearch
        title: i18n.tr("Search")
        searchLabel: i18n.tr("Search email messages…")
        Component.onCompleted: {
            recentSearches.append({ "query": "ouya" })
            recentSearches.append({ "query": "ubuntu" })
            recentSearches.append({ "query": "retired" })
        }

        onQueryReset: {
            tabs.selectedTabIndex = 0 // Inbox
        }
        onQuerySubmitted: {
            results.clear()
            for (var i = 0; i < emails.count; i++) {
                var canonicalized = keywords.toLowerCase()
                var row = emails.get(i)
                if (row.subject.toLowerCase().indexOf(canonicalized) > -1
                 || row.preview.toLowerCase().indexOf(canonicalized) > -1) {
                    row = JSON.parse(JSON.stringify(row)) // don't modify the original
                    row.subject = row.subject.replace(new RegExp(keywords, "gi"), "<b>$&</b>")
                    row.preview = row.preview.replace(new RegExp(keywords, "gi"), "<b>$&</b>")
                    results.append(row)
                }
            }
        }

        MessageList {
            id: messagesSearch
            model: results
        }
    } }

    Tab { title: tabSettings.title; Page {
        id: tabSettings
        title: i18n.tr("Settings")
        Button {
            text: i18n.tr("No settings whatsoever")
            width: units.gu(20)
            height: units.gu(20)
        }
    } }

    }

    ListModel {
        id: results
    }

    ListModel {
        id: emails
        ListElement {
            subject: "Payment receipt #490524"
            preview: "Habitually we avoid making it too obvious what you paid to ensure you don't remember how much money you pay just for geek stuff."
            when: "2 days ago"
            starred: false
        }
        ListElement {
            subject: "Ubuntu Phone coming soon"
            preview: "Brave humans can already try it on their Android phone and even develop apps for it right now."
            when: "Tomorrow"
            starred: false
        }
        ListElement {
            subject: "The pope retired"
            preview: "In his own words “I'm too young to stay forever in one place”"
            when: "Last Month"
            starred: false
        }
        ListElement {
            subject: "OUYA now in store shelves"
            preview: "Lorem ipsum dolor sit OUYA, quo vadis"
            when: "Yesterday"
            starred: true
        }
        ListElement {
            subject: "Canabalt HD released"
            preview: "Lorem ipsum even the pope dolor sit OUYA quo vadis Ubuntu"
            when: "Eons ago"
            starred: true
        }
    }
}
