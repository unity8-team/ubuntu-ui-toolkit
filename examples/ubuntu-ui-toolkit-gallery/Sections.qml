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

Template {
    objectName: "sectionsTemplate"

    TemplateSection {
        title: "Sections"
        className: "Sections"

        Label {
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: "This is a special example which can be used to reproduce one of the bugs I have with the keyboard navigation in the Sections component. Just click on the first item, then press and hold the right arrow on your keyboard, then press and hold LEFT on your keyboard. You'll notice that the first (or last) element will align to the middle when it actually shouldn't. I believe this happens because contentWidth/X changes is changed on c++ side after I set it on qml side. This is just one of the bugs which happen as a consequence of us playing with contentX to scroll the view, because ListView doesn't provide a way to scroll a view WITH an animation. PositionViewAtIndex(..) is reliable BUT it does NOT animate, and it only works with an index."
        }

        TemplateRow {
            title: i18n.tr("Bug1")

            Sections {
                actions: [
                    Action { text: "two" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneo" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneon" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneon" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "two" }
                ]
            }
        }

        TemplateRow {
            title: i18n.tr("Bug1")

            Sections {
                actions: [
                    Action { text: "two" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneo" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneon" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneon" },
                    Action { text: "oneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "two" },
                    Action { text: "three" },
                    Action { text: "four" },
                    Action { text: "five" },
                    Action { text: "six" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneonev" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneone" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "seven" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "sevenoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneoneone" },
                    Action { text: "seven" }
                ]
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")

            Sections {
                model: ["one", "two", "three"]
                enabled: false
            }
        }
    }
}
