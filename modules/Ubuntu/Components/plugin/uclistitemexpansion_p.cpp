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
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickflickable_p_p.h>
#include <QtQuick/private/qquickanchors_p.h>


UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , listItem(0)
    , content(0)
    , contentItem(0)
    , heightChange(0)
    , heightAnimation(0)
    , expanded(false)
    , flags(UCListItem::CollapseOnExternalClick | UCListItem::GrabNextInView | UCListItem::ExclusiveExpand)
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

void UCListItemExpansion::completeCollapse()
{
    expanded = false;
    if (contentItem) {
        contentItem->setVisible(false);
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
    }
    if (expanded) {
        this->expanded = true;
        updateExpanded();
    } else {
        PropertyChange::restore(heightChange);
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
    QQuickItem *expansionItem = NULL;
    if (content) {
        // create content in the ListItem's context
        expansionItem = qobject_cast<QQuickItem*>(content->create(qmlContext(listItem)));
    }
    if (expansionItem) {
        expansionItem->setParent(listItem);
        // create a flickable and reparent the content into that
        QQuickFlickable *flickable = new QQuickFlickable(listItem);
        flickable->setParentItem(listItem);
        expansionItem->setParentItem(flickable->contentItem());
        // set the default height the same as the content's height
        flickable->setHeight(expansionItem->height());
        // make interactive false by default
        flickable->setInteractive(false);
        // anchor left and right to the parent's edges
        QQuickAnchors *anchors = QQuickFlickablePrivate::get(flickable)->anchors();
        anchors->setLeft(UCListItemPrivate::get(listItem)->left());
        anchors->setRight(UCListItemPrivate::get(listItem)->right());
        contentItem = flickable;
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

    qreal expansionHeight = listItem->height();
    if (!height && createContent()) {
        // content height drives the expansion height
        if (hasFlag(UCListItem::ExpandContentItem)) {
            // update contentItem's top anchor
            QQuickAnchors *anchors = QQuickFlickablePrivate::get(contentItem)->anchors();
            anchors->setTop(UCListItemPrivate::get(listItem)->top());
            expansionHeight = contentItem->height();
        } else {
            // expand under item->contentItem, so anchor its top to item->contentItem
            QQuickAnchors *anchors = QQuickFlickablePrivate::get(contentItem)->anchors();
            anchors->setTop(QQuickItemPrivate::get(listItem->contentItem())->bottom());
            expansionHeight = contentItem->height() + listItem->height();
        }
    } else if (height && height > listItem->height()) {

    }

    // apply new height
    PropertyChange::setValue(heightChange, expansionHeight);
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
    this->content = content;
    if (UCListItemPrivate::get(listItem)->ready) {
        updateExpanded();
    }
    Q_EMIT contentChanged();
}
