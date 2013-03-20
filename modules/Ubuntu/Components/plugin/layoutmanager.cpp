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

#include "layoutmanager.h"
#include "layoutmanager_p.h"
#include <QDebug>

LayoutManagerAttached::LayoutManagerAttached(QObject *parent)
    : QObject(parent)
    , m_itemName(QString())
{
}

QString LayoutManagerAttached::itemName() const
{
    return m_itemName;
}

void LayoutManagerAttached::setItemName(const QString &name)
{
    if (name != m_itemName) {
        m_itemName = name;
        Q_EMIT itemNameChanged();
    }
}

LayoutManager::LayoutManager(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new LayoutManagerPrivate(parent, this))
{
    QObject::connect(this, &LayoutManager::childrenChanged, d, &LayoutManagerPrivate::getItemsToLayout);
}

QString LayoutManager::layout() const
{
    return d->currentLayout;
}

void LayoutManager::setLayout(const QString &layout)
{
    if (layout != d->currentLayout) {
        d->currentLayout = layout;
        d->performLayoutChange();
        Q_EMIT layoutChanged();
    }
}

QQmlListProperty<Layout> LayoutManager::layouts()
{
    return QQmlListProperty<Layout>(this, &(d->layouts), &LayoutManagerPrivate::append_layout,
                                                         &LayoutManagerPrivate::count_layouts,
                                                         &LayoutManagerPrivate::at_layout,
                                                         &LayoutManagerPrivate::clear_layouts);
}

bool LayoutManager::updateAutoLayout()
{
    return d->updateAutoLayout();
}

LayoutManagerAttached *LayoutManager::qmlAttachedProperties(QObject *parent)
{
    return new LayoutManagerAttached(parent);
}
