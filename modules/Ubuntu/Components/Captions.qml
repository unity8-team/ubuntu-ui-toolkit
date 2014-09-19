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
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \brief Container providing presets for a twin-label colun that can be used in
  \l ListItemLayout.

  The labels are placed in a column layout and can be configured through \l title
  and \l subtitle properties. The spacing between the labels is driven by the
  \l spacing property. \l preset property configures these labels.

  The container only shows the labels whos text is set to a valid string. The
  labels not having any text set are not occupying the space. The labels are
  arranged vertically centered, which means if one label is set, that one will
  be shown centered.

  Being derived from MouseArea, Captions provides the ability to handle mouse
  related events on the area covered. When used with the ListItem, the component
  can provide different handling when pressed over or on other areas of the
  ListItem. By default, the container is disabled. When enabled, the ListItem
  clicked signal is suppressed when clicked over the container.
  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  ListItem {
      ListLayout {
           Captions {
               enabled: true
               onClicked: console.log("Captions clicked")
           }
           Switch {
           }
           onClicked: console.log("clicked on ListItem")
      }
  }
  \endqml

  Properties of the labels can be altered by overriding the properties of individual
  labels.
  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  ListItem {
      ListLayout {
           Captions {
               title.text: "Caption"
               subtitle.text: "Subtitle text"
               Component.onCompleted: subtitle.Layout.alignment = Qt.AlignRight
           }
           Captions {
               preset: "details"
               title.text: "Text"
               subtitle.text: "Text"
           }
      }
  }
  \endqml

  Additional items can also be added to the layout after the two labels. These
  can be simply declared as child items to the container.
  \qml
  Captions {
       title.text: "Caption"
       subtitle.text: "Subtitle"
       Label {
           text: "third line"
           fontSize: "x-small"
       }
  }
  \endqml

  The Captions is not visible if neither the label content is set. Altering \c
  visible property may destroy the handling. Therefore should be altered only
  through \c PropertyChanges so visibility control is preserved.
  \qml
  import QtQuick 2.2
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.2

  ListItem {
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
        \li \b titles - (default) large title fontSize, small subtitle fontSize, both aligned
            to left and the container fills the remainder space on the layout.
        \li \b details - medium title fontSize, small subtitle fontSize, both aligned
            to right and the container can take a maximum of 6 grid units.
      \endlist
      */
    property string preset: "titles"

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

    /*!
      \qmlproperty list<QtObject> data
      \default
      Default property holding the data of the container.
      */
    default property alias data: layout.data

    /*!
      \qmlproperty real spacing
      The property configures the spacing between the contained labels. The default
      spacing is 0.5 grid units.
      */
    property alias spacing: layout.spacing

    clip: true
    enabled: false
    visible: (title.text !== "" || subtitle.text !== "")
    Layout.fillWidth: (preset === "titles")
    Layout.alignment: Qt.AlignVCenter
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "details") ? units.gu(6) : parent.width
    Layout.preferredWidth: (preset === "details") ? Layout.maximumWidth : childrenRect.width
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height

    ColumnLayout {
        id: layout
        spacing: units.gu(0.5)
        anchors {
            left: parent.left
            right: parent.right
        }
        Layout.preferredHeight: childrenRect.height

        Label {
            id: titleLabel
            fontSize: (captions.preset === "titles") ? "large" : "medium"
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
