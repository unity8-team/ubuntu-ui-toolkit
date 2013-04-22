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

MainView {
    id: gallery

    width: units.gu(100)
    height: units.gu(75)
    objectName: "gallery"

    PageStack {
        id: pageStack
        Component.onCompleted: push(mainPage)

        Page {
            id: mainPage
            title: "Ubuntu UI Toolkit"
            visible: false
            flickable: listView

            Rectangle {
                color: "#e0e0e0"
                anchors.fill: parent

                ListView {
                    id: listView
                    anchors.fill: parent
                    model: widgetsModel
                    delegate: ListItem.Standard {
                        text: model.label
                        enabled: model.source != ""
                        progression: true
                        selected: enabled && contentPage.source == Qt.resolvedUrl(model.source)
                        onClicked: {
                            contentPage.title = model.label;
                            contentPage.source = model.source;
                            pageStack.push(contentPage);
                        }
                    }
                }
            }
        }

        Page {
            id: contentPage
            visible: false
            property alias source: contentLoader.source
            onActiveChanged: if (!active) source = ""

            ToolbarActions {
                id: noActions
            }
            tools: contentLoader.item && contentLoader.item.tools ? contentLoader.item.tools : noActions
            flickable: contentLoader.item ? contentLoader.item.flickable : null

            Loader {
                id: contentLoader
                anchors.fill: parent
            }
        }
    }

    WidgetsModel {
        id: widgetsModel
    }
}
