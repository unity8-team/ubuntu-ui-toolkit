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

#include "ucunits.h"
#include "uctheme.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "propertychange_p.h"
#include "quickutils.h"
#include "i18n.h"
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQml/QQmlInfo>

#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>

/*
 * The properties are attached to the ListItem's parent item or to its closest
 * Flickable parent, when embedded in ListView or Flickable. There will be only
 * one attached property per Flickable for all embedded child ListItems, enabling
 * in this way the controlling of the interactive flag of the Flickable and all
 * its ascendant Flickables.
 */
UCListItemAttachedPrivate::UCListItemAttachedPrivate(UCListItemAttached *qq)
    : q_ptr(qq)
    , listView(0)
    , dragHandlerArea(0)
    , globalDisabled(false)
    , selectable(false)
    , draggable(false)
    , dragSuspended(false)
    , dragVisible(0)
    , dragTempItem(0)
    , dragFromIndex(-1)
    , dragToIndex(-1)
    , dragMinimum(-1)
    , dragMaximum(-1)
{
}

UCListItemAttachedPrivate::~UCListItemAttachedPrivate()
{
    clearChangesList();
    clearFlickablesList();
}

// disconnect all flickables
void UCListItemAttachedPrivate::clearFlickablesList()
{
    Q_Q(UCListItemAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, flickables) {
        if (flickable.data())
        QObject::disconnect(flickable.data(), &QQuickFlickable::movementStarted,
                            q, &UCListItemAttached::unbindItem);
    }
    flickables.clear();
}

// connect all flickables
void UCListItemAttachedPrivate::buildFlickablesList()
{
    Q_Q(UCListItemAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearFlickablesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            QObject::connect(flickable, &QQuickFlickable::movementStarted,
                             q, &UCListItemAttached::unbindItem);
            flickables << flickable;
        }
        item = item->parentItem();
    }
}

void UCListItemAttachedPrivate::clearChangesList()
{
    // clear property change objects
    Q_Q(UCListItemAttached);
    Q_FOREACH(PropertyChange *change, changes) {
        // deleting PropertyChange will restore the saved property
        // to its original binding/value
        delete change;
    }
    changes.clear();
}

void UCListItemAttachedPrivate::buildChangesList(const QVariant &newValue)
{
    // collect all ascendant flickables
    Q_Q(UCListItemAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearChangesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            PropertyChange *change = new PropertyChange(item, "interactive");
            PropertyChange::setValue(change, newValue);
            changes << change;
        }
        item = item->parentItem();
    }
}

UCListItemAttached::UCListItemAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCListItemAttachedPrivate(this))
{
    if (QuickUtils::inherits(owner, "QQuickListView")) {
        Q_D(UCListItemAttached);
        d->listView = static_cast<QQuickFlickable*>(owner);
    }
}

UCListItemAttached::~UCListItemAttached()
{
}

// register item to be rebound
bool UCListItemAttached::listenToRebind(UCListItem *item, bool listen)
{
    // we cannot bind the item until we have an other one bound
    bool result = false;
    Q_D(UCListItemAttached);
    if (listen) {
        if (d->boundItem.isNull() || (d->boundItem == item)) {
            d->boundItem = item;
            // rebuild flickable list
            d->buildFlickablesList();
            result = true;
        }
    } else if (d->boundItem == item) {
        d->boundItem.clear();
        result = true;
    }
    return result;
}

// reports true if any of the ascendant flickables is moving
bool UCListItemAttached::isMoving()
{
    Q_D(UCListItemAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, d->flickables) {
        if (flickable && flickable->isMoving()) {
            return true;
        }
    }
    return false;
}

// returns true if the given ListItem is bound to listen on moving changes
bool UCListItemAttached::isBoundTo(UCListItem *item)
{
    Q_D(UCListItemAttached);
    return d->boundItem == item;
}

/*
 * Disable/enable interactive flag for the ascendant flickables. The item is used
 * to detect whether the same item is trying to enable the flickables which disabled
 * it before. The enabled/disabled states are not equivalent to the enabled/disabled
 * state of the interactive flag.
 * When disabled, always the last item disabling will be kept as active disabler,
 * and only the active disabler can enable (restore) the interactive flag state.
 */
