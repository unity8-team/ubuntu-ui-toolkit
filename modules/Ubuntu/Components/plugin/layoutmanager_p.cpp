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

#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach

LayoutManagerPrivate::LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager)
    : QObject(parent)
    , currentLayout(NULL)
    , oldLayout(NULL)
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
                    if (currentLayout != layout) {
                        q->setLayout(layout);
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

    // undo all changes previous state made

    // hide old layout
    if (oldLayout != NULL) {
        for (int i = 0; i < oldLayout->m_items.count(); i++) {
            oldLayout->m_items.at(i)->setProperty("visible", false);
            //currentLayout->m_items.at(i)->setParentItem(QQuickItem(0));
        }
    }

    // show new layout
    for (int i = 0; i < currentLayout->m_items.count(); i++) {
        currentLayout->m_items.at(i)->setParentItem(q);
        currentLayout->m_items.at(i)->setProperty("visible", true);
    }

    reparentItems();
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

        if (attached != 0 && attached->itemName() != "") {
            if (!items.contains(attached->itemName())) {
                items.insert(attached->itemName(), child);
            } else {
                qDebug() << "Duplicate itemName" << attached->itemName() << ", Item will not be managed by LayoutManager";
            }
        } else {
            // if no itemName set, hide the component
            if (QString(child->metaObject()->className()) != "Layout") {
                qDebug() << "Child of LayoutManager with no or empty itemName:"
                         << child->metaObject()->className() << "it will be hidden and not managed by LayoutManager";
                child->setProperty("visible", false);
            }
        }
    }
}

void LayoutManagerPrivate::reparentItems()
{
    // iterate through the Layout definition to find those Items with Layout.item set
    QList<QQuickItem *> layoutChildren = currentLayout->findChildren<QQuickItem *>();

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

void LayoutManagerPrivate::append_layout(QQmlListProperty<Layout> *list, Layout *layout)
{
    LayoutManager *_this = static_cast<LayoutManager *>(list->object);
    if (layout) {
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

