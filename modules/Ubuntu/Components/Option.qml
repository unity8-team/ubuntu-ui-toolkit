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

import QtQuick 2.0
import U1db 1.0 as U1db
import Ubuntu.Unity.Action 1.0 as UnityActions

/*!
    \qmltype Option
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief An option within application-specific settings.

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
            Option {
                id: optionNickname
                name: "nomDePlum"
                defaultValue: "A. Nonymous"
                text: i18n.tr("Pseudonym")
                visible: proFeaturesUnlocked // variable defined in the application
            }
        }

        Column {
            Row {
                Label {
                    text: optionVibrate.text
                }
                Switch {
                    action: optionVibrate
                }
            }
            Row {
                Label {
                    text: optionNickname.text
                }
                TextField {
                    action: optionNickname
                }
            }
        }
    }
    \endqml

    See Settings for more examples.
*/
Action {
    id: option
    /*!
      A name for the setting that is unique in the group.
      */
    property var name: null
    /*!
      The default value. Any basic type is allowed. After the first time an
      option is used on one particular device, the default is ignored unless
      the group is reset.
      */
    property var defaultValue: null
    /*!
      The current value, either a default, locally saved state or synchronized.
      */
    property var value: defaultValue

    /*!
      \internal
      The default value changed. From this we assume the value type.
      */
    onDefaultValueChanged: {
        if (typeof defaultValue == "string") {
            parameterType = UnityActions.Action.String;
        } else if (typeof defaultValue == "number") {
            parameterType = UnityActions.Action.Real; // Javascript doesn't distinguish int and real
        } else if (typeof defaultValue == "boolean") {
            parameterType = UnityActions.Action.Bool;
        } else {
            parameterType = UnityActions.Action.None;
        }
    }

    /*!
      \internal
      Store the new value when the action is triggered.
      */
    onTriggered: option.value = value

    /* Can't declare anonymous Object because parent class is no Item */
    property var __internal: Item {
        property string group: ""
        onGroupChanged: {
            if (name == null || defaultValue == null) {
                console.log("Incomplete Option declaration %1 in %2".arg(objectName).arg(group));
            } else {
                state = "valid";
            }
        }
        states: State {
            name: "valid"
            PropertyChanges {
                target: doc
                docId: "%1-%2".arg(__internal.group).arg(name)
            }
            PropertyChanges {
                target: option
                value: doc.contents["value"]
                onValueChanged: doc.contents = { "value": value }
            }
        }
        U1db.Document {
            id: doc
            database: U1db.Database {
                path: "settings.db"
            }
        }
    }
}

