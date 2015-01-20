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
#include "uclistitemstyle.h"
#include "propertychange_p.h"
#include "quickutils.h"
#include "i18n.h"
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQml/QQmlInfo>

#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQml/private/qqmlcomponentattached_p.h>

/*
 * The properties are attached to the ListItem's parent item or to its closest
 * Flickable parent, when embedded in ListView or Flickable. There will be only
 * one attached property per Flickable for all embedded child ListItems, enabling
 * in this way the controlling of the interactive flag of the Flickable and all
 * its ascendant Flickables.
 */
UCViewItemsAttachedPrivate::UCViewItemsAttachedPrivate(UCViewItemsAttached *qq)
    : q_ptr(qq)
    , listView(0)
    , ready(false)
    , globalDisabled(false)
    , selectable(false)
    , draggable(false)
    , dragHandlerArea(0)
    , dragTempItem(0)
    , dragLastY(0)
    , dragFromIndex(-1)
    , dragToIndex(-1)
    , dragMinimum(-1)
    , dragMaximum(-1)
{
}

UCViewItemsAttachedPrivate::~UCViewItemsAttachedPrivate()
{
    clearChangesList();
    clearFlickablesList();
}

// disconnect all flickables
void UCViewItemsAttachedPrivate::clearFlickablesList()
{
    Q_Q(UCViewItemsAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, flickables) {
        if (flickable) {
            QObject::disconnect(flickable, &QQuickFlickable::movementStarted,
                                q, &UCViewItemsAttached::unbindItem);
            QObject::disconnect(flickable, &QQuickFlickable::flickStarted,
                                q, &UCViewItemsAttached::unbindItem);
        }
    }
    flickables.clear();
}

// connect all flickables
void UCViewItemsAttachedPrivate::buildFlickablesList()
{
    Q_Q(UCViewItemsAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearFlickablesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            QObject::connect(flickable, &QQuickFlickable::movementStarted,
                             q, &UCViewItemsAttached::unbindItem);
            QObject::connect(flickable, &QQuickFlickable::flickStarted,
                             q, &UCViewItemsAttached::unbindItem);
            flickables << flickable;
        }
        item = item->parentItem();
    }
}

void UCViewItemsAttachedPrivate::clearChangesList()
{
    // clear property change objects
    Q_Q(UCViewItemsAttached);
    Q_FOREACH(PropertyChange *change, changes) {
        // deleting PropertyChange will restore the saved property
        // to its original binding/value
        delete change;
    }
    changes.clear();
}

void UCViewItemsAttachedPrivate::buildChangesList(const QVariant &newValue)
{
    // collect all ascendant flickables
    Q_Q(UCViewItemsAttached);
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

/*!
 * \qmltype ViewItems
 * \instantiates UCViewItemsAttached
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief A set of properties attached to the ListItem's parent item or ListView.
 *
 * These properties are attached to the parent item of the ListItem, or to
 * ListView, when the component is used as delegate.
 */
UCViewItemsAttached::UCViewItemsAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCViewItemsAttachedPrivate(this))
{
    if (owner->inherits("QQuickListView")) {
        d_ptr->listView = static_cast<QQuickFlickable*>(owner);
    }
    // check attachee's readyness
    QQmlComponentAttached *attached = QQmlComponent::qmlAttachedProperties(owner);
    connect(attached, SIGNAL(completed()), this, SLOT(completed()));
}

UCViewItemsAttached::~UCViewItemsAttached()
{
}

UCViewItemsAttached *UCViewItemsAttached::qmlAttachedProperties(QObject *owner)
{
    return new UCViewItemsAttached(owner);
}

