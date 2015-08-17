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
        Action { id: global1 }
        Action { id: global2 }
        Action { id: global3 }
        sharedContext.actions: [
            Action { id: shared1 },
            Action { id: shared2 }
        ]
    }
    Item {
        property ActionContext actionContext: ActionContext {
            id: context
        }
        Action { id: local }
    }
    ActionContext {
        id: activeContext
        active: true
    }

    UbuntuTestCase {
        name: "ActionAPI13"
        when: windowShown

        SignalSpy {
            id: triggeredSpy
            signalName: "triggered"
        }

        function contains(list, entry) {
            for (var i = 0; i < list.length; i++) {
                if (list[i] == entry) {
                    return true;
                }
            }
            return false;
        }

        function cleanup() {
            context.removeAction(shared1);
            context.removeAction(global2);
            activeContext.removeAction(global1);
            context.active = false;
            activeContext.overlay = false;
            context.active = false;
            activeContext.active = true;
            triggeredSpy.target = null;
            triggeredSpy.clear();
        }

        function initTestCase() {
            compare(manager.globalContext.active, true, "globalContext is always active");
            compare(manager.sharedContext.active, false, "sharedContext is always inactive");
            compare(manager.globalContext.overlay, false, "globalContext is not overlay");
            compare(manager.sharedContext.overlay, false, "sharedContext is not overlay");
            compare(manager.globalContext.actions.length, 3, "global context action count mismatch");
            compare(manager.sharedContext.actions.length, 2, "shared context action count mismatch");
            compare(context.actions.length, 1, "Local context action count mismatch");
            compare(context.active, false, "context should be inactive");
            compare(activeContext.active, true, "activeContext shoudl be active");
        }

        function test_actions_in_multiple_contexts() {
            context.addAction(shared1);
            context.addAction(global2);
            verify(contains(manager.sharedContext.actions, shared1), "shared Action missing from shared");
            verify(contains(context.actions, shared1), "shared Action missing from local");
            verify(contains(manager.globalContext.actions, global2), "global Action missing from global");
            verify(contains(context.actions, global2), "global Action missing from local");
        }

        function test_global_actions_always_active() {
            context.addAction(global1);
            triggeredSpy.target = global1;
            global1.trigger();
            triggeredSpy.wait(400);
        }

        function test_shared_actions_active_data() {
            return [
                {tag: "shared Action not in any local context", contexts: [], action: shared1, xfail: true},
                {tag: "shared Action in inactive local context", contexts: [context], action: shared1, xfail: true},
                {tag: "shared Action in active local context", contexts: [activeContext], action: shared1, xfail: false},
                {tag: "shared Action in an active and inactive local context", contexts: [context, activeContext], action: shared1, xfail: false},
            ];
        }
        function test_shared_actions_active(data) {
            for (var i = 0; i < data.contexts.length; i++) {
                data.contexts[i].addAction(data.action);
                verify(contains(data.contexts[i].actions, data.action), "Action not found in context");
            }
            triggeredSpy.target = data.action;
            data.action.trigger();
            if (data.xfail) {
                expectFail(data.tag, "Action should not trigger");
            }
            triggeredSpy.wait(400);
            for (var i = 0; i < data.contexts.length; i++) {
                data.contexts[i].removeAction(data.action);
                verify(!contains(data.contexts[i].actions, data.action), "Action not removed from context");
            }
        }

        function test_multiple_conytexts_active() {
            context.active = true;
            compare(activeContext.active, true);
            compare(context.active, true);
        }

        function test_overlay_data() {
            return [
                {tag: "inactive overlay", active: false, activeContext: true},
                {tag: "active overlay", active: true, activeContext: false},
            ];
        }
        function test_overlay(data) {
            context.overlay = true;
            context.active = data.active;
            compare(activeContext.active, data.activeContext);
        }
    }
}

