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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

TextField {
    placeholderText: null
    property Popover visiblePopover: null

    function showSuggestions(searchField) {
        if (visiblePopover)
            return
        visiblePopover = PopupUtils.open(suggestionsComponent, searchField, {
            "contentWidth": searchField.width,
            "edgeMargins": units.gu(1)
        })
    }
    function hideSuggestions() {
        if (!visiblePopover)
            return
        PopupUtils.close(visiblePopover)
        visiblePopover = null
    }
    function searchWhileTyping(searchField) {
        var keywords = searchField.text.replace(/^\s+|\s+$/g, '')
        if (keywords == "") {
            hideSuggestions()
            queryReset()
            return
        }

        if (searchField.activeFocus)
            showSuggestions(searchField)
        queryUpdated(keywords)
    }
    function submitSearch(keywords) {
        hideSuggestions()
        keywords = keywords.replace(/^\s+|\s+$/g, '')
        for (var i = 0; i < recentSearches.count; i++)
            if (recentSearches.get(i).query == keywords) {
                recentSearches.move(i, 0, 1)
                querySubmitted(keywords)
                return
            }
        recentSearches.insert(0, { "query": keywords })
        querySubmitted(keywords)
    }

    Component {
        id: suggestionsComponent
        Popover {
            id: suggestionsPopover
            Column {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                ListItem.Standard {
                    enabled: false
                    text: i18n.tr("Recent Searches")
                }

                Repeater {
                    model: recentSearches
                    delegate: ListItem.Standard {
                        showDivider: index - model.count - 1
                        text: query
                        onClicked: submitSearch(text)
                    }
                }
            }
        }
    }

    id: searchField
    popover: suggestionsComponent
    font.bold: true
    horizontalAlignment: Text.AlignRight
    height: units.gu(7)
    primaryItem: Button {
        width: height
        height: parent.height - units.gu(2)
        onClicked: submitSearch(text)
        color: 'transparent'
        Icon {
            name: "search"
            width: parent.width
            height: parent.height
        }
    }
    onAccepted: submitSearch(text)
    onTextChanged: {
        searchWhileTyping(searchField)
    }
    onActiveFocusChanged: {
        if (!activeFocus)
            hideSuggestions()
    }
}

