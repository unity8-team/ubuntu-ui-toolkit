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
//import Ubuntu.Components.ListItems 1.3 as ListItem

MainView {
    id: gallery
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "com.ubuntu.developer.bzoltan.ubuntu-ui-toolkit-gallery"

    width: units.gu(120)
    height: units.gu(75)

    LayoutMirroring.enabled: rtl
    LayoutMirroring.childrenInherit: true
    property bool rtl: Qt.application.layoutDirection == Qt.RightToLeft

    property string loremIpsum:
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " +
                    "Integer et ante at erat malesuada accumsan in eget mauris. " +
                    "Nunc ultrices tristique laoreet. In non tristique lorem. " +
                    "Donec in libero ut libero pretium lacinia. Proin dictum faucibus viverra. " +
                    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. " +
                    "Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. " +
                    "Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. "

    AdaptivePageLayout {
id: layout
        anchors.fill: parent
        primaryPageSource: Qt.resolvedUrl("MainPage.qml")

        layouts: [
            PageColumnsLayout {
                when: layout.width > units.gu(80)
                PageColumn {
                    minimumWidth: units.gu(30)
                    maximumWidth: units.gu(50)
                    preferredWidth: units.gu(40)
                }
                PageColumn {
                    fillWidth: true
                }
            },
            // configure single column mode so we can only size it to minimum 20 GU
            PageColumnsLayout {
                when: true
                PageColumn {
                    minimumWidth: units.gu(20)
                    fillWidth: true
                }
            }
        ]
    }
}
