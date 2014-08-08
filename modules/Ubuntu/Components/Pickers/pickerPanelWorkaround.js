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

.import "../Popups/popupUtils.js" as PopupUtils

function openDatePicker(caller, property, mode) {
    if (mode === undefined) {
        mode = "Years|Months|Days";
    }
    var params = {
        "date": caller[property],
        "pickerMode": mode,
        "callerProperty": property
    }

    var rootItem = QuickUtils.rootItem(null)
    var isPhone = rootItem.width <= units.gu(40) && rootItem.height <= units.gu(71) && (QuickUtils.inputMethodProvider !== "")

    if (!isPhone) {
        // we have no input panel defined, or the therefore we show the picker in a Popover
        return PopupUtils.open(Qt.resolvedUrl("pickerPanelPopover.qml"), caller, params);
    }
    // OSK panel
    var component = Qt.createComponent(Qt.resolvedUrl("pickerPanelPanel.qml"));
    return component.createObject(QuickUtils.rootItem(null), params);
}
