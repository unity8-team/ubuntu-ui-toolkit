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
 *
 * Author: Gerry Boland <gerry.boland@canonical.com>
 */

#ifndef LAYOUTMANAGER_P_H
#define LAYOUTMANAGER_P_H

#include "layoutmanager.h"

class LayoutManagerPrivate : public QObject {
public:
    LayoutManagerPrivate()
    {};

    // helper functions for the "layouts" QQmlListProperty
    static void append_layout(QQmlListProperty<Layout> *list, Layout *layout)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        if (layout) {
            _this->m_layouts.append(layout);
        }
    }

    static int count_layouts(QQmlListProperty<Layout> *list)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        return _this->m_layouts.count();
    }

    static Layout *at_layout(QQmlListProperty<Layout> *list, int index)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        return _this->m_layouts.at(index);
    }

    static void clear_layouts(QQmlListProperty<Layout> *list)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        _this->m_layouts.clear();
    }
};

#endif // LAYOUTMANAGER_P_H
