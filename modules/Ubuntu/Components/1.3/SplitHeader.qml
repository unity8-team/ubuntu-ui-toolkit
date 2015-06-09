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

/*!
    \qmltype SplitHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief TODO
*/
Item {
    id: splitHeader

    property var widths

    property list<PageHeadConfiguration> configurations

    Row {
        anchors.fill: parent
        Repeater {
            model: widths.length

            StyledItem {
                height: parent.height
                width: splitHeader.widths[index]
                styleName: "PageHeadStyle"

                property PageHeadConfiguration config: splitHeader.configurations[index]

                property color dividerColor: "black"// header.dividerColor
                property color panelColor: "orange"//header.panelColor //"orange" //Qt.lighter(mainView.headerColor, 1.1)
                //        property color foregroundColor: "black"

                property string title: "yeah"//rightPage && rightPage.hasOwnProperty("title") ?
                //rightPage.title : ""

                property Item contents: null


                Rectangle {
                    visible: index > 0
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: units.dp(1)
                    color: "black" //header.dividerColor
                }
            }
        }
    }
}
