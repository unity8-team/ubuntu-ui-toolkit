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
import Ubuntu.Components.Popups 1.3

MainView {
    width: units.gu(40)
    height: units.gu(71)

    property Dialog testDialog: null

    Action {
        id: sharedAction;
        text: "pressme"
        onTriggered: print("hey")
    }

    Component {
        id: dialogComponent
        Dialog {
            id: dialog
            Button {
                objectName: "okButton"
                action: Action {
                    text: "Ok"
                    onTriggered: {
                        testDialog = null;
                        PopupUtils.close(dialog);
                    }
                }
            }
        }
    }

    AdaptivePageLayout {
        id: multiColumn
        anchors.fill: parent
        // only one column!
        layouts: PageColumnsLayout {
            when: true
            PageColumn {
                fillWidth: true
            }
        }

        primaryPage: Page {
            Column {
                Button {
                    action: sharedAction
                }
                Button {
                    action: Action {
                        id: dialogOpen
                        text: "Open dialog"
                        onTriggered: testDialog = PopupUtils.open(dialogComponent)
                    }
                }
            }
        }
        Page {
            id: secondPage
            objectName: "secondPage"
            Button {
                action: Action {
                    id: secondAction
                    text: "once more"
                }
            }
        }
    }

    UbuntuTestCase {
        name: "PageAndDialogActions"
        when: windowShown

        SignalSpy {
            id: triggerSpy
            signalName: "triggered"
        }
        SignalSpy {
            id: activeSpy
            signalName: "activeChanged"
        }

        function cleanup() {
            triggerSpy.target = null;
            triggerSpy.clear();
            activeSpy.target = null;
            activeSpy.clear();
            if (testDialog) {
                PopupUtils.close(testDialog);
                testDialog = null;
            }
            multiColumn.removePages(multiColumn.primaryPage);
            waitForRendering(multiColumn);
        }

        function test_secondPage_deactivates_first_actions() {
            activeSpy.target = secondPage.actionContext;
            multiColumn.addPageToCurrentColumn(multiColumn.primaryPage, secondPage);
            activeSpy.wait();

            triggerSpy.target = sharedAction;
            triggerSpy.target.trigger();
            compare(triggerSpy.count, 0, "sharedAction should not be triggered");
        }

        function test_dialog() {
            dialogOpen.trigger();
            waitForRendering(testDialog);
            // the active property doesn't change, only actions are not triggered from it
            verify(multiColumn.primaryPage.actionContext.active, "primary page context should be active");
            verify(testDialog.actionContext.active, "test dialog context should be active");
            verify(testDialog.actionContext.overlay, "test dialog context should be overlay");

            // test action triggering
            triggerSpy.target = sharedAction;
            triggerSpy.target.trigger();
            compare(triggerSpy.count, 0, "sharedAction should not be triggered");

            // trigger dialog action
            triggerSpy.target = findChild(testDialog, "okButton").action;
            triggerSpy.clear();
            triggerSpy.target.trigger();
            triggerSpy.wait();
        }
    }
}