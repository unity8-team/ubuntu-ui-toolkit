/*
 * Copyright 2014 Canonical Ltd.
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
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include <QtQuick/private/qquickanimation_p.h>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickitemview_p.h>

UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , listItem(0)
    , content(0)
    , contentItem(0)
    , heightChange(0)
    , heightAnimation(0)
    , expanded(false)
    , flags(UCListItem::CollapseOnExternalClick | UCListItem::GrabNextInView | UCListItem::DimmOthersOnExpand)
    , height(0.0)
{
}

UCListItemExpansion::~UCListItemExpansion()
{
}

void UCListItemExpansion::init(UCListItem *item)
{
    this->listItem = item;
}

qreal UCListItemExpansion::collapsedHeight()
{
    return heightChange ? PropertyChange::originalValue(heightChange).toReal() : listItem->height();
}

bool UCListItemExpansion::hasFlag(UCListItem::ExpansionFlag flag)
{
    return (flags & flag) == flag;
}

void UCListItemExpansion::completeExpand()
{
    UCListItemAttachedPrivate *groupAttached = UCListItemPrivate::get(listItem)->parentAttached();
    if (groupAttached) {
        groupAttached->expansionCompleted();
    }
}

void UCListItemExpansion::completeCollapse()
{
    expanded = false;
    if (contentItem) {
        // do not consume memory while collapsed
        delete contentItem;
        contentItem = 0;
    }
}

bool UCListItemExpansion::isExpanded() const
{
    return expanded;
}

void UCListItemExpansion::setExpanded(bool expanded)
{
    if (this->expanded == expanded) {
        return;
    }
    // make sure we have the change and animations set
    if (!heightChange) {
        heightChange = new PropertyChange(listItem, "height", this);
        connect(heightChange, SIGNAL(restoreCompleted()), this, SLOT(completeCollapse()));
        connect(heightChange, SIGNAL(setCompleted()), this, SLOT(completeExpand()));
    }
    UCListItemAttachedPrivate *groupAttached = UCListItemPrivate::get(listItem)->parentAttached();
    if (expanded) {
        this->expanded = true;
        // collapse other expanded before we start expansion
        if (groupAttached) {
            groupAttached->setExpandedItem(listItem);
        }
        updateExpanded();
    } else {
        PropertyChange::restore(heightChange);
        if (groupAttached) {
            groupAttached->collapseExpanded();
        }
    }
    Q_EMIT expandedChanged();
}

bool UCListItemExpansion::createAnimation()
{
    if (!heightAnimation && UCListItemPrivate::get(listItem)->ready) {
        UCUbuntuAnimation animationCodes;
        heightAnimation = new QQuickPropertyAnimation(this);
        heightAnimation->setEasing(animationCodes.StandardEasing());
        heightAnimation->setDuration(animationCodes.SnapDuration());
        heightAnimation->setTargetObject(listItem);
        heightAnimation->setProperty("height");
        heightAnimation->setAlwaysRunToEnd(false);
    }
    return (heightAnimation != NULL);
}

bool UCListItemExpansion::createContent()
{
    if (content) {
        QQuickItem *item = qobject_cast<QQuickItem*>(content->beginCreate(qmlContext(listItem)));
        if (item) {
            QQml_setParent_noEvent(item, listItem);
            UCListItemAttached *attached = static_cast<UCListItemAttached*>(
                        qmlAttachedPropertiesObject<UCListItem>(item));
            if (attached) {
                attached->setItem(listItem);
            }
            item->setParentItem(listItem);
            QQuickAnchors *anchors = QQuickItemPrivate::get(item)->anchors();
            anchors->setLeft(UCListItemPrivate::get(listItem)->left());
            anchors->setRight(UCListItemPrivate::get(listItem)->right());
            item->setClip(true);
            content->completeCreate();
        }
        contentItem = item;
    }
    return (contentItem != NULL);
}

// update expansion in case flags, content or heigth changes
void UCListItemExpansion::updateExpanded()
{
    if (!expanded) {
        return;
    }

    // make sure we have up to date animation object
    createAnimation();
    PropertyChange::setAnimation(heightChange, heightAnimation);

    effectiveExpansionHeight = listItem->height();
    if (!height && createContent()) {
        // content height drives the expansion height
        if (hasFlag(UCListItem::ExpandContentItem)) {
            // update contentItem's top anchor
            QQuickAnchors *anchors = QQuickItemPrivate::get(contentItem)->anchors();
            anchors->setTop(UCListItemPrivate::get(listItem)->top());
            effectiveExpansionHeight = contentItem->height();
        } else {
            // expand under item->contentItem, so anchor its top to item->contentItem
            QQuickAnchors *anchors = QQuickItemPrivate::get(contentItem)->anchors();
            anchors->setTop(QQuickItemPrivate::get(listItem->contentItem())->bottom());
            effectiveExpansionHeight = contentItem->height() + listItem->height();
        }
    } else if (height && height > listItem->height()) {
        if (createContent()) {
            if (hasFlag(UCListItem::ExpandContentItem)) {
                // update contentItem's top anchor
                QQuickAnchors *anchors = QQuickItemPrivate::get(contentItem)->anchors();
                anchors->setTop(UCListItemPrivate::get(listItem)->top());
                effectiveExpansionHeight = contentItem->height();
            } else {
                // expand under item->contentItem, so anchor its top to item->contentItem
                QQuickAnchors *anchors = QQuickItemPrivate::get(contentItem)->anchors();
                anchors->setTop(QQuickItemPrivate::get(listItem->contentItem())->bottom());
                effectiveExpansionHeight = contentItem->height() + listItem->height();
            }
            effectiveExpansionHeight = CLAMP(effectiveExpansionHeight, listItem->height(), height);
            if (hasFlag(UCListItem::ExpandContentItem)) {
                contentItem->setHeight(effectiveExpansionHeight);
            } else {
                contentItem->setHeight(effectiveExpansionHeight - listItem->height());
            }
        } else {
            effectiveExpansionHeight = height;
        }
    }

    // apply new height
    PropertyChange::setValue(heightChange, effectiveExpansionHeight);
    // make contentItem visible
    if (contentItem) {
        contentItem->setVisible(true);
    }
}

void UCListItemExpansion::setFlags(UCListItem::ExpansionFlags flags)
{
    if (this->flags == flags) {
        return;
    }
    this->flags = flags;
    if (UCListItemPrivate::get(listItem)->ready) {
        updateExpanded();
    }
    Q_EMIT flagsChanged();
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (this->height == height) {
        return;
    }
    this->height = height;
    if (UCListItemPrivate::get(listItem)->ready) {
        updateExpanded();
    }
    Q_EMIT heightChanged();
}

void UCListItemExpansion::setContent(QQmlComponent *content)
{
    if (this->content == content) {
        return;
    }
    delete contentItem;
    this->content = content;
    if (UCListItemPrivate::get(listItem)->ready) {
        updateExpanded();
    }
    Q_EMIT contentChanged();
}

/*-----------------------------------------------------------------------------
 * ListItem attached properties
 */