// register item to be rebound
bool UCViewItemsAttached::listenToRebind(UCListItem *item, bool listen)
{
    // we cannot bind the item until we have an other one bound
    bool result = false;
    Q_D(UCViewItemsAttached);
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
bool UCViewItemsAttached::isMoving()
{
    Q_D(UCViewItemsAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, d->flickables) {
        if (flickable && flickable->isMoving()) {
            return true;
        }
    }
    return false;
}

// returns true if the given ListItem is bound to listen on moving changes
bool UCViewItemsAttached::isBoundTo(UCListItem *item)
{
    Q_D(UCViewItemsAttached);
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
void UCViewItemsAttached::disableInteractive(UCListItem *item, bool disable)
{
    Q_D(UCViewItemsAttached);
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

void UCViewItemsAttached::unbindItem()
{
    Q_D(UCViewItemsAttached);
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

void UCViewItemsAttached::completed()
{
    Q_D(UCViewItemsAttached);
    d->ready = true;
    if (d->draggable) {
        d->enterDragMode();
    } else {
        d->leaveDragMode();
    }
}

/*!
 * \qmlattachedproperty bool ViewItems::selectMode
 * The property drives whether list items are selectable or not.
 *
 * When set, the default style implementation will show a check box on the leading
 * side hanving the content item pushed towards trailing side and dimmed. The checkbox
 * will reflect and drive the \l ListItem::selected state. Defaults to \c false.
 */
bool UCViewItemsAttached::selectMode() const
{
    Q_D(const UCViewItemsAttached);
    return d->selectable;
}
void UCViewItemsAttached::setSelectMode(bool value)
{
    Q_D(UCViewItemsAttached);
    if (d->selectable == value) {
        return;
    }
    d->selectable = value;
    Q_EMIT selectModeChanged();
}

/*!
 * \qmlattachedproperty list<int> ViewItems::selectedIndices
 * The property contains the indexes of the ListItems marked as selected. The
 * indexes are model indexes when used in ListView, and child indexes in other
 * components. The property being writable, initial selection configuration
 * can be provided for a view, and provides ability to save the selection state.
 */
QList<int> UCViewItemsAttached::selectedIndices() const
{
    Q_D(const UCViewItemsAttached);
    return d->selectedList.toList();
}
void UCViewItemsAttached::setSelectedIndices(const QList<int> &list)
{
    Q_D(UCViewItemsAttached);
    if (d->selectedList.toList() == list) {
        return;
    }
    d->selectedList = QSet<int>::fromList(list);
    Q_EMIT selectedIndicesChanged();
}

bool UCViewItemsAttachedPrivate::addSelectedItem(UCListItem *item)
{
    int index = UCListItemPrivate::get(item)->index();
    if (!selectedList.contains(index)) {
        selectedList.insert(index);
        Q_EMIT q_ptr->selectedIndicesChanged();
        return true;
    }
    return false;
}
bool UCViewItemsAttachedPrivate::removeSelectedItem(UCListItem *item)
{
    if (selectedList.remove(UCListItemPrivate::get(item)->index()) > 0) {
        Q_EMIT q_ptr->selectedIndicesChanged();
        return true;
    }
    return false;
}

bool UCViewItemsAttachedPrivate::isItemSelected(UCListItem *item)
{
    return selectedList.contains(UCListItemPrivate::get(item)->index());
}

/*!
 * \qmlattachedproperty bool ViewItems::dragMode
 * The property drives the dragging mode of the ListItems within a ListView. It
 * has no effect on any other parent of the ListItem.
 *
 * When set, ListItem content will be disabled and a panel will be shown enabling
 * the dragging mode. The items can be dragged by dragging this handler only.
 * The feature can be activated same time with \l ListItem::selectable.
 *
 * The panel is configured by the \l {ListItemStyle::dragHandlerDelegate}{dragHandlerDelegate}
 * component.
 *
 * \sa ListItemStyle::dragHandlerDelegate, draggingStarted
 */

/*!
 * \qmlattachedsignal ViewItems::draggingStarted(ListItemDrag event)
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
 *     ViewItems.dragMode: true
 *     ViewItems.onDraggingStarted: {
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
 *     ViewItems.dragMode: true
 *     ViewItems.onDraggingStarted: {
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
 * }
 * \endqml
 *
 * \note None of the above examples will move the dragged item. In order that to
 * happen, you must implement \l draggingUpdated signal and move the model data.
 * \note Implementing the signal handler is not mandatory and should only happen
 * if restrictions on the drag must be applied.
 */

/*!
 * \qmlattachedsignal ViewItems::draggingUpdated(ListItemDrag event)
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
 *     ViewItems.dragMode: true
 *     ViewItems.onDraggingStarted: {
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
 *     ViewItems.onDraggingUpdated: {
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
 *
 * \note Do not forget to set \b{event.attached} to false in \l draggingUpdated,
 * otherwise the system will not know whether the move has been performed or not,
 * and selected indexes will not be synchronized properly.
 */
bool UCViewItemsAttached::dragMode() const
{
    Q_D(const UCViewItemsAttached);
    return d->draggable;
}
void UCViewItemsAttached::setDragMode(bool value)
{
    Q_D(UCViewItemsAttached);
    if (d->draggable == value) {
        return;
    }
    if (value) {
        /*
         * The dragging works only if the ListItem is used inside a ListView, and the
         * model used is a list, a ListModel or a derivate of QAbstractItemModel. Do
         * not enable dragging if these conditions are not fulfilled.
         */
        if (!d->listView) {
            qmlInfo(parent()) << UbuntuI18n::instance().tr("dragging mode requires ListView");
            return;
        }
        QVariant modelValue = d->listView->property("model");
        if (!modelValue.isValid()) {
            return;
        }
    }
    d->draggable = value;
    if (d->draggable) {
        // enter drag mode
        d->enterDragMode();
    } else {
        // exit drag mode
        d->leaveDragMode();
    }
    Q_EMIT dragModeChanged();
}

// enters dragging mode, creates a MouseArea over the right side of the item
void UCViewItemsAttachedPrivate::enterDragMode()
{
    if (dragHandlerArea || !ready) {
        return;
    }
    dragHandlerArea = new QQuickMouseArea(listView);
    dragHandlerArea->setObjectName("draghandler_area");
    dragHandlerArea->setParentItem(listView);
    QQuickAnchors *areaAnchors = QQuickItemPrivate::get(dragHandlerArea)->anchors();
    QQuickItemPrivate *listViewPrivate = QQuickItemPrivate::get(listView);
    areaAnchors->setTop(listViewPrivate->top());
    areaAnchors->setBottom(listViewPrivate->bottom());
    areaAnchors->setRight(listViewPrivate->right());
    dragHandlerArea->setWidth(UCUnits::instance().gu(5));
    // connect onPressed to get signals when to start, stop and update dragging
    Q_Q(UCViewItemsAttached);
    QObject::connect(dragHandlerArea, SIGNAL(pressed(QQuickMouseEvent*)),
                     q, SLOT(startDragging(QQuickMouseEvent*)));
    QObject::connect(dragHandlerArea, &QQuickMouseArea::released,
                     q, &UCViewItemsAttached::stopDragging);
    QObject::connect(dragHandlerArea, &QQuickMouseArea::positionChanged,
                     q, &UCViewItemsAttached::updateDragging);

    if (!isDraggingUpdatedConnected()) {
        qmlInfo(listView) << UbuntuI18n::instance().
                             tr("Listview has no ListItem.draggingUpdated signal handler " \
                                "implemented. No dragging will be possible.");
    }
}

// leaves the drag mode.
void UCViewItemsAttachedPrivate::leaveDragMode()
{
    delete dragHandlerArea;
    dragHandlerArea = 0;
}

// starts dragging operation; emits draggingStarted() and if the signal handler is implemented,
// depending on the acceptance, will create a fake item and will start dragging. If the start is
// cancelled, no dragging will happen
void UCViewItemsAttached::startDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCViewItemsAttached);
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
        // lock ListView and turn on dragging flags
        d->buildChangesList(false);
        d->dragToIndex = d->dragFromIndex = index;
        d->dragLastY = pos.y();
        d->createDraggedItem(listItem);
    }
}

void UCViewItemsAttached::stopDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCViewItemsAttached);
    if (d->dragTempItem) {
        // stop scroll timer
        d->dragScrollTimer.stop();
        if (d->isDraggingUpdatedConnected() && d->dragFromIndex != d->dragToIndex) {
            UCDragEvent dragEvent(UCDragEvent::None, d->dragFromIndex, d->dragToIndex, d->dragMinimum, d->dragMaximum);
            Q_EMIT draggingUpdated(&dragEvent);
            d->updateDraggedItem();
            if (dragEvent.m_accept) {
                d->updateSelectedIndexes(d->dragFromIndex, d->dragToIndex);
            }
        }
        // unlock flickables
        d->clearChangesList();
        UCListItemPrivate::get(d->dragTempItem)->dragHandler->drop();
        d->dragFromIndex = d->dragToIndex = -1;
    }
}

