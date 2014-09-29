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
  \qmltype StandardLayout
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \brief Helper component composing a \l ImageContainer and two \l Captions containers,
  one for the titles and one for details.

  The three building elements are accessible and configurable through the \l image,
  \l captions and \l details properties. The following example illustrates how the
  StandardLayout helps on easing your code:
  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  Column {
      // remember to set a width for the ListItem parent!
      width: units.gu(40)
      ListItem {
          ListItemLayout {
              ImageContainer {
                  shape.color: UbuntuColors.blue
              }
              Captions {
                  title.text: "Title text"
                  subtitle.text: "Subtitle text"
              }
              Captions {
                  preset: "details"
                  title.text: "Text"
                  subtitle.text: "Text"
              }
          }
      }
      ListItem {
          StandardLayout {
              image.shape.color: UbuntuColors.blue
              captions {
                  title.text: "Captions (title)"
                  subtitle.text: "Subtitle text"
              }
              details {
                  title.text: "Text"
                  subtitle.text: "Text"
              }
          }
      }
  }
  \endqml

  Additional containers or components can be added to the layout by simply declaring
  it as child elements, however they cannot be inserted in between the standard
  building ones. To do that you must use \l ListItemLayout and declare the containers
  in the desired order.

  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  Column {
      // remember to set a width for the ListItem parent!
      width: units.gu(40)
      ListItem {
          ListItemLayout {
              ImageContainer {
                  shape.color: UbuntuColors.blue
              }
              Captions {
                  title.text: "Title text"
                  subtitle.text: "Subtitle text"
              }
              Switch {
              }
              Captions {
                  preset: "details"
                  title.text: "Text"
                  subtitle.text: "Text"
              }
          }
      }
      ListItem {
          StandardLayout {
              image.shape.color: UbuntuColors.blue
              captions {
                  title.text: "Captions (title)"
                  subtitle.text: "Subtitle text"
              }
              details {
                  title.text: "Text"
                  subtitle.text: "Text"
              }
              Switch {
              }
          }
      }
  }
  \endqml
  \section3 Activating components
  In the examples above, the \l Switch declared in the layout is toggled only when
  clicked above it. Tapping anywhere else outside the \l Switch area will not
  activate the component. There can be situations when toggling the \l Switch is
  desired wherever the user taps over the \l ListItem area. This can be achieved
  by connecting the \c clicked() signals of the \l ListItem and \l Switch components.
  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  ListItem {
      width: units.gu(40)
      StandardLayout {
          image.shape.color: UbuntuColors.blue
          captions {
              title.text: "Captions (title)"
              subtitle.text: "Subtitle text"
          }
          details {
              title.text: "Text"
              subtitle.text: "Text"
          }
          Switch {
              id: toggle
          }
      }
      Component.onCompleted: clicked.connect(toggle.clicked)
  }
  \endqml

  Beware that enabling a container will suppress the click signal from \l ListItem.
  \qml
  import QtQuick 2.2
  import Ubuntu.Components 1.2

  ListItem {
      width: units.gu(40)
      StandardLayout {
          image {
              shape.color: UbuntuColors.blue
              enabled: true
              onClicked: console.log("clicked over the image, Switch is not toggled")
          }
          captions {
              title.text: "Captions (title)"
              subtitle.text: "Subtitle text"
          }
          details {
              title.text: "Text"
              subtitle.text: "Text"
          }
          Switch {
              id: toggle
          }
      }
      Component.onCompleted: clicked.connect(toggle.clicked)
  }
  \endqml
  */
ListItemLayout {
    /*!
      \qmlproperty ImageContainer image
      The property holds the instance of the \l ImageContainer container.
      */
    property alias image: imageContainer

    /*!
      \qmlproperty Captions caption
      The property holds the \l Captions container with \b defaults preset.
      */
    property alias caption: captionsContainer

    /*!
      \qmlproperty Captions summary
      The property holds the \l Captions container with \b details preset.
      */
    property alias summary: summaryContainer

    ImageContainer {
        id: imageContainer
    }
    Captions {
        id: captionsContainer
    }
    Captions {
        id: summaryContainer
        preset: "summary"
    }
}
