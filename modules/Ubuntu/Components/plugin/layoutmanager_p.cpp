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

class Action
{
public:
    Action(QQuickItem *, const QString &, const QVariant &);
    Action(QQuickItem *, const QString &, bool);
    Action(QQuickItem *, const QString &, QQuickItem *);
    void execute();
    void reverse();

    QQmlProperty fromProperty, toProperty;
    QVariant fromValue, toValue;

    QQmlAbstractBinding *fromBinding;
    QQmlAbstractBinding *toBinding;
};

Action::Action(QQuickItem *target, const QString &propertyName, const QVariant &value)
    : toProperty(target, propertyName, QQmlEngine::contextForObject(target))
    , toValue(value)
    , fromBinding(NULL)
{
    if (toProperty.isValid()) {
        fromBinding = QQmlPropertyPrivate::binding(toProperty);
        if (!fromBinding) {
            fromValue = toProperty.read();
        }
    }
}

Action::Action(QQuickItem *target, const QString &propertyName, bool value)
    : toProperty(target, propertyName, QQmlEngine::contextForObject(target))
    , toValue(value)
    , fromBinding(NULL)
{
    if (toProperty.isValid()) {
        fromBinding = QQmlPropertyPrivate::binding(toProperty);
        if (!fromBinding) {
            fromValue = toProperty.read();
        }
    }
}

Action::Action(QQuickItem *target, const QString &propertyName, QQuickItem *source)
    : fromProperty(source, propertyName, QQmlEngine::contextForObject(source))
    , toProperty(target, propertyName, QQmlEngine::contextForObject(target))
    , toValue(fromProperty.read())
    , fromBinding(NULL)
    , toBinding(NULL)
{
    if (toProperty.isValid()) {
        fromBinding = QQmlPropertyPrivate::binding(toProperty);
        if (!fromBinding) {
            fromValue = toProperty.read();
        }
    }
}

void Action::execute()
{
    if (fromProperty.isValid()) {
        toBinding = QQmlPropertyPrivate::binding(fromProperty);
        if (toBinding) {
            QQmlPropertyPrivate::setBinding(toProperty, toBinding, QQmlPropertyPrivate::DontRemoveBinding);
            return;
        }
    }
    toProperty.write(toValue);
}

void Action::reverse()
{
    if (fromBinding) {
        QQmlPropertyPrivate::setBinding(toProperty, fromBinding, QQmlPropertyPrivate::DontRemoveBinding);
    } else {
        toProperty.write(fromValue);
    }
}



LayoutManagerPrivate::LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager)
    : QObject(parent)
    , currentLayout(NULL)
    , oldLayout(NULL)
    , q(layoutManager)
    , ready(false)
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
    if (!ready) return;

    // undo all changes previous property changes
    applyActionsList(actions, true);

    actions.clear();

    // show new layout
    for (int i = 0; i < currentLayout->m_items.count(); i++) {
        currentLayout->m_items.at(i)->setParentItem(q);
        //actions << Action(currentLayout->m_items.at(i), "parent", q);
        actions << Action(currentLayout->m_items.at(i), "visible", true);
        actions << Action(currentLayout->m_items.at(i), "enabled", true);
    }

    reparentItems();
    applyActionsList(actions);
}

void LayoutManagerPrivate::getItemsToLayout()
{
    if (!ready) return;

    items.clear();

    const QObjectList* children = &(q->children());
    for (int i = 0; i < children->count(); i++) {
        QQuickItem* child = static_cast<QQuickItem*>(children->at(i));

        // has child the LayoutManager.itemName attached property set?
        LayoutManagerAttached *attached =
                qobject_cast<LayoutManagerAttached*>(qmlAttachedPropertiesObject<LayoutManager>(child, false));

        if (attached != NULL && attached->itemName() != "") {
            if (!items.contains(attached->itemName())) {
                items.insert(attached->itemName(), child);
            } else {
                qDebug() << "Duplicate itemName" << attached->itemName() << ", Item will not be managed by LayoutManager";
            }
        } else {
            // if no itemName set, hide the component
            if (QString(child->metaObject()->className()) != "Layout") {
//                qDebug() << "Child of LayoutManager with no or empty itemName:"
//                         << child->metaObject()->className() << "it will be hidden and not managed by LayoutManager";
                child->setProperty("visible", false);
                child->setProperty("enabled", false);
            }
        }
    }
}

void LayoutManagerPrivate::reparentItems()
{
    // create copy of items list, to keep track of which ones we change
    QHash <QString, QQuickItem*> unusedItems = items;

    // iterate through the Layout definition to find those Items with Layout.item set
    QList<QQuickItem *> layoutChildren = currentLayout->findChildren<QQuickItem *>();

    // list of properties we want applied to the Item
    QString includeList("x, y, z, width, height, anchors, top, bottom, left, right, baseline, "
                        "baselineOffset, verticalCenter, horizontalCenter, opacity, clip, "
                        "rotation, scale, transform, transformOrigin, transformOriginPoint");

    for (int i = 0; i < layoutChildren.count(); i++) {
        QQuickItem *child = static_cast<QQuickItem*>(layoutChildren.at(i));

        if (child->inherits("LayoutItem")) {
            const QString itemName = child->property("itemName").toString();

            if (unusedItems.contains(itemName)) {
                // set all properties of the Item to be those defined on the LayoutItem
                const QMetaObject *childMoc = child->metaObject();
                QQuickItem *itemToMove = unusedItems.value(itemName);

                for (int i=0; i<childMoc->propertyCount(); i++) {
                    QString propertyName = childMoc->property(i).name();

                    // special case if LayoutItem direct child of Layout, then parent is null
                    // so manually set this
                    if (propertyName == "parent"/*&& child->property("parent").isNull()*/) {
                        actions << Action(itemToMove,
                                          propertyName,
                                          q);

                    } else if (includeList.contains(propertyName)) {
                        actions << Action(itemToMove,
                                          propertyName,
                                          child);
                    }
                }

                unusedItems.remove(itemName);
            } else {
                qDebug() << "WARNING: LayoutItem with unrecognised itemName" << child->property("itemName").toString();
            }

        } else {
            // has child the Layout.item attached property set?
            LayoutAttached *attached =
                    qobject_cast<LayoutAttached*>(qmlAttachedPropertiesObject<Layout>(child, false));

            if (attached != 0 && attached->item() != "") {
                if (unusedItems.contains(attached->item())) {
                    actions << Action(unusedItems.value(attached->item()), "parent", child);
                    unusedItems.remove(attached->item());
                }
            }
        }
    }

    QHashIterator<QString, QQuickItem*> i(unusedItems);
    while (i.hasNext()) {
        i.next();
        actions << Action(i.value(), "visible", false);
        actions << Action(i.value(), "enabled", false);
    }
}

void LayoutManagerPrivate::applyActionsList(const ActionsList& list, bool reverse)
{
    Q_FOREACH (Action a, list) {
        if (!reverse) {
            a.execute();
        } else {
            a.reverse();
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