void UCViewItemsAttached::updateDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    Q_D(UCViewItemsAttached);
    if (d->dragTempItem) {
        QPointF pos = d->mapDragAreaPos();
        qreal dy = -(d->dragLastY - pos.y());
        qreal indexHotspot = d->dragTempItem->y() + d->dragTempItem->height() / 2;
        // update dragged item even if the dragging may be forbidden
        d->dragTempItem->setY(d->dragTempItem->y() + dy);
        d->dragLastY += dy;

        // check what will be the index after the drag
        int index = d->indexAt(pos.x(), indexHotspot);
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

        // should we scroll the view? use a margin of 20% of teh dragger item's height from top and bottom of the item
        qreal scrollMargin = d->dragTempItem->height() * 0.2;
        qreal topHotspot = d->dragTempItem->y() + scrollMargin - d->listView->contentY();
        qreal bottomHotspot = d->dragTempItem->y() + d->dragTempItem->height() - scrollMargin - d->listView->contentY();
        // use MouseArea's top/bottom as limits
        qreal topViewMargin = d->dragHandlerArea->y() + d->listView->topMargin();
        qreal bottomViewMargin = d->dragHandlerArea->y() + d->dragHandlerArea->height() - d->listView->bottomMargin();
        if (topHotspot < topViewMargin) {
            // scroll upwards
            d->dragDirection = UCDragEvent::Upwards;
            if (!d->dragScrollTimer.isActive()) {
                d->dragScrollTimer.start(DRAG_SCROLL_TIMEOUT, this);
            }
        } else if (bottomHotspot > bottomViewMargin) {
            // scroll downwards
            d->dragDirection = UCDragEvent::Downwards;
            if (!d->dragScrollTimer.isActive()) {
                d->dragScrollTimer.start(DRAG_SCROLL_TIMEOUT, this);
            }
        } else if (d->dragScrollTimer.isActive()){
            // stop drag timer
            d->dragScrollTimer.stop();
        }

        // do we have index change?
        if (d->dragToIndex == index) {
            // no change, leave
            return;
        }
        // update drag direction to make sure we're reflecting the proper one
        d->dragDirection = (d->dragToIndex > index) ? UCDragEvent::Upwards : UCDragEvent::Downwards;

        d->dragToIndex = index;
        if (d->dragFromIndex != d->dragToIndex) {
            bool update = true;
            if (d->isDraggingUpdatedConnected()) {
                UCDragEvent event(d->dragDirection, d->dragFromIndex, d->dragToIndex, d->dragMinimum, d->dragMaximum);
                Q_EMIT draggingUpdated(&event);
                update = event.m_accept;
                if (update) {
                    d->updateSelectedIndexes(d->dragFromIndex, d->dragToIndex);
                }
            }
            if (update) {
                // update item coordinates in the dragged item
                d->updateDraggedItem();
                d->dragFromIndex = d->dragToIndex;
            }
        }
    }
}

