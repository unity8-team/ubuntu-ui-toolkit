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

#ifndef LAYOUT_P_H
#define LAYOUT_P_H

#include "layout.h"
#include "layoutmanager.h"
#include <QDebug>

class LayoutPrivate : public QObject {
    Q_DECLARE_PUBLIC(Layout)
public:
    LayoutPrivate(Layout *qq)
        : when(0), layoutManager(0), named(false), q_ptr(qq) {};

    QString name;
    QQmlBinding *when;
    LayoutManager *layoutManager;
    bool named;

    Layout *q_ptr;

    // helper functions for the "items" QQmlListProperty
    static void append_item(QQmlListProperty<QQuickItem> *items, QQuickItem *item)
    {qDebug() << "append_item";
        Layout *_this = static_cast<Layout *>(items->object);
        if (item) {
            _this->m_items.append(item);
        }
    }

    static int count_items(QQmlListProperty<QQuickItem> *items)
    {
        Layout *_this = static_cast<Layout *>(items->object);
        return _this->m_items.count();
    }

    static QQuickItem *at_item(QQmlListProperty<QQuickItem> *items, int index)
    {
        Layout *_this = static_cast<Layout *>(items->object);
        return _this->m_items.at(index);
    }

    static void clear_items(QQmlListProperty<QQuickItem> *items)
    {
        Layout *_this = static_cast<Layout *>(items->object);
        _this->m_items.clear();
    }
};

class LayoutAttachedPrivate : public QObject {
public:
    LayoutAttachedPrivate()
    {};

//    // helper functions for the "children" QQmlListProperty
//    static void append_child(QQmlListProperty<QString> *children, QString *child)
//    {
//        LayoutAttached *_this = static_cast<LayoutAttached *>(children->object);
//        if (child) {
//            _this->m_items.append(child);
//        }
//    }

//    static int count_children(QQmlListProperty<QString> *children)
//    {
//        LayoutAttached *_this = static_cast<LayoutAttached *>(children->object);
//        return _this->m_items.count();
//    }

//    static QString *at_child(QQmlListProperty<QString> *children, int index)
//    {
//        LayoutAttached *_this = static_cast<LayoutAttached *>(children->object);
//        return _this->m_items.at(index);
//    }

//    static void clear_children(QQmlListProperty<QString> *children)
//    {
//        LayoutAttached *_this = static_cast<LayoutAttached *>(children->object);
//        _this->m_items.clear();
//    }
};

#endif // LAYOUT_P_H
