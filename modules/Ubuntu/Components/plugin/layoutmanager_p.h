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
#include "layout.h"

#define foreach Q_FOREACH //workaround to fix private v8 include
#include <QtQml/private/qqmlbinding_p.h>

class LayoutManagerPrivate : public QObject {
public:
    LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager)
        : QObject(parent)
        , currentLayout(QString())
        , q(layoutManager)
    {};

    QList<Layout *> layouts;
    QString currentLayout;
    LayoutManager *q;

    bool updateAutoState()
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

    // helper functions for the "layouts" QQmlListProperty
    static void append_layout(QQmlListProperty<Layout> *list, Layout *layout)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        if (layout) {
            // check if name unique!! If so, register it.
            layout->setLayoutManager(_this);
            _this->d->layouts.append(layout);
        }
    }

    static int count_layouts(QQmlListProperty<Layout> *list)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        return _this->d->layouts.count();
    }

    static Layout *at_layout(QQmlListProperty<Layout> *list, int index)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        return _this->d->layouts.at(index);
    }

    static void clear_layouts(QQmlListProperty<Layout> *list)
    {
        LayoutManager *_this = static_cast<LayoutManager *>(list->object);
        //FIXME: reset LayoutManager property when removing
        _this->d->layouts.clear();
    }
};

#endif // LAYOUTMANAGER_P_H
