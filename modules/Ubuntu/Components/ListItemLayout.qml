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

import QtQuick 2.2
import QtQuick.Layouts 1.1

/*!
  \qmltype ListItemLayout
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \brief Standard ListItem layout for vertical flowing lists.

  The component is a GridLayout providing positioning of the different containers
  either horizontally or vertically, where the container can be one of the
  predefined containers or any other component. It fills the entire item it is
  declared in, with 1 grid unit margin on top and bottom and 2 grid units margin
  on left and right for horizontal, respectively 2 brid units on top and bottom
  and 1 grid unit left and right for vertical direction. The spacing in between the
  containers is 1 grid unit in both of the cases. Component sizes can be driven
  using the layouting mechanism provided by QtQuick.

  It is designed to be used with \l ListItem, however it can be used with any
  Item in any context, especially that ListItem does not provide vertical direction.

  \qml
  import QtQuick 2.2
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.2

  ListItem {
      width: units.gu(40)
      height: units.gu(7)
      ListItemLayout {
          ColumnLayout {
              spacing: units.gu(0.5)
              Layout.alignment: Qt.AlignVCenter
              Layout.maximumHeight: parent.height
              Layout.maximumWidth: parent.width
              Layout.preferredWidth: childrenRect.width
              Layout.fillWidth: true
              Label {
                  text: "Title"
                  fontSize: "large"
              }
              Label {
                  text: "Subtitle"
                  fontSize: "small"
              }
          }
          Switch {
          }
      }
  }
  \endqml
 */
GridLayout {
    id: layout

    /*!
      The property specifies the direction of the layout. It can take one of the
      following values: \c Qt.Horizontal, \c Qt.Vertical. Defailts to \c Qt.Horizontal.
      */
    property int direction: Qt.Horizontal

    columns: (direction == Qt.Horizontal) ? -1 : 1
    rows: (direction == Qt.Vertical) ? -1 : 1
    rowSpacing: (direction == Qt.Horizontal) ? 0 : units.gu(1)
    columnSpacing: (direction == Qt.Vertical) ? 0 : units.gu(1)
    flow: (direction == Qt.Horizontal) ? GridLayout.LeftToRight : GridLayout.TopToBottom
    anchors {
        fill: parent
        leftMargin: (direction == Qt.Horizontal) ? units.gu(2) : units.gu(1)
        rightMargin: (direction == Qt.Horizontal) ? units.gu(2) : units.gu(1)
        topMargin: (direction == Qt.Vertical) ? units.gu(2) : units.gu(1)
        bottomMargin: (direction == Qt.Vertical) ? units.gu(2) : units.gu(1)
    }

    // direction verification
    onDirectionChanged: {
        if (direction != Qt.Horizontal && direction != Qt.Vertical) {
            console.error(i18n.tr("Value entered is not one of the supported values."), direction);
        }
    }

    // layout mirroring configuration!
    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true
}
