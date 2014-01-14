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
import Ubuntu.Components 0.1
import Ubuntu.Test 0.1
import Ubuntu.Unity.Action 1.0 as UnityActions

MainView {
    width: 400
    height: 400

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }
        OptionSelector {
            id: selector

            text: "TEST"
            delegate: OptionSelectorDelegate {
                text: name
                subText: description
                iconSource: image
                constrainImage: true
            }
            model: customModel

            action: {
                enabled: true
                name: 'selector'
                text: 'Selector'
                parameterType: UnityActions.Action.Integer
            }
        }

        OptionSelector {
            id: multiSelector

            text: "TEST"
            model: [1, 2, 3, 4]
            multiSelection: true
        }
    }

    ListModel {
        id: customModel
        ListElement { name: "Name 1"; description: "Description 1"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 2"; description: "Description 2"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 3"; description: "Description 3"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 4"; description: "Description 4"; image: "../../resources/optionselector/test.png" }
    }

    SignalSpy {
        id: clickedSignal
        target: multiSelector
        signalName: "delegateClicked"
    }

    SignalSpy {
        id: triggeredSignal
        target: selector
        signalName: "triggered"
    }

    SignalSpy {
        id: expansionSignal
        target: selector
        signalName: "expansionCompleted"
    }

    UbuntuTestCase {
         name: "OptionSelectorAPI"
         when: windowShown

         function test_expanded() {
             var listContainer = findChild(selector, "listContainer");

             compare(multiSelector.expanded, false, "expanded should be false on startup.")

             compare(listContainer.currentlyExpanded, false, "expanded should be true if list is an expanded one");
             compare(listContainer.state, "collapsed", "state should be collapsed");

             selector.expanded = true;
             compare(listContainer.currentlyExpanded, true, "expanded should be false if list is an expanded one");
             compare(listContainer.state, "expanded", "state should be expanded");
         }

         function test_text() {
             var newText = "Hello World!";
             selector.text = newText;
             compare(selector.text, newText, "set/get");
         }

         function test_selectedIndex() {
            compare(selector.selectedIndex, 0, "selectedIndex is 0 by default");
         }

         function test_model() {
             selector.model = undefined;
             var newValues = ["value0","value1","value2","value3"];
             selector.model = newValues;
             compare(selector.model, newValues, "set/get");
         }

         function test_custom_model_delegate() {
             compare(selector.model, customModel, "Model wasn't set correctly.");
         }

         function test_clicked_signal() {
             skip('FIXME: This test doesn\'t pass in CI. Passes locally for anyone who tried.')
             mouseClick(multiSelector, 100, 100, Qt.LeftButton);
             clickedSignal.wait();
             compare(multiSelector.isSelected(clickedSignal.signalArguments[0][0]), true, "Clicked signal was not emitted and option was not selected.");

             //Second option.
             mouseClick(multiSelector, 100, 100, Qt.LeftButton);
             clickedSignal.wait();
             //Did the second index get clicked?
             compare(multiSelector.isSelected(clickedSignal.signalArguments[1][0]), false, "Clicked signal was not emitted and option was not deselected.");
         }

         function test_expansion_signal() {
             mouseClick(selector, 100, 100, Qt.LeftButton);
             expansionSignal.wait();
         }

         function test_triggered() {
             mouseClick(selector, 100, 100, Qt.LeftButton);
             triggeredSignal.wait();
         }
    }
}
