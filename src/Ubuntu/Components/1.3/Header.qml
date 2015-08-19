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

/*!
  TODO docs
*/
Components.StyledItem {
    id: header

    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
    }
    y: 0

    /*!
      Animate showing and hiding of the header.
     */
    property bool animate: true

    Behavior on y {
        enabled: animate && !(header.flickable && header.flickable.moving)
        SmoothedAnimation {
            duration: Components.UbuntuAnimation.BriskDuration
        }
    }

// TODO test this
    /*! \internal */
//    onHeightChanged: {
//        internal.checkFlickableMargins();
//        internal.movementEnded();
//        if (header.config.visible) {
//            header.show();
//        } else {
//            header.hide();
//        }
//    }

    // with PageHeadConfiguration 1.2, always be visible.
//    visible: title || contents || tabsModel || internal.newConfig
//    onVisibleChanged: {
//        internal.checkFlickableMargins();
//    }
    // avoid interacting with the header contents when it is animating y
    enabled: header.y === 0


    // TODO TIM: readonly property visible?

    /*!
      Show the header
     */
    function show() {
//        if (internal.newConfig) {
//            header.config.visible = true;
//        }
        // Enable the header as soon as it finished animating
        //  to the fully visible state:
        header.enabled = Qt.binding(function() { return header.y === 0; });
        header.y = 0;
    }

    /*!
      Hide the header
     */
    function hide() {
//        if (internal.newConfig) {
//            header.config.visible = false;
//        }
        // Disable the header immediately (the update of the y-value
        //  is delayed because of the Behavior defined on it):
        header.enabled = false;
        header.y = -header.height;
    }

    /*!
      The text to display in the header
     */
    property string title: ""

    /*!
      The contents of the header. If this is set, the \l title will be invisible.
     */
    property Item contents: null

    // FIXME TIM: In the CPO, add getLeadingActionBar, getTrailingActionBar and
    //  getSections functions.

    /*!
      The flickable that controls the movement of the header.
     */
    property Flickable flickable: null
    onFlickableChanged: {
        internal.connectFlickable();
        if (!header.locked) {
            header.show();
        }
    }
//    Connections {
//        target: header.config
//        ignoreUnknownSignals: true // PageHeadConfiguration <1.2 lacks the signals below
//        onVisibleChanged: {
//            if (header.config.visible) {
//                header.show();
//            } else {
//                header.hide();
//            }
//            internal.checkFlickableMargins();
//        }
//        onLockedChanged: {
//            internal.connectFlickable();
//            if (!header.config.locked) {
//                internal.movementEnded();
//            }
//        }
//    }

    /*!
      The header is not fully opened or fully closed.

      This property is true if the header is animating towards a fully
      opened or fully closed state, or if the header is moving due to user
      interaction with the flickable.

      The value of moving is always false when using an old version of
      PageHeadConfiguration (which does not have the visible property).

      Used in tst_header_locked_visible.qml.
    */
    // TODO TIM: expose as 'animating'?
//    readonly property bool moving: internal.newConfig &&
//                                   ((config.visible && header.y !== 0) ||
//                                    (!config.visible && header.y !== -header.height))

    QtObject {
        id: internal

        /*!
          Track the y-position inside the flickable.
         */
        property real previousContentY: 0

        /*!
          The previous flickable to disconnect events
         */
        property Flickable previousFlickable: null

        /*!
          Disconnect previous flickable, and connect the new one.
         */
        function connectFlickable() {
            // Finish the current header movement in case the current
            // flickable is disconnected while scrolling:
            internal.movementEnded();

            if (previousFlickable) {
                previousFlickable.contentYChanged.disconnect(internal.scrollContents);
                previousFlickable.movementEnded.disconnect(internal.movementEnded);
                previousFlickable.interactiveChanged.disconnect(internal.interactiveChanged);
                previousFlickable = null;
            }
            if (flickable && !(internal.newConfig && header.config.locked)) {
                // Connect flicking to movements of the header
                previousContentY = flickable.contentY;
                flickable.contentYChanged.connect(internal.scrollContents);
                flickable.movementEnded.connect(internal.movementEnded);
                flickable.interactiveChanged.connect(internal.interactiveChanged);
                flickable.contentHeightChanged.connect(internal.contentHeightChanged);
                previousFlickable = flickable;
            }
            internal.checkFlickableMargins();
        }

        /*!
          Update the position of the header to scroll with the flickable.
         */
        function scrollContents() {
            // Avoid updating header.y when rebounding or being dragged over the bounds.
            if (!flickable.atYBeginning && !flickable.atYEnd) {
                var deltaContentY = flickable.contentY - previousContentY;
                // FIXME: MathUtils.clamp  is expensive. Fix clamp, or replace it here.
                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
            }
            previousContentY = flickable.contentY;
        }

        /*!
          Fully show or hide the header, depending on its current y.
         */
        function movementEnded() {
            if (!header.locked) {
                if ( (flickable && flickable.contentY < 0) ||
                        (header.y > -header.height/2)) {
                    header.show();
                } else {
                    header.hide();
                }
            }
        }

        /*
          Content height of flickable changed
         */
        function contentHeightChanged() {
            if (flickable && flickable.height >= flickable.contentHeight) header.show();
        }

        /*
          Flickable became interactive or non-interactive.
         */
        function interactiveChanged() {
            if (flickable && !flickable.interactive) header.show();
        }

        /*
          Check the topMargin of the flickable and set it if needed to avoid
          contents becoming unavailable behind the header.
         */
        function checkFlickableMargins() {
            if (header.flickable) {
                var headerHeight = 0;
//                if (header.visible && !(internal.newConfig &&
//                                        header.config.locked &&
//                                        !header.config.visible)) {
                // TODO TIM: check this.
                // TODO TIM 2: Why don't we make a locked header always visible?
                if (header.visible || !header.locked) {
                    headerHeight = header.height;
                }

                if (flickable.topMargin !== headerHeight) {
                    var oldContentY = flickable.contentY;
                    var previousHeaderHeight = flickable.topMargin;
                    flickable.topMargin = headerHeight;
                    // push down contents when header grows,
                    // pull up contents when header shrinks.
                    flickable.contentY = oldContentY - headerHeight + previousHeaderHeight;
                }
            }
        }
    }

    theme.version: Components.Ubuntu.toolkitVersion
//    styleName: header.useDeprecatedToolbar ? "HeaderStyle" : "PageHeadStyle"
}
