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
import QtTest 1.0
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

Item {
    width: units.gu(40)
    height: units.gu(60)

    ExpandablesColumn {
        id: expandablesColumn
        anchors { left: parent.left; top: parent.top; right: parent.right }
        height: units.gu(60)
        clip: true

        Repeater {
            model: 20
            delegate: Expandable {
                id: expandable
                objectName: "expandable" + index
                expandedHeight: contentColumn.height

                onClicked: expanded = !expanded

                Column {
                    id: contentColumn
                    anchors { left: parent.left; right: parent.right; top: parent.top }
                    Rectangle {
                        anchors { left: parent.left; right: parent.right}
                        id: collapsedRect
                        color: index % 2 == 0 ? "khaki" : "blue"
                        height: expandable.collapsedHeight
                    }
                    Rectangle {
                        anchors { left: parent.left; right: parent.right }
                        height: units.gu(40)
                        color: "orange"
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        name: "ExpandablesColumn"
        when: windowShown

        function expandItem(item) {
            item.expanded = true;
            var targetHeight = Math.min(item.expandedHeight, expandablesColumn.height - item.collapsedHeight);
            tryCompare(item, "height", targetHeight);
        }

        function collapse() {
            var expandedItem = expandablesColumn.expandedItem;
            if (expandedItem != undefined) {
                expandedItem.expanded = false;
                tryCompare(expandedItem, "height", expandedItem.collapsedHeight);
            }
        }

        function test_expandedItem() {
            var item = findChild(expandablesColumn, "expandable1");
            expandItem(item);

            // expandedItem needs to point to the expanded item
            compare(expandablesColumn.expandedItem, item);

            collapse();

            // expandedItem must be unset after collapsing
            compare(expandablesColumn.expandedItem, undefined);
        }

        function test_noScrollingNeeded() {
            var item = findChild(expandablesColumn, "expandable1");
            compare(expandablesColumn.mapFromItem(item).y, item.collapsedHeight);

            expandItem(item);

            compare(expandablesColumn.mapFromItem(item).y, item.collapsedHeight);
        }

        function test_scrollToTop() {
            expandablesColumn.height = units.gu(30);

            var item = findChild(expandablesColumn, "expandable1");
            compare(expandablesColumn.mapFromItem(item).y, item.collapsedHeight);

            expandItem(item);

            compare(expandablesColumn.mapFromItem(item).y, 0);
        }

        function test_scrollIntoView() {
            var item = findChild(expandablesColumn, "expandable9");
            expandItem(item);

            // The item must be scrolled upwards, leaving space for one other item at the bottom
            compare(expandablesColumn.mapFromItem(item).y, expandablesColumn.height - item.collapsedHeight - item.expandedHeight);
        }

        function test_collapseByClickingOutside() {
            // expand item 0
            var item = findChild(expandablesColumn, "expandable0");
            expandItem(item);

            // click on item 1
            var item1 = findChild(expandablesColumn, "expandable1");
            mouseClick(item1, item1.width / 2, item1.height / 2);

            // make sure item1 is collapsed
            tryCompare(item, "expanded", false);
        }

        function cleanup() {
            // Restore listview height
            expandablesColumn.height = units.gu(60);
            collapse();
            // scroll the column back to top
            expandablesColumn.flick(0, units.gu(500));
            tryCompare(expandablesColumn, "flicking", false);
        }
    }
}
