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
import QtTest 1.0
import Ubuntu.Test 0.1

UbuntuTestCase {
    name: "Settings"

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

        Page {
            title: "Settings"

            TextField {
                id: textFieldUrl
                action: stringUrl
            }
        }
    }

    SignalSpy {
        id: changeSpy
        target: findInvisibleChild('settingsInternalDocument')
        signalName: "contentsChanged"
    }

    function test_defaults () {
        var expected = ['boolFalse', 'boolTrue', 'stringEmpty', 'stringUrl', 'intMax', 'intZero']
        for(var i in expected) {
            var found = false
            var name = expected[i]
            for(var item in appSettings.options) {
                var option = appSettings.options[item]
                if (option.name != name)
                    continue
                found = true
            }
            if (!found)
                fail('Expected setting %1 not found in options'.arg(name))
        }
        for(var item in appSettings.options) {
            var option = appSettings.options[item]
            compare(option.value, option.defaultValue)
        }
        var internalDocument = findInvisibleChild('settingsInternalDocument')
        compare(internalDocument.create, true)
    }

    function change_values () {
        stringUrl.value = "http://www.canonical.com"
        changeSpy.wait()
        intMax.value = 13
        changeSpy.wait()
    }

    function verify_values () {
        compare(stringUrl.value, "http://www.canonical.com")
        compare(intMax.value, 13)

        // Components bound to settings should default to their value
        compare(textFieldUrl.text, stringUrl.value)
    }
}
