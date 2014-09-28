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
import "mathUtils.js" as MathUtils

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
               preset: "summary"
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
          preset: "summary"
          title.text: "summary"
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
Item {
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
    property string preset: "caption"

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
      \qmlproperty Column layout
      Property holding the layout.
      */
    property alias layout: layoutContainer

    clip: true
    visible: (title.text !== "" || subtitle.text !== "")
    Layout.fillWidth: (preset === "caption")
    Layout.alignment: Qt.AlignVCenter | ((preset == "summary") ? Qt.AlignRight : Qt.AlignLeft)
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "summary") ? units.gu(6) : parent.width
    Layout.preferredWidth: (preset === "summary") ? Layout.maximumWidth : 0
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height

    Column {
        id: layoutContainer
        anchors {
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        clip: true
        spacing: (preset === "caption") ?
                     ((subtitleLabel.lineCount > 1) ? units.gu(0.1) : units.gu(0.5)) :
                     units.gu(0.5)

        Label {
            id: titleLabel
            anchors {
                left: parent.left
                right: parent.right
            }
            fontSize: "medium"
            horizontalAlignment: (preset === "summary") ? Text.AlignRight : Text.AlignLeft
            visible: text !== ""
            elide: (preset === "caption") ? Text.ElideRight : Text.ElideNone
        }
        Label {
            id: subtitleLabel
            anchors {
                left: parent.left
                right: parent.right
            }
            fontSize: (lineCount > 1) ? "xx-small" : "x-small"
            horizontalAlignment: (preset === "summary") ? Text.AlignRight : Text.AlignLeft
            visible: text !== ""
            maximumLineCount: (preset === "caption") ? 2 : 1
            wrapMode: (preset === "caption") ? Text.Wrap : Text.NoWrap
            elide: (preset === "caption") ? Text.ElideRight : Text.ElideNone
        }
    }
    Rectangle {anchors.fill: parent; color: "transparent"; border.width: 1}
}
