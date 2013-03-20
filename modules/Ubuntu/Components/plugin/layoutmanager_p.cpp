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

#include "layoutmanager_p.h"
#include "layoutmanager.h"
#include "layout.h"

#define foreach Q_FOREACH //workaround to fix private v8 include
#include <QtQml/private/qqmlbinding_p.h>
#undef foreach

LayoutManagerPrivate::LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager)
    : QObject(parent)
    , currentLayout(QString())
    , q(layoutManager)
{
}

bool LayoutManagerPrivate::updateAutoLayout()
{
    for (int i = 0; i < layouts.count(); i++) {
        Layout *layout = layouts.at(i);
        if (layout->isWhenKnown()) {
            if (layout->isNamed()) {
                if (layout->when() && layout->when()->evaluate().toBool()) {
                    //qDebug() << "Setting auto state due to:" << layout->when()->expression();
                    if (currentLayout != layout->name()) {
                        q->setLayout(layout->name());
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

void LayoutManagerPrivate::performLayoutChange()
{
    QHash<QString, QQuickItem*> itemParents;
    Layout *currentLayoutItem = NULL;

    // find the Layout with the correct name
    for (int i = 0; i < layouts.count(); i++) {
        if (layouts.at(i)->name() == currentLayout) {
            currentLayoutItem = layouts.at(i);
            break;
        }
    }

    if (currentLayoutItem == NULL) {
        qDebug() << "unable to find layout" << currentLayout;
        return;
    }

    // reparent children of the layout to the LayoutManager
    for (int i = 0; i < currentLayoutItem->m_items.count(); i++) {
        currentLayoutItem->m_items.at(i)->setParentItem(q);
    }

    // iterate through the Layout definition to find those Items with Layout.item set
    QList<QQuickItem *> layoutChildren = currentLayoutItem->findChildren<QQuickItem *>();

    for (int i = 0; i < layoutChildren.count(); i++) {
        QQuickItem* child = static_cast<QQuickItem*>(layoutChildren.at(i));

        // has child the Layout.item attached property set?
        LayoutAttached *attached =
                qobject_cast<LayoutAttached*>(qmlAttachedPropertiesObject<Layout>(child, false));

        if (attached != 0 && attached->item() != "") {
            if (items.contains(attached->item())) {
                items.value(attached->item())->setParentItem(child);
            }
        }
    }
}

void LayoutManagerPrivate::getItemsToLayout()
{
    items.clear();

    const QObjectList* children = &(q->children());
    for (int i = 0; i < children->count(); i++) {
        QQuickItem* child = static_cast<QQuickItem*>(children->at(i));

        // has child the LayoutManager.itemName attached property set?
        LayoutManagerAttached *attached =
                qobject_cast<LayoutManagerAttached*>(qmlAttachedPropertiesObject<LayoutManager>(child, false));

        if (attached != 0 && attached->itemName() != "" && !items.contains(attached->itemName())) {
            items.insert(attached->itemName(), child);
        } else {
            child->setProperty("visible", false);
        }
    }
}

void LayoutManagerPrivate::append_layout(QQmlListProperty<Layout> *list, Layout *layout)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    if (layout) {
        // check if name unique!! If so, register it.
//            if (_this->d->items.contains(layout->name())) {
//                qDebug() << "Duplicate itemName" << layout->name() << ", Item will not be managed by LayoutManager";
//                return;
//            }
//            _this->d->items.insert(layout->name(), layout)
        layout->setLayoutManager(_this);
        _this->d->layouts.append(layout);
    }
}

int LayoutManagerPrivate::count_layouts(QQmlListProperty<Layout> *list)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    return _this->d->layouts.count();
}

Layout *LayoutManagerPrivate::at_layout(QQmlListProperty<Layout> *list, int index)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    return _this->d->layouts.at(index);
}

void LayoutManagerPrivate::clear_layouts(QQmlListProperty<Layout> *list)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    //FIXME: reset LayoutManager property when removing
    _this->d->layouts.clear();
}

