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

/*!
    \qmltype Expandable
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief An expandable list item with no contents.
    The Expandable class can be used for generic list items containing other
    components such as buttons. It subclasses \l Empty and thus brings all that
    functionality, but additionally provides means to expand and collapse the item.

    When used together with an \l ExpandablesListView or \l ExpandablesColumn it
    can coordinate with other items in the list to make sure it is scrolled while
    expanding to be fully visible in the view. Additionally such it is made sure that
    only one Expandable item is expanded at a time and it is collapsed when the
    user clicks outside the item.

    You can set \l expanded to true/false to expand/collapse the item.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        ListModel {
            id: listModel
        }

        ListItem.ExpandablesListView {
            anchors { left: parent.left; right: parent.right }
            height: units.gu(24)
            model: listModel

            delegate: ListItem.Expandable {
                id: expandingItem

                expandedHeight: units.gu(30)

                onClicked: {
                    expanded = true;
                }
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/

Empty {
    id: root
    implicitHeight: expanded ? priv.maxExpandedHeight : collapsedHeight
    clip: true

    /*!
      \preliminary
      Reflects the expanded state. Set this to true/false to expand/collapse the item.
     */
    property bool expanded: false

    /*!
      \preliminary
      The collapsed (normal) height of the item. Defaults to the standard height for list items.
     */
    property real collapsedHeight: __height

    /*!
      \preliminary
      The expanded height of the item's content. Defaults to the same as collapsedHeight which
      disabled the expanding feature. In order for the item to be expandable, set this to the
      expanded size. Note that the actual expanded size can be smaller if there is not enough
      space in the containing list. In that case the item becomes flickable automatically.
     */
    property real expandedHeight: collapsedHeight

    /*!
      \internal
      Points to the containing ExpandablesListView or ExpandablesColumn
     */
    property Item __view: ListView.view

    /*!
      \internal
      Reparent any content to inside the Flickable
     */
    property alias children: flickableContent.data

    /*! \internal */
    QtObject {
        id: priv

        /*! \internal
          Gives information whether this item is inside a ExpandablesListView or ExpandablesColumn
         */
        readonly property bool isInExpandableList: __view && __view !== undefined && __view.hasOwnProperty("expandItem") && __view.hasOwnProperty("collapse")

        /*! \internal
          Gives information if there is another item expanded in the containing ExpandablesListView or ExpandablesColumn
         */
        readonly property bool otherExpanded: isInExpandableList && __view.expandedItem !== null && __view.expandedItem !== undefined && __view.expandedItem !== root

        /*! \internal
          Gives information about the maximum expanded height, in case that is limited by the containing ExpandablesListView or ExpandablesColumn
         */
        readonly property real maxExpandedHeight: isInExpandableList ? Math.min(__view.height - collapsedHeight, expandedHeight) : expandedHeight
    }

    states: [
        State {
            name: "otherExpanded"; when: priv.otherExpanded
            PropertyChanges { target: root; opacity: .6 }
        },
        State {
            name: "expanded"; when: expanded
            PropertyChanges { target: root; z: 3 }
        }
    ]

    onExpandedChanged: {
        if (!expanded) {
            contentFlickable.contentY = 0;
        }

        if (priv.isInExpandableList) {
            if (expanded) {
                __view.expandItem(root);
            } else {
                __view.collapse();
            }
        }
    }

    Behavior on height {
        UbuntuNumberAnimation {}
    }
    Behavior on opacity {
        UbuntuNumberAnimation {}
    }

    Flickable {
        id: contentFlickable
        anchors { fill: parent; leftMargin: root.__contentsMargins; rightMargin: __contentsMargins }
        interactive: root.expanded && flickableContent.height > height
        contentHeight: root.expandedHeight
        flickableDirection: Flickable.VerticalFlick

        Behavior on contentY {
            UbuntuNumberAnimation {}
        }

        Item {
            id: flickableContent
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
        }
    }
}
