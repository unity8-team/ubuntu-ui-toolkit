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
    \qmltype ScrollView
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief ScrollView is a scrollable view that features scrollbars and scrolling via keyboard keys.

    ScrollView is the recommended component to use in the implementation of scrollable content. It
    features scrollbars and handles keyboard input.
    Old applications that implemented scrollable views directly using the \l Scrollbar component should
    migrate to ScrollView, to ensure the UX is ready for convergent devices and is consistent with the
    rest of the platform.

    Adding scrollbars and keyboard input handling to a QML item is as simple as wrapping that item in a
    ScrollView, as shown in the following example:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    ScrollView {
        width: units.gu(40)
        height: units.gu(30)

        Rectangle {
            width: units.gu(140)
            height: units.gu(40)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightsteelblue" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
    }
    \endqml

    NOTE: the items that are wrapped in the ScrollView are reparanted to \l viewport.
*/
Flickable {
    id: root

    implicitWidth: 240
    implicitHeight: 150
    clip: true
    //TODO: do we want to have a default value for this?
    //This would need overriding when the size of the children depends on the size
    //of their parent, otherwise we have a binding loop
    //(child.width->contentItem.width->childrenRect.width->child.width)
    contentWidth: contentItem.childrenRect.width
    contentHeight: contentItem.childrenRect.height

    focus: true
    //TODO: add horizontalScrollbarPolicy
    //TODO: add verticalScrollbarPolicy

    //Currently we want to push the platform towards overlay scrollbars
    //readonly property bool alwaysOnScrollbars: false

    /*! \internal */
    //property alias __horizontalScrollBar: scroller.horizontalScrollBar
    /*! \internal */
    //property alias __verticalScrollBar: scroller.verticalScrollBar

    //shortScrollingRation is used for arrow keys, longScrollingRatio is used for pgUp/pgDown
    //0.1 means we will scroll 10% of the *visible* flickable area
    property real shortScrollingRatio: __styleInstance ? __styleInstance.shortScrollingRatio : 0.1
    property real longScrollingRatio: __styleInstance ? __styleInstance.longScrollingRatio : 0.9
    property real nonOverlayScrollbarMargin: verticalScrollbar.__styleInstance ? verticalScrollbar.__styleInstance.nonOverlayScrollbarMargin : 0

    Keys.enabled: true
    Keys.onLeftPressed: {
        console.log("Left pressed")
        if (horizontalScrollbar.__styleInstance !== null) {
            horizontalScrollbar.__styleInstance.scroll(-root.width*shortScrollingRatio)
        }
    }
    Keys.onRightPressed: {
        console.log("Right pressed")
        if (horizontalScrollbar.__styleInstance !== null) {
            horizontalScrollbar.__styleInstance.scroll(root.width*shortScrollingRatio)
        }
    }
    Keys.onDownPressed: {
        console.log("Down pressed")
        if (verticalScrollbar.__styleInstance !== null) {
            verticalScrollbar.__styleInstance.scroll(root.height*shortScrollingRatio)
        }
    }
    Keys.onUpPressed: {
        console.log("Up pressed")
        if (verticalScrollbar.__styleInstance !== null) {
            verticalScrollbar.__styleInstance.scroll(-root.height*shortScrollingRatio)
        }
    }
    Keys.onPressed:  {
        console.log("Pressed")
        if (event.key == Qt.Key_Escape) {
            var scrollbarWithActiveDrag = (horizontalScrollbar.__styleInstance && horizontalScrollbar.__styleInstance.draggingThumb)
                    || (verticalScrollbar.__styleInstance && verticalScrollbar.__styleInstance.draggingThumb)
                    || null
            if (scrollbarWithActiveDrag !== null) {
                scrollbarWithActiveDrag.__styleInstance.resetScrollingToPreDrag()
            }
            event.accepted = true
        } else if (verticalScrollbar.__styleInstance !== null) {
            if (event.key == Qt.Key_PageDown) {
                verticalScrollbar.__styleInstance.scroll(root.height*longScrollingRatio)
            } else if (event.key === Qt.Key_PageUp) {
                verticalScrollbar.__styleInstance.scroll(-root.height*longScrollingRatio)
            } else if (event.key === Qt.Key_Home) {
                verticalScrollbar.__styleInstance.scrollToBeginning()
            } else if (event.key === Qt.Key_End) {
                verticalScrollbar.__styleInstance.scrollToEnd()
            }
            event.accepted = true
        }
    }


    //replacement for StyledItem's activeFocusOnPress
    //this has to come before Scrollbars, because its parent property is initialized after Scrollbar's
    //(due to the order in which QML instantiates items), so it will end up on top of the scrollbars.
    //And that's what we want, to ensure tapping on the scrollbar also focuses the corresponding scrollview.
    //The alternative is to set "z", or force focus from inside Scrollbar (but why make scrollbar more complex?)
    MouseArea {
        anchors.fill: parent
        parent: root
        onPressed: {
            console.log("FORCING FOCUS ON SCROLLVIEW", root);
            root.forceActiveFocus();
            mouse.accepted = false
        }
    }

    Scrollbar {
        id: horizontalScrollbar
        //we don't want to be flickable's child, because that means we can't interact
        //with the scrollbar when flickable grabs the mouse (i.e. while flicking)
        parent: root
        flickableItem: root
        align: Qt.AlignBottom
        __buddyScrollbar: verticalScrollbar
        __alwaysOnScrollbars: false//alwaysOnScrollbars
        focus: false
    }

    Scrollbar {
        id: verticalScrollbar
        parent: root
        flickableItem: root
        align: Qt.AlignTrailing
        __buddyScrollbar: horizontalScrollbar
        __alwaysOnScrollbars: false//alwaysOnScrollbars
        focus: false
    }

    //DEBUG
    Text {
        anchors.left: parent.left
        z: 100
        color: root.activeFocus ? "red" : "black";
        text:"ROOT focus " + root.focus + " activeFocus " + root.activeFocus;
    }
}
