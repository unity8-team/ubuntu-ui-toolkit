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
        PropertyChange::setValue(heightChange, item->height() + height);
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
        heightAnimation->setDuration(animationCodes.SleepyDuration());
        heightAnimation->setTargetObject(item);
        heightAnimation->setProperty("height");
        heightAnimation->setAlwaysRunToEnd(false);
    }
    return (heightAnimation != NULL);
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
