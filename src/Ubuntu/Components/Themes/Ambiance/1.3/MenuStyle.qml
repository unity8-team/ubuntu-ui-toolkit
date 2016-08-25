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
 *
 * Author: Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems

Item {
    id: root
    property alias contentItem: content

    property color backgroundColor: styledItem.enabled
                                      ? theme.palette.normal.overlay
                                      : theme.palette.disabled.overlay

    property bool square: false
    property real minimumWidth: units.gu(20)

    property real maximumWidth: styledItem.__defaultMaximumWidth

    // this is the minimnum height a context menu can be before it adds scrolling
    property real minimumHeight: units.gu(30)

    property real maximumHeight: styledItem.__defaultMaximumHeight

    property url chevron_up: Qt.resolvedUrl("../artwork/chevron_up.png")

    property url chevron_down: Qt.resolvedUrl("../artwork/chevron_down.png")

    property Component headerDelegate: __defaultHeaderDelegate

    property Component footerDelegate: __defaultFooterDelegate

    property Component highlightDelegate: __defaultHighlight

    property Component menuItemDelegate: __defaultMenuItemDelegate

    UbuntuShape {
        id: shape
        anchors.fill: parent
        aspect: UbuntuShape.Flat
        backgroundColor: root.backgroundColor
        source: shapeSource
        visible: !square
    }

    ShaderEffectSource {
        id: shapeSource
        sourceItem: content
        hideSource: !square
        // FIXME: visible: false prevents rendering so make it a nearly
        // transparent 1x1 pixel instead
        opacity: 0.01
        width: 1
        height: 1
    }

    BorderImage {
        id: shadow
        anchors.fill: parent
        anchors.margins: -units.gu(1)
        source: Qt.resolvedUrl("../artwork/header_overflow_dropshadow.sci")
    }

    Item {
        id: content
        objectName: "content"
        anchors.fill: parent

        Rectangle {
            id: colorRect
            anchors.fill: parent
            color: root.backgroundColor
            visible: square
        }
    }


    Component {
        id: __defaultHeaderDelegate
        Item {
            id: header

            height: units.gu(3)

            Rectangle {
                color: enabled ? theme.palette.normal.overlayText :
                    theme.palette.disabled.overlayText
                height: units.dp(1)
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
            }

            Icon {
                anchors.centerIn: parent
                width: units.gu(2)
                height: units.gu(2)
                source: chevron_up
                color: enabled ? theme.palette.normal.overlayText :
                                 theme.palette.disabled.overlayText
            }
        }
    }

    Component {
        id: __defaultFooterDelegate
        Item {
            id: footer

            height: units.gu(3)

            Rectangle {
                color: enabled ? theme.palette.normal.overlayText :
                                 theme.palette.disabled.overlayText
                height: units.dp(1)
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
            }

            Icon {
                anchors.centerIn: parent
                width: units.gu(2)
                height: units.gu(2)
                source: chevron_down
                color: enabled ? theme.palette.normal.overlayText :
                                 theme.palette.disabled.overlayText
            }
        }
    }

    Component {
        id: __defaultHighlight
        Rectangle {
            color: "transparent"
            border.width: units.dp(1)
            border.color: UbuntuColors.orange
        }
    }

    Component {
        id: __defaultMenuItemDelegate
        Rectangle {
            color: "transparent"
            border.width: units.dp(1)
            border.color: UbuntuColors.orange
        }
    }
}
