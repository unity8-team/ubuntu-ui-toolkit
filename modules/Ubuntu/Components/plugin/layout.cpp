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

#include "layout.h"
#include "layout_p.h"
#include "layoutmanager.h"
#include <QDebug>

Layout::Layout(QObject *parent)
    : QObject(parent)
    , d(new LayoutPrivate(this))
{
}

Layout::~Layout()
{
}

/*!
    \qmlproperty string Layout::name
    This property holds the name of the layout.

    Each layout should have a unique name within its item.
*/
QString Layout::name() const
{
    return d->name;
}

void Layout::setName(const QString &n)
{
    d->name = n;
    d->named = true;
}

bool Layout::isNamed() const
{
    return d->named;
}

bool Layout::isWhenKnown() const
{
    return d->when != 0;
}

/*!
    \qmlproperty bool Layout::when
    This property holds when the layout should be used.

    This should be set to an expression that evaluates to \c true when you want the layout to
    be applied. TODO: EXAMPLE

    If multiple layouts in a group have \c when clauses that evaluate to \c true
    at the same time, the first matching layout will be applied.
*/
QQmlBinding *Layout::when() const
{
    return d->when;
}

void Layout::setWhen(QQmlBinding *when)
{
    d->when = when;
    if (d->layoutManager) d->layoutManager->updateAutoState();
}

QQmlListProperty<QQuickItem> Layout::items()
{
    return QQmlListProperty<QQuickItem>(this, &m_items, &LayoutPrivate::append_item,
                                                        &LayoutPrivate::count_items,
                                                        &LayoutPrivate::at_item,
                                                        &LayoutPrivate::clear_items);
}

LayoutManager* Layout::layoutManager() const
{
    return d->layoutManager;
}

void Layout::setLayoutManager(LayoutManager *layoutManager)
{
    d->layoutManager = layoutManager;
}













QString LayoutAttached::item() const
{
    return m_item;
}

void LayoutAttached::setItem(const QString &item)
{
    if (item != m_item) {
        m_item = item;
        Q_EMIT itemChanged();
    }
}

//QQmlListProperty<QString> LayoutAttached::items()
//{
//    return QQmlListProperty<QString>(this, &m_items, &LayoutAttachedPrivate::append_child,
//                                                        &LayoutAttachedPrivate::count_children,
//                                                        &LayoutAttachedPrivate::at_child,
//                                                        &LayoutAttachedPrivate::clear_children);
//}


LayoutAttached::LayoutAttached(QObject *parent)
    : QObject(parent)
{}

LayoutAttached *Layout::qmlAttachedProperties(QObject *object)
{
    return new LayoutAttached(object);
}
