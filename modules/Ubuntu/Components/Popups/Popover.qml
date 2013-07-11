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
import Ubuntu.Components 0.1
import "popupPositioning.js" as PopupPositioning
import "../mathUtils.js" as MathUtils

/*!
    \qmltype Popover
    \inherits PopupBase
    \inqmlmodule Ubuntu.Components.Popups 0.1
    \ingroup ubuntu-popups
    \brief A popover allows an application to present additional content without changing the view.
        A popover has a fixed width and automatic height, depending on is contents.
        It can be closed by clicking anywhere outside of the popover area.

    \l {http://design.ubuntu.com/apps/building-blocks/popover}{See also the Design Guidelines on Popovers}.

    Example:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        import Ubuntu.Components.Popups 0.1

        Rectangle {
            color: Theme.palette.normal.background
            width: units.gu(80)
            height: units.gu(80)
            Component {
                id: popoverComponent

                Popover {
                    id: popover
                    Column {
                        id: containerLayout
                        anchors {
                            left: parent.left
                            top: parent.top
                            right: parent.right
                        }
                        ListItem.Header { text: "Standard list items" }
                        ListItem.Standard { text: "Do something" }
                        ListItem.Standard { text: "Do something else" }
                        ListItem.Header { text: "Buttons" }
                        ListItem.SingleControl {
                            highlightWhenPressed: false
                            control: Button {
                                text: "Do nothing"
                                anchors {
                                    fill: parent
                                    margins: units.gu(1)
                                }
                            }
                        }
                        ListItem.SingleControl {
                            highlightWhenPressed: false
                            control: Button {
                                text: "Close"
                                anchors {
                                    fill: parent
                                    margins: units.gu(1)
                                }
                                onClicked: PopupUtils.close(popover)
                            }
                        }
                    }
                }
            }
            Button {
                id: popoverButton
                anchors.centerIn: parent
                text: "open"
                onClicked: PopupUtils.open(popoverComponent, popoverButton)
            }
        }
    \endqml
*/
PopupBase {
    id: popover

    /*!
      \qmlproperty list<Object> container
      Content will be put inside the foreround of the Popover.
    */
    default property alias container: containerItem.data

    /*!
      \qmlproperty real contentWidth
      \qmlproperty real contentHeight
      The properties can be used to alter the default content width and heights.
      */
    property alias contentWidth: foreground.width
    /*! \internal */
    property alias contentHeight: foreground.height

    /*!
      The Item such as a \l Button that the user interacted with to open the Dialog.
      This property will be used for the automatic positioning of the Dialog next to
      the caller, if possible.
     */
    property Item caller

    /*!
      The property holds the item to which the pointer should be anchored to.
      This can be same as the caller or any child of the caller. By default the
      property is set to caller.
      */
    property Item pointerTarget: caller

    /*!
      The property holds the margins from the popover's dismissArea. The property
      is themed.
      */
    property real edgeMargins: units.gu(2)

    /*!
      The property holds the margin from the popover's caller. The property
      is themed.
      */
    property real callerMargin: units.gu(1)

    /*!
      The property drives the automatic closing of the Popover when user taps
      on the dismissArea. The default behavior is to close the Popover, therefore
      set to true.

      When set to false, closing the Popover is the responsibility of the caller.
      Also, the mouse and touch events are not blocked from the dismissArea.
      */
    property bool autoClose: true

    fadingAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }

    __foreground: foreground
    __eventGrabber.enabled: autoClose
    __closeOnDismissAreaPress: true

    StyledItem {
        id: foreground

        property real minimumWidth: units.gu(40) - 2.0*edgeMargins
        property real maximumWidth: dismissArea ? dismissArea.width - 2.0*edgeMargins : 0.0
        property real minimumHeight: units.gu(6)
        property real maximumHeight: dismissArea ? dismissArea.height * 3/4 : 0.0
        width: Math.min(minimumWidth, maximumWidth)
        height: MathUtils.clamp(containerItem.height, minimumHeight, maximumHeight)

        Item {
            id: containerItem
            parent: foreground.__styleInstance.contentItem
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            height: childrenRect.height
        }

        // FIXME: transform pointerTarget into globalPointerTarget
        property var positionToAvailable: { "above": Qt.size(dismissArea.width - 2.0*edgeMargins,
                                                             pointerTarget.y - edgeMargins),
                                            "below": Qt.size(dismissArea.width - 2.0*edgeMargins,
                                                             dismissArea.height - pointerTarget.y - pointerTarget.height - edgeMargins),
                                            "left": Qt.size(pointerTarget.x - edgeMargins,
                                                            dismissArea.height - 2.0*edgeMargins),
                                            "right": Qt.size(dismissArea.width - pointerTarget.x - pointerTarget.width - edgeMargins,
                                                             dismissArea.height - 2.0*edgeMargins)
                                          }

        function isFittingPosition(position) {
            if (!pointerTarget) return false;
            var available = positionToAvailable ? positionToAvailable[position] : Qt.size(0, 0);
            // FIXME: be smarter, take into account the reachability of the target by the pointer
            //var reducedForeground = reducedRectangle(foreground, shapePadding * 2.0, shapePadding * 2.0);
            return PopupPositioning.fitsInside(foreground, available);
        }

        property var positions: ["above", "right", "left", "below"]
        property var fittingPositions: positions.filter(isFittingPosition)
        onFittingPositionsChanged: print(fittingPositions)

        // possible values: "above", "right", "left", "below"
        // FIXME: defaulting to above when no position fits is wrong
        property string position: fittingPositions.length > 0 ? fittingPositions[0] : "above"
        property real edgeMargins: popover.edgeMargins
        property real shapePadding: units.gu(3)
        property var positionToTarget: { "above": Qt.point(pointerTarget.x + pointerTarget.width/2.0, pointerTarget.y),
                                         "below": Qt.point(pointerTarget.x + pointerTarget.width/2.0, pointerTarget.y + pointerTarget.height),
                                         "left": Qt.point(pointerTarget.x, pointerTarget.y + pointerTarget.height/2.0),
                                         "right": Qt.point(pointerTarget.x + pointerTarget.width, pointerTarget.y + pointerTarget.height/2.0)
                                       }
        property point target: positionToTarget ? positionToTarget[position] : Qt.point(0, 0)
        property var positionToAnchor: { "above": Qt.point(0, foreground.height/2.0),
                                         "below": Qt.point(0, -foreground.height/2.0),
                                         "left": Qt.point(foreground.width/2.0, 0),
                                         "right": Qt.point(-foreground.width/2.0, 0)
                                       }
        property point centerToAnchor: positionToAnchor[position]
        property string alignment: position === "above" || position === "below" ? "vertical" : "horizontal"
        // FIXME: extract better function
        property rect pointerLimit: PopupPositioning.reducedRectangle(dismissArea,
                                                                      alignment === "vertical" ? edgeMargins + shapePadding : 0,
                                                                      alignment === "horizontal" ? edgeMargins + shapePadding : 0)
        property point pointer: PopupPositioning.closestPointInRectangle(target, pointerLimit)
        property point shiftedPointer: Qt.point(pointer.x - centerToAnchor.x, pointer.y - centerToAnchor.y)
        property rect centerLimit: PopupPositioning.reducedRectangle(dismissArea,
                                                                     alignment === "vertical" ? edgeMargins + foreground.width/2.0 : 0,
                                                                     alignment === "horizontal" ? edgeMargins + foreground.height/2.0 : 0)
        property point center: PopupPositioning.closestPointInRectangle(shiftedPointer, centerLimit)
        x: center.x - foreground.width / 2.0
        y: center.y - foreground.height / 2.0

        Pointer {
            x: foreground.pointer.x - width / 2.0 - parent.x
            y: foreground.pointer.y - height / 2.0 - parent.y
            property var positionToDirection: { "above": "down",
                                                "below": "up",
                                                "left": "right",
                                                "right": "left"
                                              }
            direction: positionToDirection[foreground.position]
        }

        style: Theme.createStyleComponent("PopoverForegroundStyle.qml", foreground)
    }
}
