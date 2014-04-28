/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0

TextField {
    id: control

    /*!
      Provides extended properties to the input method allowing to change the default visual of the action Key,
      the value is a dictonary and the valid keys for the dictionary are:
      \list
      \li - enterKeyText (string) - The text to be displayed in the "Enter" key of the virtual keyboard
      \li - enterKeyEnabled (boolean) - If the "Enter" key of the virtual keyboard should be enabled or not
      \li - enterKeyHighlighted (boolean) - If the "Enter" key of the virtual keyboard should be highlighted or not
      \endlist

      \qml
      TextField {
          text: "Text"

          inputMethodExtensions: {
            'enterKeyText': 'My Enter',
            'enterKeyEnabled': true,
            'enterKeyHighlighted': true,
          }
      }
      \endqml

      \qmlproperty var inputMethodExtensions
    */
    property alias inputMethodExtensions: __editor.__inputMethodExtensions
}
