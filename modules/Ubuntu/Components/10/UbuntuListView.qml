/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.0 as Toolkit

ListView {
    id: root

    property int expandedIndex: -1

    QtObject {
        id: priv

        function positionViewAtIndexAnimated(expandedIndex) {
            animation.from = root.contentY;
            root.currentIndex = expandedIndex;
            if (expandedIndex == root.count - 1) {
                root.positionViewAtIndex(expandedIndex, ListView.End);
            } else {
                root.positionViewAtIndex(expandedIndex + 1, ListView.End);
            }

            var effectiveExpandedHeight = Math.min(root.currentItem.expandedHeight, root.height - root.currentItem.collapsedHeight);
            if (root.contentY - root.originY == 0) {
                if (((root.currentIndex + 1) * root.currentItem.collapsedHeight) + effectiveExpandedHeight > root.height) {
                    animation.to = ((root.currentIndex + 1) * root.currentItem.collapsedHeight + effectiveExpandedHeight) - root.height + root.originY
                } else {
                    animation.to = root.originY
                }
            } else {
                animation.to = root.contentY + (effectiveExpandedHeight - root.currentItem.collapsedHeight);
            }
            animation.start();
        }

        function requestFocus(reason) {
            // lookup for the currentItem, and if it is a FocusScope, focus the view
            // this will also focus the currentItem
            if (root.currentItem && root.currentItem.hasOwnProperty("activeFocusOnPress")) {
                root.forceActiveFocus(reason);
            }
        }
    }

    focus: true

    /*!
      \internal
      Grab focus when moved, flicked or clicked
     */
    onMovementStarted: priv.requestFocus(Qt.MouseFocusReason)
    onFlickStarted: priv.requestFocus(Qt.MouseFocusReason)
    Toolkit.Mouse.onClicked: priv.requestFocus(Qt.MouseFocusReason)

    /*!
      \preliminary
      Expand the item at the given index.
     */
    onExpandedIndexChanged: {
        if (expandedIndex < 0) {
            return;
        }
        priv.positionViewAtIndexAnimated(expandedIndex, ListView.End)
    }

    UbuntuNumberAnimation {
        id: animation
        target: root
        property: "contentY"
    }

    MouseArea {
        parent: contentItem
        anchors.fill: parent
        z: 2
        enabled: root.expandedIndex != -1
        onClicked: root.expandedIndex = -1;
    }
}
