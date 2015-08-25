/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

ContextMenu {
    Label {
        text: i18n.tr('Comic Sans is awesome')
    }
    MenuGroup {
        Action {
            text: i18n.tr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.")
        }
        Action {
            text: i18n.tr("Once upon a time there was a story nobody told.")
        }
    }
    MenuGroup {
        Action {
            iconName: 'delete'
            text: i18n.tr("Delete")
        }
        MenuGroup {
            text: i18n.tr("But wait, there's more...")
            Action {
                iconName: 'compose'
                text: i18n.tr("Send an email")
            }
            Action {
                iconName: 'save-to'
                text: i18n.tr("Save to file")
            }
        }
    }
}

