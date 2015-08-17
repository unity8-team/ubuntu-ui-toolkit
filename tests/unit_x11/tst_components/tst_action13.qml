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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

MainView {
    width: units.gu(40)
    height: units.gu(71)

    ActionManager {
        id: manager
        Action {
        }
        Action {
        }
        Action {
        }
    }
    Action {
        id: shared1
    }
    Action {
        id: shared2
    }

    UbuntuTestCase {
        name: "ActionAPI13"
        when: windowShown

        function contains(list, entry) {
            for (var i = 0; i < list.length; i++) {
                if (list[i] == entry) {
                    return true;
                }
            }
            return false;
        }

        function initTestCase() {
            compare(manager.globalContext.active, true, "globalContext is always active");
            compare(manager.sharedContext.active, false, "sharedContext is always inactive");
            compare(manager.globalContext.overlay, false, "globalContext is not overlay");
            compare(manager.sharedContext.overlay, false, "sharedContext is not overlay");
            compare(manager.globalContext.actions.length, 3, "global context action count mismatch");
            compare(manager.sharedContext.actions.length, 2, "shared context action count mismatch");
        }

    }
}

