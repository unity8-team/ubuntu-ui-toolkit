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
 */

#include "ucmousefilter.h"
#include <QtGui/QGuiApplication>

#include <QDebug>

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

UCMouseFilter::UCMouseFilter(QQuickItem *parent)
    : QQuickItem(parent)
    , m_pressedInside(Qt::NoButton)
    , m_pressedOutside(Qt::NoButton)
    , m_pressAndHoldDelay(DefaultPressAndHoldDelay)
    , m_lastPosInside(false)
    , m_lastPosOverOsk(false)
    , m_moved(false)
    , m_longPress(false)
    , m_hovered(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);
    QGuiApplication::instance()->installEventFilter(this);
}

Qt::MouseButtons UCMouseFilter::acceptedButtons() const
{
    return acceptedMouseButtons();
}
void UCMouseFilter::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (buttons != acceptedMouseButtons()) {
        setAcceptedMouseButtons(buttons);
        Q_EMIT acceptedButtonsChanged();
    }
}

int UCMouseFilter::pressAndHoldDelay() const
{
    return m_pressAndHoldDelay;
}
void UCMouseFilter::setPressAndHoldDelay(int delay)
{
    if (m_pressAndHoldDelay != delay) {
        m_pressAndHoldDelay = delay;
        Q_EMIT pressAndHoldChanged();
    }
}


bool UCMouseFilter::pointInOSK(QPointF &point)
{
    QPointF scenePos = mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    return oskRect.contains(scenePos);
}

/*
 * Translate mouse, wheel and hover event positions to component's local coordinates.
 */
QEvent *UCMouseFilter::mapEventToArea(QObject *target, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        if (target != this) {
            // translate coordinates to local
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            QMouseEvent *mev = new QMouseEvent(ev->type(),
                                   mapFromScene(ev->windowPos()),
                                   ev->windowPos(),
                                   ev->screenPos(),
                                   ev->button(), ev->buttons(), ev->modifiers());
            return mev;
        }
        break;
//    case QEvent::Wheel:
//        if (target != this) {
//            QWheelEvent *ev = static_cast<QWheelEvent*>(event);
//            QWheelEvent *wev = new QWheelEvent(mapFromScene(ev->globalPos()), ev->globalPos(),
//                                               ev->delta(), ev->buttons(), ev->modifiers(), ev->orientation());
//            return wev;
//        }
//        break;
//    case QEvent::HoverEnter:
//    case QEvent::HoverLeave:
//    case QEvent::HoverMove: {
//        QQuickItem *item = qobject_cast<QQuickItem*>(target);
//        if (item && item != this) {
//            QHoverEvent *ev = static_cast<QHoverEvent*>(event);
//            QPointF spos = item->mapToScene(ev->posF());
//            QPointF sopos = item->mapToScene(ev->oldPosF());
//            QHoverEvent *hev = new QHoverEvent(ev->type(),
//                                               mapFromScene(spos),
//                                               mapFromScene(sopos),
//                                               ev->modifiers());
//            return hev;
//        }
//        } break;
    default:
        break;
    }
    return event;
}

bool UCMouseFilter::eventFilter(QObject *object, QEvent *event)
{
    if (!isEnabled()) {
        return false;
    }
    if (object != this) {
        QEvent *mappedEvent = mapEventToArea(object, event);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *mev= static_cast<QMouseEvent*>(mappedEvent);
            // sort out whether the press is in the area, and only forward it if we have a press
            // outside the area
            if (!contains(mev->localPos())) {
                mousePressEvent(mev);
            }
        } break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
//        case QEvent::Wheel:
//            wheelEvent(static_cast<QWheelEvent*>(mappedEvent));
//            break;
//        case QEvent::HoverEnter:
//            hoverEnterEvent(static_cast<QHoverEvent*>(mappedEvent));
//            break;
//        case QEvent::HoverLeave:
//            hoverLeaveEvent(static_cast<QHoverEvent*>(mappedEvent));
//            break;
//        case QEvent::HoverMove:
//            hoverMoveEvent(static_cast<QHoverEvent*>(mappedEvent));
//            break;
        default:
            break;
        }
        if (mappedEvent != event) {
            event->setAccepted(mappedEvent->isAccepted());
            delete mappedEvent;
        }
    }
    return QObject::eventFilter(object, event);
}

void UCMouseFilter::mousePressEvent(QMouseEvent *event)
{
    m_moved = false;
    if (isEnabled() || (event->button() & acceptedMouseButtons())) {
        m_longPress = false;
        saveEvent(event);
        setHovered(m_lastPosInside);
        if (m_lastPosInside) {
            m_pressedInside |= m_lastButton;
        } else {
            m_pressedOutside |= m_lastButton;
        }
        UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                         m_lastPosInside, m_lastPosOverOsk, false, m_longPress);
        Q_EMIT pressed(&mev);

        // start long press timer
        m_pressAndHoldTimer.start(m_pressAndHoldDelay, this);
    }
    QQuickItem::mousePressEvent(event);
}

void UCMouseFilter::mouseMoveEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        setHovered(m_lastPosInside);
        m_moved = true;
        UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                         m_lastPosInside, m_lastPosOverOsk, false, m_longPress);
        Q_EMIT positionChanged(&mev);
    }
    QQuickItem::mouseMoveEvent(event);
}

void UCMouseFilter::mouseReleaseEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        bool isClicked = ((m_lastPosInside) ? (m_pressedInside & m_lastButton) : (m_pressedOutside & m_lastButton)) && !m_longPress;
        UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                         m_lastPosInside, m_lastPosOverOsk, isClicked, m_longPress);
        Q_EMIT released(&mev);
        if (m_lastPosInside) {
            m_pressedInside &= ~m_lastButton;
        } else {
            m_pressedOutside &= ~m_lastButton;
        }
        if (isClicked) {
            // emit clicked
            Q_EMIT clicked(&mev);
        }
        if (!m_pressedInside && !acceptHoverEvents()) {
            setHovered(false);
        }
    }
    QQuickItem::mouseReleaseEvent(event);
}

void UCMouseFilter::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                         m_lastPosInside, m_lastPosOverOsk, true, false);
        Q_EMIT doubleClicked(&mev);
    }
    QQuickItem::mouseDoubleClickEvent(event);
}

void UCMouseFilter::timerEvent(QTimerEvent *event)
{
    if (isEnabled() && event->timerId() == m_pressAndHoldTimer.timerId()) {
        m_pressAndHoldTimer.stop();
        if (m_pressedInside || m_pressedOutside) {
            m_longPress = true;
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             m_lastPosInside, m_lastPosOverOsk, false, true);
            Q_EMIT pressAndHold(&mev);
            // if the event wasn't handled, allow click
            if (!mev.isAccepted()) {
                m_longPress = false;
            }
        }
    }
}

void UCMouseFilter::saveEvent(QMouseEvent *event)
{
    m_lastPos = event->localPos();
    m_lastScenePos = event->windowPos();
    m_lastButton = event->button();
    m_lastButtons = event->buttons();
    m_lastModifiers = event->modifiers();
    m_lastPosInside = contains(m_lastPos);
    m_lastPosOverOsk = pointInOSK(m_lastPos);
}

void UCMouseFilter::setHovered(bool value)
{
    if (m_hovered != value) {
        m_hovered = value;
        if (m_hovered) {
            Q_EMIT entered();
        } else {
            Q_EMIT exited();
        }
    }
}
