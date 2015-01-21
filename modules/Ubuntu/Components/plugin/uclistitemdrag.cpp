
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include "propertychange_p.h"
#include "quickutils.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickanimation_p.h>

UCDragHandler::UCDragHandler(UCListItem *listItem)
    : UCHandlerBase(listItem)
    , dragging(false)
    , isDraggedItem(false)
    , visibleProperty(0)
    , repositionAnimation(0)
{
}

UCDragHandler::~UCDragHandler()
{
    delete visibleProperty;
    visibleProperty = 0;
}

// listen for attached property's draggable change signal to activate dragging mode on the list item
void UCDragHandler::initialize(bool animated)
{
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    if (!pListItem->parentAttached) {
        return;
    }
    connect(pListItem->parentAttached, SIGNAL(dragModeChanged()),
            this, SLOT(setupDragMode()));
    if (pListItem->isDraggable()) {
        setupDragMode(animated);
        // is the current one the dragged source?
        UCViewItemsAttachedPrivate *pAttached = UCViewItemsAttachedPrivate::get(pListItem->parentAttached);
        if (pListItem->index() == pAttached->dragFromIndex) {
            setDragging(true);
            if (pAttached->dragTempItem) {
                pListItem = UCListItemPrivate::get(pAttached->dragTempItem);
                if (!pListItem->dragHandler->originalItem) {
                    // this is the one that has been dragged by the temp item, so update
                    pListItem->dragHandler->originalItem = listItem;
                }
            }
        }
    }
}

void UCDragHandler::setupDragMode(bool animated)
{
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    // make sure the ListItem is snapped out
    bool draggable = pListItem->isDraggable();
    if (draggable) {
        pListItem->promptRebound();
        // animate panel only in case is called due to a signal emit
        pListItem->initStyleItem();
        if (!panel && pListItem->styleItem && pListItem->styleItem->m_dragHandlerDelegate) {
            bool animate = animated || (senderSignalIndex() >= 0);
            setupPanel(pListItem->styleItem->m_dragHandlerDelegate, animate);
        }
    }
}

void UCDragHandler::repositionDraggedItem()
{
    QQuickPropertyAnimation *animation = qobject_cast<QQuickPropertyAnimation*>(sender());
    if (animation) {
        disconnect(animation, &QQuickPropertyAnimation::stopped,
                this, &UCDragHandler::repositionDraggedItem);
    }

    // TODO: somehow get the original if visible again!
    if (originalItem) {
        UCListItemPrivate::get(originalItem)->dragHandler->setDragging(false);
    }
    // hide and delete item
    listItem->setVisible(false);
    listItem->deleteLater();
}

// this method should only be called for the temporary ListItem used in dragging!
void UCDragHandler::startDragging(UCListItem *item)
{
    // set object name for testing purposes
    listItem->setObjectName("DraggedListItem");
    // set this item as the dragged one
    isDraggedItem = true;
    originalItem = item;
    UCListItemPrivate::get(originalItem)->dragHandler->setDragging(true);
    // initialize style and turn panels on
    listItem->setX(item->x());
    listItem->setY(item->y());
    listItem->setZ(2);
    listItem->setVisible(true);

    // if we have animation, connect to it
    repositionAnimation = UCListItemPrivate::get(listItem)->styleItem->m_dragRepositionAnimation;
    if (repositionAnimation) {
        // complete any previous animation
        repositionAnimation->complete();
        // setup from-part of the animation, so we know where to reposition in case the original item is deleted
        connect(repositionAnimation, &QQuickPropertyAnimation::stopped,
                this, &UCDragHandler::repositionDraggedItem,
                Qt::DirectConnection);
        // make sure we have the 'y' property animated
        if (repositionAnimation->property() != ("y")) {
            QString properties = repositionAnimation->properties();
            if (properties.contains("y")) {
                properties = properties.isEmpty() ? "y" : "y," + properties;
            }
            repositionAnimation->setProperties(properties);
        }
        repositionAnimation->setTo(originalItem->y());
        repositionAnimation->setTargetObject(listItem);
    }
}

void UCDragHandler::drop()
{
    if (repositionAnimation) {
        repositionAnimation->setFrom(listItem->y());
        repositionAnimation->start();
    } else {
        repositionDraggedItem();
    }
}

void UCDragHandler::update(UCListItem *newItem)
{
    // update only if the original one disappeared
    if (repositionAnimation && repositionAnimation->isRunning() && repositionAnimation->target() == this && isDraggedItem) {
        repositionAnimation->setTo(newItem->y());
    }
}

void UCDragHandler::setDragging(bool value)
{
    if (dragging == value) {
        return;
    }
    dragging = value;
    if (!isDraggedItem) {
        if (dragging) {
            visibleProperty = new PropertyChange(listItem, "visible");
            PropertyChange::setValue(visibleProperty, false);
        } else {
            delete visibleProperty;
            visibleProperty = 0;
        }
    }
    Q_EMIT listItem->draggingChanged();
}