UCListItemAttachedPrivate::UCListItemAttachedPrivate(UCListItemAttached *qq)
    : q_ptr(qq)
    , m_attacheeItem(0)
    , m_expandedIndex(-1)
{
}

/*!
 * \qmlattachedproperty int ListItem::expandedIndex
 * The property specifies the index of the expanded ListItem in a view (ListView
 * or Flickable). The proeprty is attached to ListView or positioner (Column, Row)
 * and to the expanded content specified in \l expansion.content.
 */
int UCListItemAttachedPrivate::expandedIndex() const
{
    return m_expandedIndex;
}

/*!
 * \qmlattachedproperty ListItem ListItem::item
 * The proeprty holds the instance of the ListItem. The proeprty is valid when
 * attached to the expanded content created from \l expansion.content.
 */
UCListItem *UCListItemAttachedPrivate::item() const
{
    return m_attacheeItem;
}

void UCListItemAttachedPrivate::setExpandedItem(UCListItem *listItem)
{
    Q_Q(UCListItemAttached);
    // collapse the already expanded one
    UCListItemPrivate *item = UCListItemPrivate::get(listItem);
    collapseExpanded(false);
    m_expandedItem = listItem;
    if (m_expandedIndex != item->index) {
        m_expandedIndex = item->index;
        Q_EMIT q->expandedIndexChanged(m_expandedIndex);
        // position the view!
        if (item->flickable) {
            // check if positionViewAtIndex is available, if yes, invoke it
            const QMetaObject *mo = item->flickable->metaObject();
            int slotIndex = mo->indexOfSlot("positionViewAtIndex(int,int)");
            if (slotIndex >= 0) {
                // first set the currentIndex
                item->flickable->setProperty("currentIndex", item->index);
                mo->invokeMethod(item->flickable, "positionViewAtIndex", Q_ARG(int, item->index), Q_ARG(int, QQuickItemView::End));
            }
            // connect expansion progress to adjust content; we can track expansion by connecting to the item's height change
            QObject::connect(listItem, SIGNAL(heightChanged()), q, SLOT(_q_adjustFlickableContentY()));
        }
    }
}