void UCViewItemsAttached::timerEvent(QTimerEvent *event)
{
    Q_D(UCViewItemsAttached);
    if (event->timerId() == d->dragScrollTimer.timerId()) {
        qreal scrollAmount = UCUnits::instance().gu(0.5) * (d->dragDirection == UCDragEvent::Upwards ? -1 : 1);
        qreal contentHeight = d->listView->contentHeight();
        qreal height = d->listView->height();
        if ((contentHeight - height) > 0) {
            qreal contentY = CLAMP(d->listView->contentY() + scrollAmount, 0, contentHeight - height + d->listView->originY());
            d->listView->setContentY(contentY);
            // update
            d->dragScrollTimer.stop();
            updateDragging(0);
        }
    }
}

bool UCViewItemsAttachedPrivate::isDraggingStartedConnected()
{
    Q_Q(UCViewItemsAttached);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCViewItemsAttached::draggingStarted);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

bool UCViewItemsAttachedPrivate::isDraggingUpdatedConnected()
{
    Q_Q(UCViewItemsAttached);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCViewItemsAttached::draggingUpdated);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

QPointF UCViewItemsAttachedPrivate::mapDragAreaPos()
{
    QPointF pos(dragHandlerArea->mouseX(), dragHandlerArea->mouseY() + listView->contentY());
    pos = listView->mapFromItem(dragHandlerArea, pos);
    return pos;
}


