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
  \qmltype ListItemContainer
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup new-ubuntu-listitems
  \brief Container providing presets for different use cases in a \l ListItemLayout.

  The presets configure the container with the given constraints for different
  use cases as described in \l preset property. Vertically all containers can
  have the same maximum value, which is its parent height, and are vertically
  aligned centered to their parent. It is recommended to be used with \l ListItemLayout.
  There is no other constraint than design on what is the order these containers
  can be declared in the layout.

  The containers do not take space if they are not visible or have no content.
  This means that other containers in the layout can reuse the space otherwise
  occupied by the invisible one.
  \qml
  import QtQuick 2.2
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.1

  Item {
      width: units.gu(40)
      height: units.gu(7)

      ListItemLayout {
          ListItemContainer {
              id: leading
              preset: "leading"
              UbuntuShape {
                  width: units.gu(4)
                  height: width
                  color: UbuntuColors.blue
              }
          }
          ListItemContainer {
              preset: "captions"
              Label {
                  text: "Text occupying the entire layout\n" +
                        "Click to hide/show leading container"
              }
              enabled: true
              onClicked: leading.visible = !leading.visible
          }
      }
  }
  \endqml

  When components placed inside the containers cannot anchor fill to the container,
  as the container is sized based on its children components. Components can use the
  container's Layout attached object to access the maximum values, so they can adjust
  their size based on those values.
  \qml
  import QtQuick 2.2
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.1

  Item {
      width: units.gu(40)
      height: units.gu(7)

      ListItemLayout {
          ListItemContainer {
              preset: "leading"
              UbuntuShape {
                  width: parent.Layout.maximumWidth
                  height: parent.Layout.maximumHeight
                  color: UbuntuColors.blue
              }
          }
          ListItemContainer {
              preset: "captions"
              Label {
                  text: "Blue shape"
              }
          }
      }
  }
  \endqml
  */

MouseArea {
    id: container

    /*!
      The property configures the container depending on what it is used for.
      It can take the following values:
      \list
        \li \b leading - when set, the maximum width the container can take
            is set to 5 grid units, and its content is also aligned horizontally
            centered to the layout. Affected Layout properties: \c maximumWidth.
        \li \b trailing - when set, the maximum width the container can take
            is set to 6 grid units. Affected Layout properties: \c maximumWidth.
        \li \b captions - when this preset is used, the container's maximum
            possible width can be the entire parent width, and the content will
            also be filled as wide as possible. Affected Layout properties:
            \c maximumWidth, \c fillWidth.
      \endlist
      \note These presets can be overruled on individual properties by simply
      defining values to the affected Layout attached properties. Example of
      overruling the maximum width:
      \qml
      import QtQuick 2.2
      import Ubuntu.Components 1.1
      import QtQuick.Layouts 1.1

      ListItemContainer {
          preset: "leading"
          Layout.maximumWidth: units.gu(10)
      }
      \endqml
      */
    property string preset: ""

    Layout.fillWidth: (preset === "captions")
    Layout.alignment: Qt.AlignVCenter
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "leading") ?
                            units.gu(5) :
                            (preset === "trailing") ?
                                 units.gu(6) :
                                 parent.width
    Layout.preferredWidth: childrenRect.width

    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height

    clip: true
    enabled: false

//    Rectangle { anchors.fill: parent; border.width: 1}
}
