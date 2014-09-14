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
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup new-ubuntu-listitems
  \brief Standard ListItem layout for vertical flowing lists.

  The component is a simple RowLayout which provides positioning of the different
  containers, which can either be \l ListItemContainer elements or any other element.
  It fills the entire item it is embedded in, with 1 grid unit top and bottom margin,
  2 grid units left and right margins. The spacing in between the containers is 1 grid
  unit.
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
