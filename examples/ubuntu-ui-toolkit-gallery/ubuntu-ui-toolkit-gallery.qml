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
import Ubuntu.Layouts 0.1

MainView {
    id: gallery
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "ubuntu-ui-toolkit-gallery"


//    width: units.gu(120)
    width: units.gu(70)
    height: units.gu(75)

    property bool wideAspect: width >= units.gu(80)
    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true

    //    state: wideAspect ? "wide" : ""
    //    states: [
    //        State {
    //            name: "wide"
    //            PropertyChanges {
    //                target: pageStack
    //                width: units.gu(40)
    //                anchors {
    //                    fill: null
    //                    top: parent.top
    //                    bottom: parent.bottom
    //                }
    //            }
    //            PropertyChanges {
    //                target: contentPage
    //                x: pageStack.width
    //                width: pageStack.parent.width - x
    //                anchors {
    //                    left: undefined
    //                    right: undefined
    //                    bottom: undefined
    //                }
    //                clip: true
    //                visible: true
    //            }
    //        }
    //    ]

    Layouts {
        anchors.fill: parent

        layouts: [
            ConditionalLayout {
                name: "TwoColumns"
                when: gallery.wideAspect

                Page {
                    clip: true
                    Row {
                        anchors.fill: parent
                        Rectangle {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }
                            color: "green"
                            width: units.gu(40)
                            ItemLayout {
                                anchors.fill: parent
                                item: "widgetList"
                            }
                        }
                        Rectangle {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }
                            color: "pink"
                            width: parent.width - x
                            ItemLayout {
                                anchors.fill: parent
                                item: "content"
                            }
                        }
                    }
                }
            }

        ]

        PageStack {
            id: pageStack
            Component.onCompleted: push(mainPage)
            active: !gallery.wideAspect
            clip: true

            Page {
                id: mainPage
                title: "Ubuntu UI Toolkit"
                visible: false
//                active: false
                Component.onCompleted: print("mainPage.active initially = "+active)
                onActiveChanged: print("mainPage.active = "+active)
                //                flickable: widgetList
//                flickable: gallery.wideAspect ? null : widgetList
                flickable: null
                clip: true

                    ListView {
                        id: widgetList
                        objectName: "widgetList"
                        Layouts.item: "widgetList"
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
                                                            if (!wideAspect) {
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
//                onActiveChanged: if (!active) source = ""
//                ToolbarItems{ id: defTools}
//                tools: contentLoader.item && contentLoader.item.tools ? contentLoader.item.tools : defTools
//                flickable: contentLoader.item && !wideAspect ? contentLoader.item.flickable : null
                flickable: null

                Loader {
                    id: contentLoader
                    objectName: "contentLoader"
                    anchors.fill: parent
                    Layouts.item: "content"
                }
            }
        }
    }
    WidgetsModel {
        id: widgetsModel
    }
}
