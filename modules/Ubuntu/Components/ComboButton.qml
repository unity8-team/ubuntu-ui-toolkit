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
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

/*!
    \qmlabstract ComboButton
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief An enhancemed Button that offers secondary actions in a menu.
*/
StyledItem {
    id: comboButton
    property string text: null
    property color color: __styleInstance.defaultColor
    property Gradient gradient: __styleInstance.defaultGradient
    /*!
      The list of secondary actions. This is a model.
    */
    property var secondaryActions: null
    /*!
      If set the delegate can be used to customize how secondaryActions are
      rendered, for example using icons. The delegate must have a width and
      call clicked on the button with a value to match built-in behavior.
     */
    property Component delegate: ListItem.Standard {
        text: modelData
        onClicked: comboButton.clicked(modelData)
        width: comboButton.width
    }
    /*!
      If either the primary button or a secondary action is clicked, the signal
      will be emitted and \l value is either the button itself or the action selected.
    */
    signal clicked(var value)
    onClicked: if (row.lastPopover != null) row.lastPopover.hide()

    Row {
        id: 'row'
        property var lastPopover: null
        property bool depressed: lastPopover != null && lastPopover.visible
        Button {
            id: primaryButton
            text: comboButton.text
            color: comboButton.color
            gradient: comboButton.gradient
            onClicked: comboButton.clicked
        }
        Button {
            id: secondaryButton
            iconSource: comboButton.__styleInstance.chevron
            color: parent.depressed ? UbuntuColors.coolGrey : comboButton.color
            gradient: parent.depressed ? UbuntuColors.greyGradient : comboButton.gradient
            width: units.gu(5)
            onClicked: {
                parent.lastPopover = PopupUtils.open(secondaryComponent, secondaryButton, {
                    "contentWidth": comboButton.childrenRect.width,
                    "__preferredAlignment": "positionBelow",
                    "edgeMargins": units.gu(1)
                    })
                }
        }
    }
    style: Theme.createStyleComponent("ComboButtonStyle.qml", comboButton)

    Component {
        id: secondaryComponent
        Popover {
            id: secondaryPopover
            Column {
                Repeater {
                    model: secondaryActions
                    delegate: Loader {
                        sourceComponent: delegate
                        property variant modelData: model.modelData
                    }
                }
            }
        }
    }
}

