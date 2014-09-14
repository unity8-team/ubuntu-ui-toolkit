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
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup new-ubuntu-listitems
  \brief Container providing presets for a framed image that can be used in
  \l ListItemLayout.

  The image is embedded and clipped into an UbuntuShape.
  */
ListItemContainer {
    id: framedImage

    property color color

    property Image image: null

    property string iconName

    /*!
      \qmlproperty Item frame
      */
    property alias frame: frameLoader

    visible: frameLoader.item != null
    preset: "leading"
    Layout.preferredWidth: Layout.maximumWidth
    Loader {
        id: frameLoader
        width: framedImage.Layout.maximumWidth
        height: framedImage.Layout.maximumHeight
        anchors.horizontalCenter: parent.horizontalCenter
    }

    onColorChanged: frameLoader.sourceComponent = frameComponent
    onImageChanged: {
        if (image) {
            frameLoader.sourceComponent = frameComponent
        } else {
            frameLoader.sourceComponent = null
        }
    }
    onIconNameChanged: {
        if (!frameLoader.item) {
            frameLoader.sourceComponent = frameComponent
        }
    }

    Component {
        id: frameComponent
        Item {
            UbuntuShape {
                id: frameItem
                image: framedImage.image ? framedImage.image : icon
                anchors.fill: parent
            }
            Image {
                id: icon
                visible: false
                anchors.fill: parent
                source: !framedImage.image ? "image://theme/" + framedImage.iconName : ""
                fillMode: Image.PreserveAspectCrop
                smooth: true
                asynchronous: true
            }
        }
    }
}