// called when expansion animation completes
void UCListItemAttachedPrivate::expansionCompleted()
{
    // disconnect expansion progress
    Q_Q(UCListItemAttached);
    QObject::disconnect(m_expandedItem.data(), SIGNAL(heightChanged()), q, SLOT(_q_adjustFlickableContentY()));
}

void UCListItemAttachedPrivate::collapseExpanded(bool signalChange)
{
    if (m_expandedIndex >= 0 && m_expandedItem && m_expandedItem->expansion()->isExpanded()) {
        // collapse may result in loop, so set the expansion index first
        m_expandedIndex = -1;
        m_expandedItem->expansion()->setExpanded(false);
        m_expandedItem.clear();
        if (signalChange) {
            Q_Q(UCListItemAttached);
            Q_EMIT q->expandedIndexChanged(m_expandedIndex);
        }
    }
}

// slot connected when the ListItemAttached is attached to the expansion.content
void UCListItemAttachedPrivate::_q_updateExpandedIndex(int index)
{
    m_expandedIndex = index;
    Q_EMIT q_ptr->expandedIndexChanged(index);
}

// slots adjusting contentY of ListView or Flickable
void UCListItemAttachedPrivate::_q_adjustFlickableContentY()
{
    // check if we have to connect to the height animation
    UCListItemPrivate *item = UCListItemPrivate::get(m_expandedItem.data());
//    qreal expansionHeight = MIN(item->expansion->effectiveHeight(), item->flickable->height() - item->expansion->collapsedHeight());
    qreal contentY = item->flickable->contentY();
    qreal originY = item->flickable->originY();

    qreal to = contentY + m_expandedItem->height() + originY;
    qreal dy = to - item->flickable->height();
    qDebug() << "DY" << dy;
    if (dy > 0) {
        item->flickable->setContentY(contentY - dy - originY);
    }
}

UCListItemAttached::UCListItemAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCListItemAttachedPrivate(this))
{
}

void UCListItemAttached::setItem(UCListItem *item)
{
    Q_D(UCListItemAttached);
    d->m_attacheeItem = item;
    // when attached to expansion.content, the expandedIndex updates are coming from the one
    // attached to the ListItem's owning flickable or positioner, so we must connect to that one
    UCListItemAttachedPrivate *parentAttached = UCListItemPrivate::get(item)->parentAttached();
    if (parentAttached) {
        connect(parentAttached->q_ptr, SIGNAL(expandedIndexChanged(int)),
                this, SLOT(_q_updateExpandedIndex(int)));
    }
    Q_EMIT itemChanged();
}
