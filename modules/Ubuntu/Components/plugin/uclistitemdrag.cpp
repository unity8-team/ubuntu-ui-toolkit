
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include "propertychange_p.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickflickable_p.h>

UCDragHandler::UCDragHandler(UCListItem *listItem)
    : UCHandlerBase(listItem)
    , dragging(false)
{
}

QPointF UCDragHandler::panelCenterToListView()
{
    QQuickFlickable *listView = UCListItemAttachedPrivate::get(listItem->attachedProperties)->listView;
    QPointF pos(panel->width() / 2, panel->height() / 2);
    pos = listView->mapFromItem(static_cast<QQuickItem*>(listItem->item()), pos);
    return pos;
}

QPointF UCDragHandler::mapMousePosToListView(QEvent *event)
{
    QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
    return mouse->windowPos();
}

QPointF UCDragHandler::deltaPos(const QPointF &pos)
{
    qreal dx = -(lastPos.x() - pos.x());
    qreal dy = -(lastPos.y() - pos.y());
    QPointF result(dx, dy);
    lastPos = pos;
    return result;
}

bool UCDragHandler::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // notify attached about drag start
        UCListItemAttachedPrivate *attached = UCListItemAttachedPrivate::get(listItem->attachedProperties);
        attached->startDragOnItem(listItem, panelCenterToListView());
        lastPos = mapMousePosToListView(event);
        return true;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QPointF pos = mapMousePosToListView(event);
        UCListItemAttachedPrivate *attached = UCListItemAttachedPrivate::get(listItem->attachedProperties);
        attached->dropItem(deltaPos(pos));
    } else if (event->type() == QEvent::MouseMove) {
        QPointF pos = mapMousePosToListView(event);
        UCListItemAttachedPrivate *attached = UCListItemAttachedPrivate::get(listItem->attachedProperties);
        attached->updateDragPosition(deltaPos(pos));
    }
    return QObject::eventFilter(watched, event);
}

// listen for attached property's draggable change signal to activate dragging mode on the list item
void UCDragHandler::initialize()
{
    if (!listItem->attachedProperties) {
        return;
    }
    connect(listItem->attachedProperties, &UCListItemAttached::dragModeChanged,
            this, &UCDragHandler::setupDragMode);
    if (listItem->isDraggable()) {
        setupDragMode();
    }
}

void UCDragHandler::setupDragMode()
{
    // make sure the ListItem is snapped out
    bool draggable = listItem->isDraggable();
    if (draggable) {
        listItem->promptRebound();
        // animate panel only in case is called due to a signal emit
        listItem->initStyleItem();
        if (!panel && listItem->styleItem && listItem->styleItem->m_dragHandlerDelegate) {
            bool animate = (senderSignalIndex() >= 0);
            setupPanel(listItem->styleItem->m_dragHandlerDelegate, animate);
        }
        // install an event filter to catch mouse press events, as those are not
        // intercepted by the ListView filter
        if (panel) {
            // make panel to accept mouse buttons so we get mouse events over it
            panel->setAcceptedMouseButtons(Qt::LeftButton);
            panel->installEventFilter(this);
        }
    } else if (panel) {
        // disable mouse buttons from panel while not in drag mode
        panel->setAcceptedMouseButtons(Qt::NoButton);
        panel->removeEventFilter(this);
    }

    // update visuals
    listItem->update();
}

