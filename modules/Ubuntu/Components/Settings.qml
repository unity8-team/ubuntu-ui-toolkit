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
            sync: false
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
    onGroupChanged: {
        if (__doc.create)
            console.log('Settings groups must not change after declaration!');
    }

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
    onOptionsChanged: {
        if (!SettingsStorage.addGroup(group, settings)) {
            console.log("Non-unique Settings declared with the group '%1'".arg(group));
            return;
        }
        var defaultValues = {};
        for(var item in options) {
            var child = options[item];
            if (child.name != null && child.defaultValue != null) {
                defaultValues[child.name] = child.defaultValue;
                child.__doc = __doc;
            } else {
                console.log("Ignoring incomplete Option declaration %1 in %2"
                    .arg(child.name ? child.name : child.objectName).arg(group));
            }
        }
        __doc.defaults = defaultValues;
        __doc.create = true;
    }

    U1db.Document {
        id: __doc
        docId: group
        create: false
        onContentsChanged: {
            if (!__doc.contents)
                return;
            for (var item in options) {
                var child = options[item];
                child.value = __doc.contents[child.name];
            }
        }
        database: U1db.Database {
            path: "settings.db"
        }
    }
}
