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

Item {
    id: root

    implicitWidth: units.gu(2)
    implicitHeight: units.gu(2)

    Rectangle {
        anchors.fill: parent
        radius: width / 2
        color: {
            var palette = theme.palette.normal
            if (!styledItem.enabled) {
                return palette.foreground
            }
            return styledItem.checked? palette.positive : palette.background
        }
        border.color: theme.palette.normal.base
        border.width: units.dp(
            styledItem.checked || !styledItem.enabled? 0 : 1
        )
    }

    Rectangle {

        // These values are calculated using the proportions between
        // the container and the inner shapes (from the design source files).
        property real indeterminateWidth: Math.round(root.width * .426470588)
        property real indeterminateHeight: Math.round(root.height * .088235294)
        property real checkedWidth: Math.round(root.width * .397058824)

        visible: styledItem.checked
        width: styledItem.indeterminate? indeterminateWidth : checkedWidth
        height: styledItem.indeterminate? indeterminateHeight : checkedWidth
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        radius: styledItem.indeterminate? 0 : width / 2
        color: (
            styledItem.enabled
            ? theme.palette.normal.positiveText
            : theme.palette.normal.backgroundTertiaryText
        )
    }
}
