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
            Action { id: shared1; objectName: "shared1" },
            Action { id: shared2; objectName: "shared2" },
            Action { id: shared3; objectName: "shared3" }
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
    ActionContext {
        id: overlayContext
        overlay: true
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
            context.removeAction(shared2);
            context.removeAction(global2);
            activeContext.removeAction(global1);
            activeContext.removeAction(shared1);
            activeContext.removeAction(shared2);
            overlayContext.removeAction(shared1);
            overlayContext.removeAction(shared2);
            overlayContext.removeAction(shared3);
            context.active = false;
            activeContext.overlay = false;
            context.active = false;
            overlayContext.active = false;
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
            compare(manager.sharedContext.actions.length, 3, "shared context action count mismatch");
            compare(context.actions.length, 1, "Local context action count mismatch");
            compare(context.active, false, "context should be inactive");
            compare(activeContext.active, true, "activeContext should be active");
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
                        {tag: "active overlay", active: true, xfail: true},
                        {tag: "inactive overlay", active: false, xfail: false},
                    ];
        }
        function test_overlay(data) {
            activeContext.addAction(shared1);
            context.overlay = true;
            context.active = data.active;
            verify(activeContext.active, "active ActionContext deactivated");
            // try emit activeContext actions
            triggeredSpy.target = shared1;
            shared1.trigger();
            if (data.xfail) {
                expectFailContinue(data.tag, "Overlay should suppress other active context triggering");
            }

            triggeredSpy.wait(400);
        }

        function test_only_one_overlay_active_data() {
            return [
                {tag: "disable topmost first", trigger: [shared1, shared2, shared3], triggerCount: [0, 0, 1], disableOrder: [overlayContext, context], disableTriggerCount: [[0, 1, 0], [1, 0, 0]] },
                {tag: "disable second first", trigger: [shared1, shared2, shared3], triggerCount: [0, 0, 1], disableOrder: [context, overlayContext], disableTriggerCount: [[0, 0, 1], [1, 0, 0]] },
            ]
        }
        function test_only_one_overlay_active(data) {
            activeContext.addAction(shared1);
            context.addAction(shared2);
            overlayContext.addAction(shared3);
            context.overlay = true;
            context.active = true;
            overlayContext.active = true;
            verify(context.active, "context deactivated");
            verify(activeContext.active, "activeContext deactivated");
            verify(overlayContext.active, "overlayContext deactivated");

            // try to trigger actions
            for (var i = 0; i < data.trigger.length; i++) {
                triggeredSpy.clear();
                triggeredSpy.target = data.trigger[i];
                triggeredSpy.target.trigger();
                compare(triggeredSpy.count, data.triggerCount[i], "action #" + data.trigger[i] + " should not be triggered");
            }
            // deactivate
            for (var i = 0; i < data.disableOrder.length; i++) {
                data.disableOrder[i].active = false;
                var disableCount = data.disableTriggerCount[i]
                for (var j = 0; j < data.trigger.length; j++) {
                    triggeredSpy.clear();
                    triggeredSpy.target = data.trigger[j];
                    triggeredSpy.target.trigger();
                    compare(triggeredSpy.count, disableCount[j], "action #" + data.trigger[j] + " should not be triggered");
                }
            }
        }
    }
}

