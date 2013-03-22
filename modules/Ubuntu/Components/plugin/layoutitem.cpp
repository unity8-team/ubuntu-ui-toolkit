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

#include "layoutitem.h"

LayoutItem::LayoutItem(QObject *parent)
    : QObject(parent)
    , d(new QQuickItem)
    , m_itemName(QString())
{
    QObject::connect(d, &QQuickItem::xChanged, this, &LayoutItem::xChanged);
    QObject::connect(d, &QQuickItem::yChanged, this, &LayoutItem::yChanged);
    QObject::connect(d, &QQuickItem::zChanged, this, &LayoutItem::zChanged);
    QObject::connect(d, &QQuickItem::widthChanged, this, &LayoutItem::widthChanged);
    QObject::connect(d, &QQuickItem::heightChanged, this, &LayoutItem::heightChanged);
    QObject::connect(d, &QQuickItem::baselineOffsetChanged, this, &LayoutItem::baselineOffsetChanged);
    QObject::connect(d, &QQuickItem::xChanged, this, &LayoutItem::xChanged);
}

QString LayoutItem::itemName() const
{
    return m_itemName;
}

void LayoutItem::setItemName(const QString &name)
{
    if (name != m_itemName) {
        m_itemName = name;
        Q_EMIT itemNameChanged();
    }
}

QQuickItem *LayoutItem::parentItem() const
{
    return d->parentItem();
}

void LayoutItem::setParentItem(QQuickItem *parent)
{
    d->setParentItem(parent);
}

qreal LayoutItem::x() const
{
    return d->x();
}

qreal LayoutItem::y() const
{
    return d->y();
}

qreal LayoutItem::z() const
{
    return d->z();
}

qreal LayoutItem::width() const
{
    return d->width();
}

qreal LayoutItem::height() const
{
    return d->height();
}

void LayoutItem::setX(qreal v)
{
    d->setX(v);
}

void LayoutItem::setY(qreal v)
{
    d->setY(v);
}

void LayoutItem::setZ(qreal v)
{
    d->setZ(v);
}

void LayoutItem::setWidth(qreal w)
{
    d->setWidth(w);
}

void LayoutItem::setHeight(qreal h)
{
    d->setHeight(h);
}

qreal LayoutItem::baselineOffset() const
{
    return d->baselineOffset();
}

void LayoutItem::resetWidth()
{
    d->resetWidth();
}

void LayoutItem::resetHeight()
{
    d->resetHeight();
}

void LayoutItem::setBaselineOffset(qreal o)
{
    d->setBaselineOffset(o);
}

QQuickAnchors* LayoutItem::anchors() const
{
    return d->property("anchors").value<QQuickAnchors*>();
}

QQuickAnchorLine LayoutItem::left() const
{
    return d->property("left").value<QQuickAnchorLine>();
}

QQuickAnchorLine LayoutItem::right() const
{
    return d->property("right").value<QQuickAnchorLine>();
}

QQuickAnchorLine LayoutItem::top() const
{
    return d->property("top").value<QQuickAnchorLine>();
}

QQuickAnchorLine LayoutItem::bottom() const
{
    return d->property("bottom").value<QQuickAnchorLine>();
}

QQuickAnchorLine LayoutItem::horizontalCenter() const
{
    return d->property("horizontalCenter").value<QQuickAnchorLine>();;
}

QQuickAnchorLine LayoutItem::verticalCenter() const
{
    return d->property("verticalCenter").value<QQuickAnchorLine>();
}

QQuickAnchorLine LayoutItem::baseline() const
{
    return d->property("baseline").value<QQuickAnchorLine>();
}

