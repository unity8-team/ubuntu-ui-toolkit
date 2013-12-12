/*
 * Copyright 2013 Canonical Ltd.
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

.pragma library

function isNode(object) {
    // FIXME: Use QuickUtils.className() when it becomes available.
    return (object && object.hasOwnProperty("__isPageTreeNode") && object.__isPageTreeNode);
}

/*!
  Return the parent node in the page tree, or null if the item is the root node or invalid.
 */
function getParentNode(item) {
    var node = null;
    if (item) {
        var i = item.parent;
        while (i) {
            if (isNode(i)) {
                node = i;
                break;
            }
            i = i.parent;
        }
    }
    return node;
}

/*!
  If none of the children of the item is an active PageTreeNode active,
  make the first child that is a PageTreeNode active.
 */
function activateOneChildNode(item) {
    var firstActiveIndex = -1;
    var firstPageTreeNodeIndex = -1;
    var child;
    for (var i=0; i < item.children.length; i++) {
        child = item.children[i];
        if (isNode(child)) {
            if (firstPageTreeNodeIndex < 0) firstPageTreeNodeIndex = i;
            if (child.active) {
                firstActiveIndex = i;
                break;
            }
        }
    }
    if (firstActiveIndex < 0) {
        // None of the children of the MainView is an active PageTreeNode.
        // Activate the first suitable child:
        if (firstPageTreeNodeIndex >= 0) {
            item.children[firstPageTreeNodeIndex].active = true;
        }
    }
}