void UCListItemAttached::disableInteractive(UCListItem *item, bool disable)
{
    Q_D(UCListItemAttached);
    if (disable) {
        // disabling or re-disabling
        d->disablerItem = item;
        if (d->globalDisabled == disable) {
            // was already disabled, leave
            return;
        }
        d->globalDisabled = true;
    } else if (d->globalDisabled && d->disablerItem == item) {
        // the one disabled it will enable
        d->globalDisabled = false;
        d->disablerItem.clear();
    } else {
        // !disabled && (!globalDisabled || item != d->disablerItem)
        return;
    }
    if (disable) {
        // (re)build changes list with disabling the interactive value
        d->buildChangesList(false);
    } else {
        d->clearChangesList();
    }
}

void UCListItemAttached::unbindItem()
{
    Q_D(UCListItemAttached);
    if (d->boundItem) {
        // depending on content item's X coordinate, we either do animated or prompt rebind
        if (d->boundItem->contentItem()->x() != 0.0) {
            // content is not in origin, rebind
            UCListItemPrivate::get(d->boundItem.data())->_q_rebound();
        } else {
            // do some cleanup
            UCListItemPrivate::get(d->boundItem.data())->promptRebound();
        }
        d->boundItem.clear();
    }
    // clear binding list
    d->clearFlickablesList();
}

/*!
 * \qmlattachedproperty bool ListItem::selectMode
 * The property drives whether list items are selectable or not. The property is
 * attached to the ListItem's parent or to the ListView/Flickable owning the
 * ListItems.
 *
 *
 * When set, the items
 * will show a check box on the leading side hanving the content item pushed towards
 * trailing side and dimmed. The checkbox which will reflect and drive the \l selected
 * state.
 * Defaults to false.
 */
bool UCListItemAttachedPrivate::selectMode() const
{
    return selectable;
}
void UCListItemAttachedPrivate::setSelectMode(bool value)
{
    if (selectable == value) {
        return;
    }
    selectable = value;
    Q_Q(UCListItemAttached);
    Q_EMIT q->selectModeChanged();
}

/*!
 * \qmlattachedproperty list<int> ListItem::selectedIndexes
 * The property is automatically attached to the ListItem's parent item, or to
 * the ListView when used with ListView. Contains the indexes of the ListItems
 * marked as selected. The indexes are model indexes when used in ListView, and
 * child indexes in other contexts.
 * \note Setting the ListItem's \l selected property to \c true will add the
 * item index to the selection list automatically, and may destroy the initial
 * state of the selection. Therefore it is recommended to drive the selection
 * through the attached property rather through the \l ListItem::selected property.
 * \sa ListItem::selectable, ListItem::selected
 */
QList<int> UCListItemAttachedPrivate::selectedIndexes() const
{
    return selectedList;
}
void UCListItemAttachedPrivate::setSelectedIndexes(const QList<int> &list)
{
    if (selectedList == list) {
        return;
    }
    selectedList = list;
    Q_Q(UCListItemAttached);
    Q_EMIT q->selectedIndexesChanged();
}

void UCListItemAttachedPrivate::addSelectedItem(UCListItem *item)
{
    int index = UCListItemPrivate::get(item)->index();
    if (!selectedList.contains(index)) {
        selectedList.append(index);
        Q_EMIT q_ptr->selectedIndexesChanged();
    }
}
void UCListItemAttachedPrivate::removeSelectedItem(UCListItem *item)
{
    if (selectedList.removeAll(UCListItemPrivate::get(item)->index()) > 0) {
        Q_EMIT q_ptr->selectedIndexesChanged();
    }
}

bool UCListItemAttachedPrivate::isItemSelected(UCListItem *item)
{
    return selectedList.contains(UCListItemPrivate::get(item)->index());
}

/*!
 * \qmlattachedproperty bool ListItem::dragMode
 * The property drives the dragging mode of the ListItems within a ListView. It
 * has no effect on any other parent of the ListItem.
 *
 * When set, ListItem content will be disabled and a panel will be shown enabling
 * the dragging mode. The items can be dragged by dragging this handler only.
 * The feature can be activated same time with \l selectable.
 *
 * The panel is configured by the \l {ListItemStyle::dragHandlerDelegate}{dragHandlerDelegate}
 * component.
 *
 * \sa ListItemStyle::dragHandlerDelegate, draggingStarted
 */

