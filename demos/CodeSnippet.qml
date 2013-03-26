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
import Ubuntu.Components.Popups 0.1

MouseArea {
    id: codeSnippet

    property string code
    property var __popover

    anchors.fill: parent
    hoverEnabled: true
    propagateComposedEvents: true
    onPressed: {PopupUtils.close(codeSnippet.__popover); mouse.accepted = false}
    onEntered: codeSnippet.__popover = PopupUtils.open(popoverComponent, codeSnippet)
    onExited: if (codeSnippet.__popover) PopupUtils.close(codeSnippet.__popover)

    Component {
        id: popoverComponent

        Popover {
            autoClose: false

            Label {
                width: paintedWidth + units.gu(2)
                height: paintedHeight + units.gu(2)
                x: units.gu(1)
                y: units.gu(1)
                text: codeSnippet.code
            }
        }
    }
}



