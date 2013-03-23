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

LayoutItem::LayoutItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_itemName(QString())
{
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

