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
        FramedImage {
            id: defaults
        }
        FramedImage {
            id: testItem
        }
        ListItem {
            id: listItem
            ListItemLayout {
                FramedImage {
                    id: image
                    shape.color: UbuntuColors.blue
                }
            }
        }
    }

    UbuntuTestCase {
        name: "FramedImageAPI"
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
            clickSpy.target = null;
            clickSpy.clear();
            listItemClickSpy.clear();
            testItem.enabled = false;
            image.enabled = false;
        }

        function test_0_defaults() {
            verify(QuickUtils.inherits(defaults.shape, "ShapeItem"), "shape is not UbuntuShape!");
            compare(defaults.overlay.length, 0, "Nothing in teh overlay");
            compare(defaults.enabled, false, "Container must be disabled by default");
            compare(defaults.shape.color, Qt.rgba(0, 0, 0, 0), "No color is set to container");
            compare(defaults.shape.image, null, "No image is set to container");
            compare(defaults.visible, false, "Container is not visible when no color or image set.");
        }

        function test_captions_clisk_data() {
            return [
                {tag: "disabled container", enabled: false},
                {tag: "enabled container", enabled: true},
            ];
        }
        function test_captions_clisk(data) {
            image.enabled = data.enabled;
            clickSpy.target = image;
            mouseClick(image, centerOf(image).x, centerOf(image).y);
            if (data.enabled) {
                clickSpy.wait();
                compare(listItemClickSpy.count, 0, "Click must be suppressed when container is enabled.");
            } else {
                listItemClickSpy.wait();
                compare(clickSpy.count, 0, "Container should not emit clicked signal when disabled.");
            }
        }
    }
}
