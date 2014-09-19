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
  \qmltype Captions
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup new-ubuntu-listitems
  \brief Container providing presets for a twin-label colun that can be used in
  \l ListItemLayout.

  The labels are placed in a column layout and can be configured through \l title
  and \l subtitle properties. The label's main configuration is provided by the
  \l preset property.

  The Captions is not visible if neither the label content is set. Altering \c
  visible property may destroy the handling. Therefore should be altered only
  through \c PropertyChanges so visibility control is preserved.
  \qml
  import QtQuick 2.2
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.1

  Item {
      width: units.gu(40)
      height: units.gu(7)

      Captions {
          enabled: true
          onClicked: details.state = (details.state == "hidden") ? "" : "hidden"
          title.text: "Caption text"
      }
      Captions {
          id: details
          preset: "details"
          title.text: "Details"
          states: State {
              name: "hidden"
              PropertyChanges {
                  target: details
                  visible: false
              }
          }
      }
  }
  \endqml
  */
MouseArea {
    id: captions

    /*!
      The property configures the arrangement and font sizes of the Labels in the
      component. It can take the following values:
      \list
        \li \b main - large title fontSize, small subtitle fontSize, both aligned
            to left and the container fills the remainder space on the layout.
        \li \b details - medium title fontSize, small subtitle fontSize, both aligned
            to right and the container can take a maximum of 6 grid units.
      \endlist
      Defaults to \b main.
      */
    property string preset: "main"

    /*!
      \qmlproperty Label title
      Label occupying the top area of the container.
      */
    property alias title: titleLabel

    /*!
      \qmlproperty Label subtitle
      Label occupying the bottom area of the container.
      */
    property alias subtitle: subtitleLabel

    clip: true
    enabled: false
    visible: (title.text !== "" || subtitle.text !== "")
    Layout.fillWidth: (preset === "main")
    Layout.alignment: Qt.AlignVCenter
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "details") ? units.gu(6) : parent.width
    Layout.preferredWidth: (preset === "details") ? Layout.maximumWidth : childrenRect.width
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height

    ColumnLayout {
        spacing: units.gu(0.5)
        anchors {
            left: parent.left
            right: parent.right
        }
        Layout.preferredHeight: childrenRect.height

        Label {
            id: titleLabel
            fontSize: (captions.preset === "main") ? "large" : "medium"
            Layout.alignment: (captions.preset === "details") ? Qt.AlignRight : Qt.AlignLeft
            visible: text !== ""
        }
        Label {
            id: subtitleLabel
            fontSize: "small"
            Layout.alignment: (captions.preset === "details") ? Qt.AlignRight : Qt.AlignLeft
            visible: text !== ""
        }
    }
}