/*!
 * \qmlattachedsignal ListItem::draggingStarted(ListItemDrag event)
 * The signal is emitted when a ListItem dragging is started. \c event.from
 * specifies the index of the ListItem being dragged. \c event.minimumIndex and
 * \c event.maximumIndex configures the index interval the dragging of the item
 * is allowed. If set (meaning their value differs from -1), items cannot be
 * dragged outside of this region. The \c event.accept property, if set to false,
 * will cancel dragging operation. The other fields of the event (i.e. \c event.to
 * and \c event.direction) contain invalid data.
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     width: units.gu(40)
 *     height: units.gu(40)
 *     model: ListModel {
 *         // initiate with random data
 *     }
 *     delegate: ListItem {
 *         // content
 *     }
 *
 *     ListItem.dragMode: true
 *     ListItem.onDraggingStarted: {
 *         if (event.from < 5) {
 *             // deny dragging on the first 5 element
 *             event.accept = false;
 *         } else if (event.from >= 5 && event.from <= 10) {
 *             // specify the interval
 *             event.minimumIndex = 5;
 *             event.maximumIndex = 10;
 *         }
 *     }
 * }
 * \endqml
 *
 * In the example above the first 5 items are not draggable, though drag handler
 * will still be shown for them. If the drag starts on item index 5, it will only
 * accept drag gesture downwards, respectively starting a drag on item index 10
 * will only allow dragging that element upwards. Every item dragged between
 * indexes 5 and 10 will be draggable both directions, however only till 5th or
 * 10th index. On the other hand, items dragged from index > 10 will be draggable
 * to any index, including the first 11 items. In order to avoid dragging those
 * items in between the first 11 items, the following change must be made:
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     width: units.gu(40)
 *     height: units.gu(40)
 *     model: ListModel {
 *         // initiate with random data
 *     }
 *     delegate: ListItem {
 *         // content
 *     }
 *
 *     ListItem.dragMode: true
 *     ListItem.onDraggingStarted: {
 *         if (event.from < 5) {
 *             // deny dragging on the first 5 element
 *             event.accept = false;
 *         } else if (event.from >= 5 && event.from <= 10) {
 *             // specify the interval
 *             event.minimumIndex = 5;
 *             event.maximumIndex = 10;
 *         } else if (event.from > 10) {
 *             // prevent dragging to the first 11 items area
 *             event.minimumIndex = 11;
 *         }
 *     }
 * }
 * \endqml
 *
 * \note None of the above examples will move the dragged item. In order that to
 * happen, you must implement \l draggingUpdated signal and move the model data.
 * \note Implementing the signal handler is not mandatory and should only happen
 * if restrictions on the drag must be applied.
 */

/*!
 * \qmlattachedsignal ListItem::draggingUpdated(ListItemDrag event)
 * The signal is emitted when the list item from \c event.from index has been
 * dragged over to \c event.to, and a move operation is possible. Implementations
 * \b {must move the model data} between these indexes. If the move is not acceptable,
 * it can be cancelled by setting \c event.accept to \c false, in which case the
 * dragged item will stay on its last moved position or will snap back to its
 * previous or original place, depending whether the drag was sent during the
 * drag or as a result of a drop gesture. The direction of the drag is given in the
 * \c event.direction property. Extending the example from \l draggingStarted, an
 * implementation of a live dragging would look as follows
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     width: units.gu(40)
 *     height: units.gu(40)
 *     model: ListModel {
 *         // initiate with random data
 *     }
 *     delegate: ListItem {
 *         // content
 *     }
 *
 *     ListItem.dragMode: true
 *     ListItem.onDraggingStarted: {
 *         if (event.from < 5) {
 *             // deny dragging on the first 5 element
 *             event.accept = false;
 *         } else if (event.from >= 5 && event.from <= 10) {
 *             // specify the interval
 *             event.minimumIndex = 5;
 *             event.maximumIndex = 10;
 *         } else {
 *             // prevent dragging to the first 11 items area
 *             event.minimumIndex = 11;
 *         }
 *     }
 *     ListItem.onDraggingUpdated: {
 *         model.move(event.from, event.to, 1);
 *     }
 * }
 * \endqml
 *
 * \c event.direction set to \e ListItemDrag.None means the signal is sent as a
 * result of a drop operation, and this was the last update signal emitted. The
 * following sample shows how to implement list reordering when dropping the item
 * (non-live update).
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *    width: units.gu(40)
 *    height: units.gu(40)
 *    model: ListModel {
 *        // initiate with random data
 *    }
 *    delegate: ListItem {
 *        // content
 *    }
 *
 *    ListItem.dragMode: true
 *    ListItem.onDraggingUpdated: {
 *        if (event.direction == ListItemDrag.None) {
 *            // this is the last event, so drop the item
 *            model.move(event.from, event.to, 1);
 *        } else {
 *            // do not accept the other events so drag.from will
 *            // always contain the original drag index
 *            event.accept = false;
 *        }
 *    }
 * }
 * \endqml
 */

