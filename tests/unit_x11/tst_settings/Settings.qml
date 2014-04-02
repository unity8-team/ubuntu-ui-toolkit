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
import Ubuntu.Components 0.1

MainView {
    applicationName: "tst_settings"

    Settings {
        id: appSettings
        objectName: "settings"

        Option {
            id: boolFalse
            name: "boolFalse"
            objectName: "boolFalse"
            defaultValue: false
        }
        Option {
            id: boolTrue
            name: "boolTrue"
            objectName: "boolTrue"
            defaultValue: true
        }
        Option {
            id: stringEmpty
            name: "stringEmpty"
            objectName: "stringEmpty"
            defaultValue: ""
        }
        Option {
            id: stringUrl
            name: "stringUrl"
            objectName: "stringUrl"
            defaultValue: "http://www.ubuntu.com"
        }
        Option {
            id: intZero
            name: "intZero"
            objectName: "intZero"
            defaultValue: 0
        }
        Option {
            id: intMax
            name: "intMax"
            objectName: "intMax"
            defaultValue: 65535
        }
        Option {
            id: doubleAgent
            name: "doubleAgent"
            objectName: "doubleAgent"
            defaultValue: 5.5
        }
    }

    Page {
        title: "Settings"

        Switch {
            objectName: "boolFalseComponent"
            action: boolFalse
        }
        Switch {
            objectName: "boolTrueComponent"
            action: boolTrue
        }
        TextField {
            objectName: "stringEmptyComponent"
            action: stringEmpty
        }
        TextField {
            objectName: "stringUrlComponent"
            action: stringUrl
        }
        TextField {
            objectName: "intZeroComponent"
            action: intZero
        }
        TextField {
            objectName: "intMaxComponent"
            action: intMax
        }
        TextField {
            objectName: "doubleAgentComponent"
            action: doubleAgent
        }
    }
}

