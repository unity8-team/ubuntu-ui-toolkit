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

.import "../mathUtils.js" as MathUtils

function closestPointInRectangle(point, rect) {
    var x = MathUtils.clamp(point.x, rect.x, rect.x + rect.width);
    var y = MathUtils.clamp(point.y, rect.y, rect.y + rect.height);
    return Qt.point(x, y);
}

function reducedRectangle(rect, horizontalMargin, verticalMargin) {
    return Qt.rect(rect.x + horizontalMargin,
                   rect.y + verticalMargin,
                   rect.width - 2*horizontalMargin,
                   rect.height - 2*verticalMargin);
}

function fitsInside(rect, container) {
    return rect.width <= container.width && rect.height <= container.height;
}
