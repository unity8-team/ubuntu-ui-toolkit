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
import Ubuntu.Components 0.1

Template {
    id: templateSlider
    TemplateSection {
        id: templateSection1
        className: "Slider"
        spacing: units.gu(5)

        TemplateFlow {
            id: templateFlow1
            title: i18n.tr("Standard")

            Slider {
                id: defaultSlider
                objectName: "slider_standard"
                width: parent.width
                Component.onCompleted: print("SLIDER defaultSlider COMPLETED", value)
                onValueChanged: print("SLIDER  defaultSlider VALUE CHANGED", value)
                value: 10
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(defaultSlider.value)
                font.weight: Font.Light
            }
        }

        TemplateFlow {
            id: templateFlow2
            title: i18n.tr("Live Update")

            Slider {
                id: liveSlider
                objectName: "slider_live"
                width: parent.width
                live: true
                Component.onCompleted: print("SLIDER liveSlider COMPLETED", value)
                onValueChanged: print("SLIDER  liveSlider VALUE CHANGED", value)
                StateSaver.enabled: true
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(liveSlider.value)
                font.weight: Font.Light
            }
        }
/*
        TemplateFlow {
            title: i18n.tr("Range")

            Slider {
                id: rangeSlider
                objectName: "slider_range"
                width: parent.width
                minimumValue: -1.0
                maximumValue: 1.0
                function formatValue(v) { return Number(v.toFixed(2)).toLocaleString(Qt.locale()) }
            }
            Label {
                text: i18n.tr("Actual value: %1").arg(rangeSlider.value)
                font.weight: Font.Light
            }
        }*/
    }
}
