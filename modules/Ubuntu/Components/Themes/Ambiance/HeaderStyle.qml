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

Item {
    id: headerStyle
    /*!
      The height of the headercontents, which is the full height of
      the header minus the separators shown at the bottom of it.
     */
    property real contentHeight: units.gu(7.5)

    /*!
      The source of the image that separates the header from the contents of a \l MainView.
      The separator will be drawn over the contents.
     */
    property url separatorSource: "artwork/PageHeaderBaseDividerLight.sci"

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: "artwork/PageHeaderBaseDividerBottom.png"

    property int fontWeight: Font.Light
    property string fontSize: "x-large"
    property color textColor: Theme.palette.selected.backgroundText
    property real textLeftMargin: units.gu(2)

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

    BorderImage {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorBottomSource
    }

    Item {
        id: backContainer
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: styledItem.tools && styledItem.tools.back && styledItem.tools.back.visible ? units.gu(5) : 0

        Binding {
            target: styledItem.tools.back
            property: "parent"
            value: backContainer
            when: styledItem.tools && styledItem.tools.back
        }
    }

    Item {
        id: foreground
        anchors {
            left: backContainer.right
//            right: parent.right
            right: toolsContainer.left
            top: parent.top
        }
        height: headerStyle.contentHeight

        Label {
            LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: headerStyle.textLeftMargin
            }
            text: styledItem.title
            font.weight: headerStyle.fontWeight
            visible: !styledItem.contents
            fontSize: headerStyle.fontSize
            color: headerStyle.textColor
        }

        Binding {
            target: styledItem.contents
            property: "anchors.fill"
            value: foreground
            when: styledItem.contents
        }
        Binding {
            target: styledItem.contents
            property: "parent"
            value: foreground
            when: styledItem.contents
        }
    }

    Item {
        id: toolsContainer
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
//        width: styledItem.tools ? styledItem.tools.children.length * units.gu(6) : 0
//        width: 200
        width: childrenRect.width
        onWidthChanged: print("toolsContainer.width = "+toolsContainer.width)

        Rectangle {
            anchors.fill: parent
            color: "red"
            visible: false
        }

        Binding {
            target: styledItem.tools
            property: "parent"
            value: toolsContainer
            when: styledItem.tools
        }
    }
}
