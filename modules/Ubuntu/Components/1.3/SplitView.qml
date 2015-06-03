/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

// TODO TIM: DOCS
PageTreeNode {
    id: splitView
    anchors.fill: parent

    // TODO TIM: automatically put the first child left,
    //  the second child right. No more children currently supported.
    property Page leftContents: null
    property Page rightContents: null
    PageTreeNode {
        id: leftItem
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: units.gu(40)
//        Rectangle {
//            anchors.fill: parent
//            color: "blue"
//            opacity: 0.1
//        }
        Binding {
            target: leftContents
            when: leftContents
            property: "parent"
            value: leftItem
        }
    }
    Item {
        id: rightItem
        anchors {
            left: leftItem.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
//        Rectangle {
//            id: rightRect
//            anchors.fill: parent
//            color: "red"
////            opacity: 0.1
//            border.width: units.gu(1)
//            border.color: "black"
//        }
        Binding {
            target: rightContents
            when: rightContents
            property: "parent"
            value: rightItem
        }
    }
    Rectangle {
        anchors {
            left: leftItem.right
            top: parent.top
            bottom: parent.bottom
        }
        width: units.dp(1)
        color: header.dividerColor
    }
    StyledItem {
        id: rightHeader
        styleName: "PageHeadStyle"
        anchors {
            left: rightItem.left
            top: parent.top
//            bottom: parent.bottom
            right: parent.right
        }
        height: header.height

        property color dividerColor: "yellow" //Qt.darker(mainView.headerColor, 1.1)
        property color panelColor: "orange" //Qt.lighter(mainView.headerColor, 1.1)
        property color foregroundColor: "black"

        property string title: rightContents && rightContents.hasOwnProperty("title") ?
                                   rightContents.title : ""

        property Item contents: null

        property PageHeadConfiguration config: rightContents ? rightContents.head : null
//        property PageHeadConfiguration config: PageHeadConfiguration {

//            property Flickable flickable: null //internal.activePage ? internal.activePage.flickable : null
//            property var pageStack: null //internal.activePage ? internal.activePage.pageStack : null

//            actions: rightContents ? rightContents.actions : null
//        }

    }
}
