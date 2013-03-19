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

#include "layoutmanager_p.h"
#include "layoutmanager.h"
#include "layout.h"

#define foreach Q_FOREACH //workaround to fix private v8 include
#include <QtQml/private/qqmlbinding_p.h>
#undef foreach

LayoutManagerPrivate::LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager)
    : QObject(parent)
    , currentLayout(QString())
    , q(layoutManager)
{};

bool LayoutManagerPrivate::updateAutoLayout()
{
    for (int ii = 0; ii < layouts.count(); ++ii) {
        Layout *layout = layouts.at(ii);
        if (layout->isWhenKnown()) {
            if (layout->isNamed()) {
                if (layout->when() && layout->when()->evaluate().toBool()) {
                    //qDebug() << "Setting auto state due to:" << layout->when()->expression();
                    if (currentLayout != layout->name()) {
                        q->setLayout(layout->name());
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

void LayoutManagerPrivate::append_layout(QQmlListProperty<Layout> *list, Layout *layout)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    if (layout) {
        // check if name unique!! If so, register it.
//            if (_this->d->items.contains(layout->name())) {
//                qDebug() << "Duplicate itemName" << layout->name() << ", Item will not be managed by LayoutManager";
//                return;
//            }
//            _this->d->items.insert(layout->name(), layout)
        layout->setLayoutManager(_this);
        _this->d->layouts.append(layout);
    }
}

int LayoutManagerPrivate::count_layouts(QQmlListProperty<Layout> *list)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    return _this->d->layouts.count();
}

Layout *LayoutManagerPrivate::at_layout(QQmlListProperty<Layout> *list, int index)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    return _this->d->layouts.at(index);
}

void LayoutManagerPrivate::clear_layouts(QQmlListProperty<Layout> *list)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    //FIXME: reset LayoutManager property when removing
    _this->d->layouts.clear();
}

