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
import Ubuntu.Components 1.1

/*!
  \qmltype FramedImage
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \brief Container providing presets for a framed image that can be used in
  \l ListItemLayout.

  The component, similarly to \l Captions, is a container which clips an image
  into a \l shape. Derived from MouseArea, it can handle mouse events over the shape
  when enabled. Also similarly to \l Captions, the default state is disabled, meaning
  no mouse events are handled.

  Another similarity to \l Captions is the handling of visibility. The container
  is hidden when no color or image is specified to the shape. Therefore visible
  property should be handled using PropertyChange to preserve internal visible
  handling.

  The component also provides the possibility to place an item overlay. This can
  be useful when additional content needs to be provided over the image or shape.
  \qml
  FramedImage {
       shape.color: UbuntuColors.blue
       overlay: Label {
           anchors.fill: parent
           horizontalAlignment: Text.AlignHCenter
           verticalAlignment: Text.AlignVCenter
           text: "XY"
       }
  }
  \endqml
  */
MouseArea {
    id: framedImage

    /*!
      \qmlproperty UbuntuShape shape
      Container for the image or color
      */
    property alias shape: frameItem

    /*!
      \qmlproperty list<QtObject> overlay
      The property holds items placed overlay the shape.
      */
    property alias overlay: overlayItem.data

    enabled: false
    clip: true
    Layout.alignment: Qt.AlignVCenter
    width: units.gu(5)
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height
    visible: (frameItem.image || (frameItem.color.a > 0))

    UbuntuShape {
        id: frameItem
        width: parent.width
        height: framedImage.Layout.maximumHeight
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#00000000"
        image: framedImage.image
        Item {
            id: overlayItem
            anchors.fill: parent
        }
    }
}
