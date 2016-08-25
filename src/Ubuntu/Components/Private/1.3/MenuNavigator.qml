/*
 * Copyright 2016 Canonical Ltd.
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
 *
 * Author: Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.4

QtObject {
    id: root
    // items to navigate through
    property QtObject model

    signal select(int index)

    function selectNext(currentIndex) {
        var delegate;
        var newIndex = 0;
        if (currentIndex === -1 && model.count > 0) {
            while (model.count > newIndex) {
                delegate = model.get(newIndex);
                if (!!delegate["enabled"]) {
                    select(newIndex);
                    break;
                }
                newIndex++;
            }
        } else if (currentIndex !== -1 && model.count > 1) {
            var startIndex = (currentIndex + 1) % model.count;
            newIndex = startIndex;
            do {
                delegate = model.get(newIndex);
                if (!!delegate["enabled"]) {
                    select(newIndex);
                    break;
                }
                newIndex = (newIndex + 1) % model.count;
            } while (newIndex !== startIndex)
        }
    }

    function selectPrevious(currentIndex) {
        var delegate;
        var newIndex = model.count-1;
        if (currentIndex === -1 && model.count > 0) {
            while (model.count > newIndex) {
                delegate = model.get(newIndex);
                if (!!delegate["enabled"]) {
                    select(newIndex);
                    break;
                }
                newIndex--;
            }
        } else if (currentIndex !== -1 && model.count > 1) {
            var startIndex = currentIndex - 1;
            newIndex = startIndex;
            do {
                if (newIndex < 0) {
                    newIndex = model.count - 1;
                }
                delegate = model.get(newIndex);
                if (!!delegate["enabled"]) {
                    select(newIndex);
                    break;
                }
                newIndex--;
            } while (newIndex !== startIndex)
        }
    }
}

