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
    Action(QObject *, const QString &, const QVariant &);
    void execute();
    void reverse();

    QQmlProperty property;
    QVariant fromValue;
    QVariant toValue;

    QQmlAbstractBinding *fromBinding;
    QWeakPointer<QQmlAbstractBinding> toBinding;
};

Action::Action(QObject *target, const QString &propertyName, const QVariant &value)
    : property(target, propertyName, qmlEngine(target))
    , toValue(value)
    , fromBinding(0)
{
    if (property.isValid())
        fromValue = property.read();
}

void Action::execute()
{
//    fromBinding = QQmlPropertyPrivate::binding(property);
//    if (!fromBinding) {
//        fromValue = property.read();
//    }

    if (!toBinding.isNull()) {
        QQmlPropertyPrivate::setBinding(property, toBinding.data(), QQmlPropertyPrivate::DontRemoveBinding);
    } else {
        property.write(toValue);
    }
}

void Action::reverse()
{
    if (fromBinding) {
        QQmlPropertyPrivate::setBinding(property, fromBinding, QQmlPropertyPrivate::DontRemoveBinding);
    } else {
        property.write(fromValue);
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
        actions << Action(currentLayout->m_items.at(i), "parent", qVariantFromValue(q));
        actions << Action(currentLayout->m_items.at(i), "visible", true);
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
    // create copy of items list, to keep track of which ones we change
    QHash <QString, QQuickItem*> unusedItems = items;

    // iterate through the Layout definition to find those Items with Layout.item set
    QList<QObject *> layoutChildren = currentLayout->findChildren<QObject *>();

    for (int i = 0; i < layoutChildren.count(); i++) {
        QObject *child = static_cast<QObject*>(layoutChildren.at(i));

        if (child->inherits("LayoutItem")) {
            const QString itemName = child->property("itemName").toString();

            if (unusedItems.contains(itemName)) {
                // set all properties of the Item to be those defined on the LayoutItem
                const QMetaObject *childMoc = child->metaObject();
                QQuickItem *itemToMove = unusedItems.value(itemName);

                for (int i=0; i<childMoc->propertyCount(); i++) {
                    QString propertyName = childMoc->property(i).name();
                    actions << Action(itemToMove,
                                      propertyName,
                                      child->property(propertyName.toLatin1().constData()));
                }

                unusedItems.remove(itemName);
            } else {
                qDebug() << "WARNING: LayoutItem with unrecognised itemName" << child->property("itemName");
            }

        } else {
            // has child the Layout.item attached property set?
            LayoutAttached *attached =
                    qobject_cast<LayoutAttached*>(qmlAttachedPropertiesObject<Layout>(child, false));

            if (attached != 0 && attached->item() != "") {
                if (unusedItems.contains(attached->item())) {
                    actions << Action(unusedItems.value(attached->item()), "parent", qVariantFromValue(child));
                    unusedItems.remove(attached->item());
                }
            }
        }
    }

    QHashIterator<QString, QQuickItem*> i(unusedItems);
    while (i.hasNext()) {
        i.next();
        actions << Action(i.value(), "visible", false);
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