bool UCListItemAttachedPrivate::dragMode() const
{
    return draggable;
}
void UCListItemAttachedPrivate::setDragMode(bool value)
{
    if (draggable == value) {
        return;
    }
    Q_Q(UCListItemAttached);
    if (value) {
        /*
         * The dragging works only if the ListItem is used inside a ListView, and the
         * model used is a list, a ListModel or a derivate of QAbstractItemModel. Do
         * not enable dragging if these conditions are not fulfilled.
         */
        if (!listView) {
            qmlInfo(q->parent()) << UbuntuI18n::instance().tr("dragging mode requires ListView");
            return;
        }
        QVariant modelValue = listView->property("model");
        if (!modelValue.isValid()) {
            return;
        }
        if (modelValue.type() == QVariant::Int || modelValue.type() == QVariant::Double) {
            qmlInfo(listView) << UbuntuI18n::instance().tr("model must be a list, ListModel or a derivate of QAbstractItemModel");
            return;
        }
    }
    draggable = value;
    if (draggable) {
        // enter drag mode
        enterDragMode();
    } else {
        // exit drag mode
        leaveDragMode();
    }
    Q_EMIT q->dragModeChanged();
}

void UCListItemAttachedPrivate::enterDragMode()
{
    dragHandlerArea = new QQuickMouseArea(listView);
    dragHandlerArea->setParentItem(listView);
    QQuickAnchors *areaAnchors = QQuickItemPrivate::get(dragHandlerArea)->anchors();
    QQuickItemPrivate *listViewPrivate = QQuickItemPrivate::get(listView);
    areaAnchors->setTop(listViewPrivate->top());
    areaAnchors->setBottom(listViewPrivate->bottom());
    areaAnchors->setRight(listViewPrivate->right());
    dragHandlerArea->setWidth(UCUnits::instance().gu(5));
    // connect onPressed to get signals when to start, stop and update dragging
    Q_Q(UCListItemAttached);
    QObject::connect(dragHandlerArea, SIGNAL(pressed(QQuickMouseEvent*)),
                     q, SLOT(startDragging(QQuickMouseEvent*)));
    QObject::connect(dragHandlerArea, &QQuickMouseArea::released,
                     q, &UCListItemAttached::stopDragging);
    QObject::connect(dragHandlerArea, &QQuickMouseArea::positionChanged,
                     q, &UCListItemAttached::updateDragging);

    if (!isDraggingUpdatedConnected()) {
        qmlInfo(listView) << UbuntuI18n::instance().
                             tr("Listview has no ListItem.draggingUpdated signal handler " \
                                "implemented. No dragging will be possible.");
    }
}

void UCListItemAttachedPrivate::leaveDragMode()
{
    delete dragHandlerArea;
    dragHandlerArea = 0;
}

void UCListItemAttached::startDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCListItemAttached);
    QPointF pos = d->mapDragAreaPos();
    UCListItem *listItem = d->itemAt(pos.x(), pos.y());
    if (!listItem) {
        return;
    }
    int index = d->indexAt(pos.x(), pos.y());
    bool start = true;
    d->dragMinimum = -1;
    d->dragMaximum = -1;
    if (d->isDraggingStartedConnected()) {
        UCDragEvent event(UCDragEvent::None, index, -1, -1, -1);
        Q_EMIT draggingStarted(&event);
        start = event.m_accept;
        // set limits
        d->dragMinimum = event.m_minimum;
        d->dragMaximum = event.m_maximum;
    }
    if (start) {
        d->dragItem = listItem;
        d->dragVisible = new PropertyChange(d->dragItem, "visible");
        d->dragToIndex = d->dragFromIndex = index;
        d->dragLastY = pos.y();
        d->createDraggedItem();

        // lock ListView and rise z-order
        disableInteractive(d->dragItem, true);
        PropertyChange::setValue(d->dragVisible, false);
        UCListItemPrivate::get(d->dragTempItem)->dragHandler->setDragging(true);
    }
}

void UCListItemAttached::stopDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCListItemAttached);
    if (d->dragItem) {
        if (d->isDraggingUpdatedConnected()) {
            UCDragEvent dragEvent(UCDragEvent::None, d->dragFromIndex, d->dragToIndex, d->dragMinimum, d->dragMaximum);
            Q_EMIT draggingUpdated(&dragEvent);
        }
        UCListItemPrivate::get(d->dragTempItem)->dragHandler->setDragging(false);
        disableInteractive(d->dragItem, false);
        delete d->dragVisible;
        d->dragVisible = 0;
        delete d->dragTempItem;
        d->dragTempItem = 0;
        d->dragToIndex = d->dragFromIndex = -1;
    }
}

