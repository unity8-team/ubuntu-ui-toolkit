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
#include <QtQuick/private/qquicktransition_p.h>
#include <QtQuick/private/qquickanimation_p.h>

UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , m_listItem(static_cast<UCListItem13*>(parent))
    , m_content(Q_NULLPTR)
    , m_expandCollapse(Q_NULLPTR)
    , m_height(0.0)
    , m_hideCollapsedContent(false)
    , m_filtering(false)
    , m_customExpandCollapse(true) // set so reset can proceed
{
    resetExpandCollapse();
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    if (listItem->styleItem) {
        // update the transitions
        updateTransitions(static_cast<UCListItemStyle*>(listItem->styleItem));
    }
}

bool UCListItemExpansion::expandedWithFlag(UCViewItemsAttached::ExpansionFlag flag)
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    return expanded() && ((viewItems->expansionFlags & flag) == flag);
}

void UCListItemExpansion::enableClickFiltering(bool enable)
{
    if (m_filtering == enable) {
        return;
    }
    m_filtering = enable;
    if (m_filtering) {
        m_listItem->window()->installEventFilter(this);
    } else {
        m_listItem->window()->removeEventFilter(this);
    }
}

// event filter for external mouse presses to collapse when pressed outside
bool UCListItemExpansion::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            QPointF myPos(window->contentItem()->mapToItem(m_listItem, mouse->localPos()));
            if (!m_listItem->contains(myPos)) {
                UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
                UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
                if (viewItems) {
                    // collapse all as there can be only one expanded when the flag is set
                    viewItems->collapseAll();
                }
            }
        }
    }
    return false;
}


bool UCListItemExpansion::expanded()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    return (viewItems && viewItems->expansionList.contains(listItem->index()));
}

void UCListItemExpansion::setExpanded(bool expanded)
{
    if (this->expanded() == expanded) {
        return;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    if (viewItems) {
        if (viewItems->expansionFlags & UCViewItemsAttached::Exclusive) {
            // collapse all the expanded ones
            viewItems->collapseAll();
        }
        if (expanded) {
            viewItems->expand(listItem->index(), m_listItem);
        } else {
            viewItems->collapse(listItem->index());
        }
    }
    UCListItemPrivate::get(m_listItem)->loadStyleItem();
    // no need to emit changed signal, as ViewItems.expandedIndicesChanged is connected to the change signal
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (m_height == height) {
        return;
    }
    m_height = height;
    Q_EMIT heightChanged();
}

void UCListItemExpansion::setContent(QQmlComponent *component)
{
    if (component == m_content) {
        return;
    }
    m_content = component;
    Q_EMIT contentChanged();
}

void UCListItemExpansion::setHideCollapsedContent(bool hide)
{
    if (m_hideCollapsedContent == hide) {
        return;
    }
    m_hideCollapsedContent = hide;
    Q_EMIT hideCollapsedContentChanged();
}

void UCListItemExpansion::setExpandCollapse(QQuickTransition *transition)
{
    if (transition == m_expandCollapse) {
        return;
    }
    if (!m_customExpandCollapse) {
        delete m_expandCollapse;
        m_expandCollapse = Q_NULLPTR;
    }
    m_expandCollapse = transition;
    if (!m_expandCollapse) {
        resetExpandCollapse();
        UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
        if (listItem->styleItem) {
            // update the transitions
            updateTransitions(static_cast<UCListItemStyle*>(listItem->styleItem));
        }
    }
    m_customExpandCollapse = (transition != Q_NULLPTR);
}
void UCListItemExpansion::resetExpandCollapse()
{
    if (!m_customExpandCollapse) {
        return;
    }
    // re-create transition
    m_expandCollapse = new QQuickTransition(m_listItem);
    m_expandCollapse->setFromState("");
    m_expandCollapse->setToState("expanded");
    m_expandCollapse->setReversible(true);
    m_customExpandCollapse = false;
}

void UCListItemExpansion::updateTransitions(UCListItemStyle *style)
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    if (style->m_flickableAnimation && !listItem->flickable.isNull()) {
        QQmlListProperty<QQuickAbstractAnimation> animations = m_expandCollapse->animations();
        animations.append(&animations, style->m_flickableAnimation);
    }
}

