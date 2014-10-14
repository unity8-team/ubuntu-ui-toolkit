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

UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , item(0)
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
    this->item = item;
}

qreal UCListItemExpansion::collapsedHeight()
{
    return PropertyChange::originalValue(heightChange).toReal();
}

bool UCListItemExpansion::hasFlag(UCListItem::ExpansionFlag flag)
{
    return (flags & flag) == flag;
}

void UCListItemExpansion::completeCollapse()
{
    expanded = false;
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
        heightChange = new PropertyChange(item, "height", this);
        connect(heightChange, SIGNAL(restoreCompleted()), this, SLOT(completeCollapse()));
    }
    createAnimation();
    PropertyChange::setAnimation(heightChange, heightAnimation);

    if (expanded) {
        this->expanded = true;
        qreal expandedHeight = 0.0;
        if (!height && createContent()) {
            // height is not set, but content is, get height from content
            expandedHeight = contentItem->height();
            if (!hasFlag(UCListItem::ExpandContentItem)) {
                // policy sais we should add the ListItem's height
                expandedHeight += item->height();
            }
        } else if (height) {
            // TODO: the height depends on the content's height as well
            expandedHeight = height;
        }
        PropertyChange::setValue(heightChange, expandedHeight);
    } else {
        PropertyChange::restore(heightChange);
    }
    Q_EMIT expandedChanged();
}

bool UCListItemExpansion::createAnimation()
{
    if (!heightAnimation && UCListItemPrivate::get(item)->ready) {
        UCUbuntuAnimation animationCodes;
        heightAnimation = new QQuickPropertyAnimation(this);
        heightAnimation->setEasing(animationCodes.StandardEasing());
        heightAnimation->setDuration(animationCodes.SnapDuration());
        heightAnimation->setTargetObject(item);
        heightAnimation->setProperty("height");
        heightAnimation->setAlwaysRunToEnd(false);
    }
    return (heightAnimation != NULL);
}

bool UCListItemExpansion::createContent()
{
    if (content) {

    }
    return (contentItem != NULL);
}

void UCListItemExpansion::setFlags(UCListItem::ExpansionFlags flags)
{
    if (this->flags == flags) {
        return;
    }
    this->flags = flags;
    Q_EMIT flagsChanged();
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (this->height == height) {
        return;
    }
    this->height = height;
    Q_EMIT heightChanged();
}

void UCListItemExpansion::setContent(QQmlComponent *content)
{
    if (this->content == content) {
        return;
    }
    this->content = content;
    Q_EMIT contentChanged();
}
