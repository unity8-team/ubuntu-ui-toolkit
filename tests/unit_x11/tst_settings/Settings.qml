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
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "Settings"

    MainView {
        applicationName: "tst_settings"

        Settings {
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
                defaultValue: ""
            }
            Option {
                id: stringUrl
                name: "stringUrl"
                defaultValue: "http://www.ubuntu.com"
            }
            Option {
                id: intZero
                name: "intZero"
                defaultValue: 0
            }
            Option {
                id: intMax
                name: "intMax"
                defaultValue: 65535
            }
        }
        TextField {
            id: textFieldUrl
            action: stringUrl
        }
    }

    function test_defaults () {
        for(var item in resources) {
            var child = resources[item]
            if (child.hasOwnProperty("defaultValue"))
                compare(child.value, child.defaultValue)
        }
    }

    function change_values () {
        stringUrl.value = "http://www.canonical.com"
        intMax.value = 13
    }

    function verify_values () {
        compare(stringUrl.value, "http://www.canonical.com")
        compare(intMax.value, 13)

        console.log("now tFu.t")
        // Components bound to settings should default to their value
        compare(textFieldUrl.text, stringUrl.value)
    }
}
