
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include "propertychange_p.h"
#include "quickutils.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
//#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickanimation_p.h>

UCDragHandler::UCDragHandler(UCListItem *listItem)
    : UCHandlerBase(listItem)
    , dragging(false)
    , isFakeItem(false)
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
void UCDragHandler::initialize()
{
    if (!listItem->parentAttached) {
        return;
    }
    connect(listItem->parentAttached, &UCViewItemsAttached::dragModeChanged,
            this, &UCDragHandler::setupDragMode);
    if (listItem->isDraggable()) {
        setupDragMode();
        // is the current one the dragged source?
        UCViewItemsAttachedPrivate *pAttached = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
        if (listItem->index() == pAttached->dragFromIndex) {
            setDragging(true);
            if (pAttached->dragTempItem) {
                UCListItemPrivate *pListItem = UCListItemPrivate::get(pAttached->dragTempItem);
                if (!pListItem->dragHandler->originalItem) {
                    // this is the one that has been dragged by the temp item, so update
                    pListItem->dragHandler->originalItem = listItem->item();
                }
            }
        }
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
    }

    // update visuals
    listItem->update();
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
    listItem->item()->setVisible(false);
    listItem->item()->deleteLater();
}

// this method should only be called for the temporary ListItem used in dragging!
void UCDragHandler::startDragging(UCListItem *item)
{
    UCListItem *fakeItem = listItem->item();
    originalItem = item;
    // set this item as fake
    isFakeItem = true;
    UCListItemPrivate::get(originalItem)->dragHandler->setDragging(true);
    // initialize style and turn panels on
    listItem->initStyleItem();
    if (listItem->isSelectable()) {
        listItem->selectionHandler->setupSelection();
    }
    if (listItem->isDraggable()) {
        listItem->dragHandler->setupDragMode();
    }
    fakeItem->setX(item->x());
    fakeItem->setY(item->y());
    fakeItem->setZ(2);
    fakeItem->setWidth(item->width());
    fakeItem->setHeight(item->height());
    QColor color = item->color();
    if (color.alphaF() == 0.0) {
        color = QuickUtils::instance().rootItem(item)->property("backgroundColor").value<QColor>();
    }
    fakeItem->setColor(color);
    fakeItem->setObjectName("DraggedListItem");
    setDragging(true);

    // if we have animation, connect to it
    repositionAnimation = UCListItemPrivate::get(fakeItem)->styleItem->m_dragRepositionAnimation;
    if (repositionAnimation) {
        // setup from-part of the animation, so we know where to reposition in case the original item is deleted
        connect(repositionAnimation, &QQuickPropertyAnimation::stopped,
                this, &UCDragHandler::repositionDraggedItem,
                Qt::DirectConnection);
        // make sure we have the 'y' proeprty animated
        if (repositionAnimation->property() != ("y")) {
            QString properties = repositionAnimation->properties();
            if (properties.contains("y")) {
                properties = properties.isEmpty() ? "y" : "y," + properties;
            }
            repositionAnimation->setProperties(properties);
        }
        repositionAnimation->setTo(originalItem->y());
        repositionAnimation->setTargetObject(fakeItem);
    }
}

void UCDragHandler::stopDragging()
{
    if (repositionAnimation) {
        repositionAnimation->setFrom(listItem->item()->y());
        repositionAnimation->start();
    } else {
        repositionDraggedItem();
    }
}

void UCDragHandler::update(UCListItem *newItem)
{
    // update only if the original one disappeared
    if (repositionAnimation) {
        repositionAnimation->setTo(newItem->y());
    }
}

void UCDragHandler::setDragging(bool value)
{
    if (dragging == value) {
        return;
    }
    dragging = value;
    if (!isFakeItem) {
        if (dragging) {
            visibleProperty = new PropertyChange(listItem->item(), "visible");
            PropertyChange::setValue(visibleProperty, false);
        } else {
            delete visibleProperty;
            visibleProperty = 0;
        }
    }
    Q_EMIT listItem->item()->draggingChanged();
}
