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
  \qmltype ImageContainer
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \brief Container providing presets for a framed image that can be used in
  \l ListItemLayout.

  The component is a container which clips an image into a \l shape. The container
  has a fixed width of 5 grid units and a height as its parent, and the shape is
  centered into the container. Similarly to \l Captions, the container is not
  visible thus not occupying space in the layout when the shape has no color or
  image set. As well as in \l Captions, altering the visible property must be
  handled with states to preserve property handling.

  */
Item {
    id: imageContainer

    /*!
      \qmlproperty Shape shape
      \readonly
      Container for the image or color.
      */
    property alias shape: frameItem

    enabled: false
    clip: true
    Layout.alignment: Qt.AlignVCenter
    width: units.gu(5)
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height
    visible: (frameItem.image || (frameItem.color.a > 0))

    Shape {
        id: frameItem
        width: parent.width
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#00000000"
    }
}
