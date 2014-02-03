/*
 * Copyright 2013-2014 Canonical Ltd.
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

/*!
  The given object is an instance of the PageTreeNode component.
 */
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
  Return the first child that is a PageTreeNode, or null if none of the children
  is a PageTreeNode.
 */
function getFirstChildNode(item) {
    var child;
    for (var i=0; i < item.children.length; i++) {
        child = item.children[i];
        if (isNode(child)) {
            return child;
        }
    }
    return null;
}
