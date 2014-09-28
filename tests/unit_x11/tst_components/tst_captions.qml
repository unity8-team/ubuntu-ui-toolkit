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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1

Item {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        width: parent.width
        Captions {
            id: defaults
        }
        Captions {
            id: testCaptions
        }
        ListItem {
            id: listItem
            ListItemLayout {
                id: layout
                Captions {
                    id: titles
                    title.text: "Text"
                }
                Switch {
                    id: testComponent
                }
            }
        }
    }

    UbuntuTestCase {
        name: "CaptionsAPI"
        when: windowShown

        SignalSpy {
            id: listItemClickSpy
            target: listItem
            signalName: "clicked"
        }

        SignalSpy {
            id: clickSpy
            signalName: "clicked"
        }

        function cleanup() {
            titles.enabled = false;
            listItemClickSpy.clear();
            clickSpy.clear();
        }

        function test_0_defaults() {
            compare(defaults.preset, "", "Default preset is 'titles'");
            compare(defaults.height, 0, "default height is 0");
            compare(defaults.width, 0, "default width is 0")
            compare(defaults.Layout.alignment, Qt.AlignVCenter, "center vertically by default");
            compare(defaults.spacing, units.gu(0.5), "default spacing failure");
        }

        function test_captions_data() {
            return [
                {tag: "titles, title", preset: "titles", label: "title", fontSize: "large", alignment: Qt.AlignLeft},
                {tag: "titles, subtitle", preset: "titles", label: "subtitle", fontSize: "small", alignment: Qt.AlignLeft},
                {tag: "details, title", preset: "details", label: "title", fontSize: "medium", alignment: Qt.AlignRight},
                {tag: "details, subtitle", preset: "details", label: "subtitle", fontSize: "small", alignment: Qt.AlignRight},
            ];
        }
        function test_captions(data) {
            testCaptions.preset = data.preset;
            compare(testCaptions[data.label].fontSize, data.fontSize, "fontSize differs");
            compare(testCaptions[data.label].Layout.alignment, data.alignment, "alignment differs");
        }

        function test_captions_clisk_data() {
            return [
                {tag: "disabled container", enabled: false},
                {tag: "enabled container", enabled: true},
            ];
        }
        function test_captions_clisk(data) {
            titles.enabled = data.enabled;
            clickSpy.target = titles;
            mouseClick(titles, centerOf(titles).x, centerOf(titles).y);
            if (titles.enabled) {
                clickSpy.wait();
                compare(listItemClickSpy.count, 0, "Click must be suppressed when container is enabled.");
            } else {
                listItemClickSpy.wait();
                compare(clickSpy.count, 0, "Container should not emit clicked signal when disabled.");
            }
        }
    }
}
