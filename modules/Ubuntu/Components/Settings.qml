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
    Settings {
        group "sound
        Option {
            name: "vibrate"
            defaultValue: false
        }
    }
    Settings {
        group "tablet"
        sync: false
        persistent: !isPrivateMode // variable defined in the application
        Option {
            name "bigLayout"
            defaultValue: false
        }
    }
    \endqml
*/
Item {
    /*!
      An optional group for these settings.
      */
    property string group: "default"
    /*!
      By default all settings transparently store on disk and sync to the cloud
      if a U1 account is setup. Set to false to keep settings local.
      */
    property bool sync: true
    /*!
      By default values are stored on disk and updated as needed. Set to false
      to prevent that behavior - for example for a private mode use case. This
      doesn't interfere with sync behavior.
      */
    property bool persistent: true

    U1db.Database {
        id: __database
        path: persistent ? "settings.db" : null
    }
}
