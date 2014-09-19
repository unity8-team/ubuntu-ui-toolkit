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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2

Item {
    width: units.gu(40)
    height: units.gu(71)

    ListItemLayout {
        id: defaults
    }

    UbuntuTestCase {
        name: "ListItemLayout API"
        when: windowShown

        function test_0_defaults() {
            compare(defaults.anchors.leftMargin, units.gu(2), "Left margin wrong");
            compare(defaults.anchors.rightMargin, units.gu(2), "Right margin wrong");
            compare(defaults.anchors.topMargin, units.gu(1), "Top margin wrong");
            compare(defaults.anchors.bottomMargin, units.gu(1), "Bottom margin wrong");
            compare(defaults.spacing, units.gu(1), "Wrong spacing");
        }
    }
}