int UCViewItemsAttachedPrivate::indexAt(qreal x, qreal y)
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

UCListItem *UCViewItemsAttachedPrivate::itemAt(qreal x, qreal y)
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


void UCViewItemsAttachedPrivate::createDraggedItem(UCListItem *dragItem)
{
    if (dragTempItem || !dragItem) {
        return;
    }
    QQmlComponent *delegate = listView->property("delegate").value<QQmlComponent*>();
    if (!delegate) {
        return;
    }
    // use dragItem's context to get access to the ListView's model roles
    dragTempItem = static_cast<UCListItem*>(delegate->create(qmlContext(dragItem)));
    dragTempItem->setParentItem(listView->contentItem());
    UCListItemPrivate::get(dragTempItem)->dragHandler->startDragging(dragItem);
}

void UCViewItemsAttachedPrivate::updateDraggedItem()
{
    if (abs(dragFromIndex - dragToIndex) > 0) {
        UCListItem *targetItem = itemAt(dragTempItem->x(), dragTempItem->y() + dragTempItem->height() / 2);
        UCListItemPrivate::get(dragTempItem)->dragHandler->update(targetItem);
    }
}

void UCViewItemsAttachedPrivate::updateSelectedIndexes(int fromIndex, int toIndex)
{
    if (selectedList.count() == listView->property("count").toInt()) {
        // all the items are selected, leave
        return;
    }

    // the direction
    Q_Q(UCViewItemsAttached);
    bool isFromSelected = selectedList.contains(fromIndex);
    if (isFromSelected) {
        selectedList.remove(fromIndex);
        Q_EMIT q->selectedIndicesChanged();
    }
    // direction is -1 (forwards) or 1 (backwards)
    int direction = (fromIndex < toIndex) ? -1 : 1;
    int i = (direction < 0) ? fromIndex + 1 : fromIndex - 1;
    while (1) {
        if (((direction < 0) && (i > toIndex)) ||
            ((direction > 0) && (i < toIndex))) {
            break;
        }

        if (selectedList.contains(i)) {
            selectedList.remove(i);
            selectedList.insert(i + direction);
            Q_EMIT q->selectedIndicesChanged();
        }
        i -= direction;
    }
    if (isFromSelected) {
        selectedList.insert(toIndex);
        Q_EMIT q->selectedIndicesChanged();
    }
}
