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

  The component is a simple RowLayout providing positioning of the different containers,
  which can be one of the predefined containers or any other component. It fills
  the entire item it is embedded in, with 1 grid unit margin on top and bottom,
  as well as 2 grid units margin on left and right. The spacing in between the
  containers is 1 grid unit. Component sizes can be driven using the layouting
  mechanism provided by QtQuick.

  It is designed to be used with \l ListItem, however it can be used with any
  Item in any context.

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
RowLayout {
    id: layout

    spacing: units.gu(1)
    anchors {
        fill: parent
        leftMargin: units.gu(2)
        rightMargin: units.gu(2)
        topMargin: units.gu(1)
        bottomMargin: units.gu(1)
    }
}