void UCListItemAttached::updateDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCListItemAttached);
    if (d->dragItem) {
        QPointF pos = d->mapDragAreaPos();
        qreal dy = -(d->dragLastY - pos.y());
        // check what will be the index after the drag
        int index = d->indexAt(pos.x(), d->dragLastY + dy);
        if (index < 0) {
            return;
        }
        if ((d->dragMinimum >= 0) && (d->dragMinimum > index)) {
            // about to drag beyond the minimum, leave
            return;
        }
        if ((d->dragMaximum >= 0) && (d->dragMaximum < index)) {
            // about to drag beyond maximum, leave
            return;
        }

        // update dragged item
        d->dragTempItem->setY(d->dragTempItem->y() + dy);
        d->dragLastY += dy;
        // do we have index change?
        if (d->dragToIndex == index) {
            // no change, leave
            return;
        }
        UCDragEvent::Direction direction = (d->dragToIndex > index) ? UCDragEvent::Upwards : UCDragEvent::Downwards;

        d->dragToIndex = index;
        if ((d->dragFromIndex != d->dragToIndex) && (d->dragToIndex != -1)) {
            bool update = true;
            if (d->isDraggingUpdatedConnected()) {
                UCDragEvent event(direction, d->dragFromIndex, d->dragToIndex, d->dragMinimum, d->dragMaximum);
                Q_EMIT draggingUpdated(&event);
                update = event.m_accept;
            }
            if (update) {
                d->dragFromIndex = d->dragToIndex;
            }
        }
    }
}

bool UCListItemAttachedPrivate::isDraggingStartedConnected()
{
    Q_Q(UCListItemAttached);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCListItemAttached::draggingStarted);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

bool UCListItemAttachedPrivate::isDraggingUpdatedConnected()
{
    Q_Q(UCListItemAttached);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCListItemAttached::draggingUpdated);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

QPointF UCListItemAttachedPrivate::mapDragAreaPos()
{
    QPointF pos(dragHandlerArea->mouseX(), dragHandlerArea->mouseY() + listView->contentY() - listView->originY());
    pos = listView->mapFromItem(dragHandlerArea, pos);
    return pos;
}


int UCListItemAttachedPrivate::indexAt(qreal x, qreal y)
{
    if (!listView) {
        return -1;
    }
    int result = -1;
    QMetaObject::invokeMethod(listView, "indexAt", Qt::DirectConnection,
                              Q_RETURN_ARG(int, result),
                              Q_ARG(qreal, x),
                              Q_ARG(qreal, y)
                              );
    return result;
}

UCListItem *UCListItemAttachedPrivate::itemAt(qreal x, qreal y)
{
    if (!listView) {
        return 0;
    }
    QQuickItem *result = 0;
    QMetaObject::invokeMethod(listView, "itemAt", Qt::DirectConnection,
                              Q_RETURN_ARG(QQuickItem*, result),
                              Q_ARG(qreal, x),
                              Q_ARG(qreal, y)
                              );
    return static_cast<UCListItem*>(result);
}


void UCListItemAttachedPrivate::createDraggedItem()
{
    if (dragTempItem || !dragItem) {
        return;
    }
    QQmlComponent *delegate = listView->property("delegate").value<QQmlComponent*>();
    if (!delegate) {
    }
    // use dragItem's context to get access to the ListView's model roles
    dragTempItem = static_cast<UCListItem*>(delegate->create(qmlContext(dragItem)));
    dragTempItem->setVisible(false);
    dragTempItem->setParentItem(listView->contentItem());
    // initialize style and turn panels on
    UCListItemPrivate *pItem = UCListItemPrivate::get(dragTempItem);
    pItem->initStyleItem();
    if (pItem->isSelectable()) {
        pItem->selectionHandler->setupSelection();
    }
    if (pItem->isDraggable()) {
        pItem->dragHandler->setupDragMode();
    }
    dragTempItem->setX(dragItem->x());
    dragTempItem->setY(dragItem->y());
    dragTempItem->setZ(2);
    dragTempItem->setWidth(dragItem->width());
    dragTempItem->setHeight(dragItem->height());
    QColor color = dragItem->color();
    if (color.alphaF() == 0.0) {
        color = QuickUtils::instance().rootItem(listView)->property("backgroundColor").value<QColor>();
    }
    dragTempItem->setColor(color);
    dragTempItem->setVisible(true);
}
