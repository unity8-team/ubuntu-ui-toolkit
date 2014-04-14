/*
 * Copyright 2013 Canonical Ltd.
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
import U1db 1.0 as U1db
import Ubuntu.Components 0.1 as Toolkit

/*!
    \qmltype Settings
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Application-specific settings.

    Settings are an easy way for an application to keep values on one or more
    devices, with optional grouping. If desired, actions can be associated with
    individual settings to provide UI.

    Example:
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    MainView {
        applicationName: "example"
        Settings {
            group: "sound"
            Option {
                name: "vibrate"
                defaultValue: false
            }
        }
        Settings {
            group: "tablet"
            Option {
                name: "bigLayout"
                defaultValue: false
            }
        }
    }
    \endqml
*/
Item {
    id: settings
    /*!
      An optional group for these settings.
      */
    property string group: "default"

    /*!
      \internal
      */
    property list<Option> couldntGetListToWorkWithoutAlias

    /*!
      The list of options. Most of the time it's enough to simply declare
      each Option within the Settings.
      */
    default property alias options: settings.couldntGetListToWorkWithoutAlias

    /*!
      \internal
      */
    Component.onCompleted: {
        for(var item in options)
            options[item].__internal.group = group;
    }
}
