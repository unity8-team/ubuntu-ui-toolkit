/*
 * Copyright 2015 Canonical Ltd.
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

#include "uclistitem.h"
#include "uclistitem_p.h"

UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , m_height(0)
    , m_item(0)
{
}

void UCListItemExpansion::init(UCListItem *item)
{
    setParent(item);
    m_item = item;
}

bool UCListItemExpansion::expanded()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_item);
    UCViewItemsAttachedPrivate *attached = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    if (attached) {
        return attached->expandedList.contains(listItem->index());
    }
    return false;
}
void UCListItemExpansion::setExpanded(bool expanded)
{
    if (expanded == this->expanded()) {
        return;
    }
    // load style
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_item);
    listItem->initStyleItem();

    UCViewItemsAttachedPrivate *attached = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    if (attached) {
        if (attached->expansionFlags & UCViewItemsAttached::Exclusive) {
            attached->collapseAll();
        }
        attached->expand(listItem->index(), m_item);
    }
    // no need to emit the signal as the attached property's expandedIndicesChanged will trigger it
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (m_height == height) {
        return;
    }
    m_height = height;
    Q_EMIT heightChanged();
}
