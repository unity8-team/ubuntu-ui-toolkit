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
#include <private/qqmlglobal_p.h>

#include <QDebug>

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

/*!
 * \qmltype MouseEvent
 * \instantiates UCMouseEvent
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief Mouse event extending \l{http://qt-project.org/doc/qt-5.1/qtquick/qml-qtquick2-mouseevent.html}{QML MouseEvent}.
 */

/*!
  * \qmlproperty bool MouseEvent::inside
  * The property specifies whether the mouse position is inside the MouseFilter
  * area or not.
  */

/*!
  * \qmlproperty bool MouseEvent::overInputPanel
  * The proeprty specifies whether the mouse event happened over the input panel
  * or not. The property has meaning only devices where input panel is available.
  */

/*!
 * \qmltype MouseFilter
 * \instantiates UCMouseFilter
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief Filters the mouse events occured inside and outside the covered area.
 *
 * MouseFilter component signals the mouse events occurring around the covered area.
 * It combines MouseArea and InverseMouseArea functionality with the exception that
 * by default does not consume the mouse events (like in case of MouseArea), just
 * signals those. This kind of functionality is useful when additional functionality
 * is required over mouse grabber components such as Flickable, QML views, or text
 * input QML elements. A MouseArea placed above those components will either capture
 * all mouse events or, if mouse.acepted is set to false, the MouseArea will only get
 * the pressed signal, all the others being consumed by the underlying component.
 *
 * MouseFilter filters the events received from the application, so stays above all
 * components in the event queue. When more MouseFilter components are instantiated,
 * the order they receive the events depend on the window stacking order of those
 * (not the z-order!). You can set to filter only events occurring outside of the
 * area or inside the area separately, or both. See \l filter for details.
 *
 * One behaviour which differs from MouseArea and InverseMouseArea is that in all
 * MouseFilter signals all \b mouse events are not accepted by default. In this way
 * the events will be passed to the descendants and siblings. This means that the
 * filter's default functionality is to pass thruought all mouse events. However
 * MouseFilter can suppress these events. This can be achieved by setting mouse.accepted
 * in \l pressed signal to \a true. Event suppression can be turned on for inside or
 * outside presses, as well as for both. Suppressing pressed() signal from the
 * descendants means those will not be able to generate all the other signals
 * dependant on pressed.
 *
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Rectangle {
 *     width: 100; height: 100
 *     color: "green"
 *     MouseArea {
 *         anchors.fill: parent
 *         onPressed: print("the control does not reach this")
 *         onClicked: print("this should not be printed")
 *     }
 *     Rectangle {
 *         width: 50; height: 50
 *         color: "blue"
 *         anchors.centerIn: parent
 *         InverseMouseArea {
 *             anchors.fill: parent
 *             onPressed: print("this will not be printed")
 *         }
 *         MouseFilter {
 *             anchors.fill: parent
 *             onPressed: {
 *                 if (!mouse.inside) {
 *                     // capture outside presses, basically turns off InverseMouseArea
 *                     mouse.accepted = true;
 *                 }
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * In order to suppress all events simply set mouse.accepted to false for all
 * types of mouse presses.
 *
 * Also composed events like \l clicked and \l doubleClicked can be suppressed in
 * the same way. Note that these can be suppressed separately, not like in MouseArea
 * or InverseMouseArea where you have to set propagateComposedEvents to false, which
 * will suppress all composed events. In MouseFilter these composed events can be
 * suppressed separately. Also note that suppressing \l doubleClicked alone will
 * still propagate the clicked events to the children and siblings.
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Rectangle {
 *     width: 100; height: 100
 *     color: "green"
 *     MouseArea {
 *         anchors.fill: parent
 *         onClicked: print("clicked on the MouseArea")
 *         onDoubleClicked: print("the control does not reach this")
 *     }
 *     Rectangle {
 *         width: 50; height: 50
 *         color: "blue"
 *         anchors.centerIn: parent
 *         InverseMouseArea {
 *             anchors.fill: parent
 *             onDoubleClicked: print("the control does not reach this")
 *         }
 *         MouseFilter {
 *             anchors.fill: parent
 *             onDoubleClicked: {
 *                 // capture all doubleClicks
 *                 mouse.accepted = true;
 *             }
 *         }
 *     }
 * }
 * \endqml
 */

/*!
 * \qmlsignal MouseFilter::pressed(MouseEvent *mouse)
 * The signal is emitted whenever the mouse is pressed inside or outside the filtered
 * area. The \a mouse parameter specifies the coordinates relative to the MouseFilter
 * area (negative values for \a x and \a y properties mean the mouse position is
 * in the left-top area). The \a inside property tells whether the press occurred
 * inside or outside of the MouseFilter area. If input panel is opened (i.e. due
 * to some text input being focus), mouse presses occurring in that area are
 * reflected in \a overInput property of the MouseEvent.
 */

/*!
 * \qmlsignal MouseFilter::positionChanged(MouseEvent *mouse)
 * The signal is emitted whenever the mouse cursor changes its position.
 */

/*!
 * \qmlsignal MouseFilter::released(MouseEvent *mouse)
 * Signal emitted when the mouse button is released.
 */

/*!
 * \qmlsignal MouseFilter::clicked(MouseEvent *mouse)
 * Signal emitted when there is a click event happening on the same side of the
 * MouseFilter area (inside or outside). The signal is suppressed if the doubleClicked
 * signal is handled (connected).
 */

/*!
 * \qmlsignal MouseFilter::pressAndHold(MouseEvent *mouse)
 * Signal emtted when the same mouse button is pressed for a certain period of time
 * specified in \l pressAndHoldDelay property.
 */

/*!
 * \qmlsignal MouseFilter::doubleClicked(MouseEvent *mouse)
 * Signal emitted on double mouse click on the same mouse button. The signal can
 * be suppreessed by setting the mouse.accepted to true.
 */

/*!
 * \qmlsignal MouseFilter::entered()
 * Signal emitted when mouse point enters the MouseFilter area, or before the
 * \l pressed signal is emitted.
 */

/*!
 * \qmlsignal MouseFilter::exited()
 * Signal emitted when mous epointer leaves the MouseFilter area. Also emitted
 * after the \l released - \l clicked, as well as after \l doubleClicked -
 * \l released signal sequences.
 */

UCMouseFilter::UCMouseFilter(QQuickItem *parent)
    : QQuickItem(parent)
    , m_pressedInside(Qt::NoButton)
    , m_pressedOutside(Qt::NoButton)
    , m_pressAndHoldDelay(DefaultPressAndHoldDelay)
    , m_filter(MouseInside | MouseOutside)
    , m_lastPosType(Nowhere)
    , m_lastPosOverOsk(false)
    , m_moved(false)
    , m_longPress(false)
    , m_hovered(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);
    QGuiApplication::instance()->installEventFilter(this);
}

/*!
 * \qmlproperty Qt.Buttons MouseFilter::acceptedButtons
 * The property folds the accepted buttons. The default value is Qt.LeftButton.
 */
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

/*!
 * \qmlproperty int MouseFilter::pressAndHoldDelay
 * Specifies the timeout in milliseconds for the pressAndHold() signal. The default
 * value is 800 milliseconds.
 */
int UCMouseFilter::pressAndHoldDelay() const
{
    return m_pressAndHoldDelay;
}
void UCMouseFilter::setPressAndHoldDelay(int delay)
{
    if (m_pressAndHoldDelay != delay) {
        m_pressAndHoldDelay = delay;
        Q_EMIT pressAndHoldDelayChanged();
    }
}

/*!
 * \qmlproperty Filter MouseFilter::filter
 * The property specifies what type of events to filter. These can be one, or
 * the combination of the following flags:
 * \list
 *  \li \a MouseFilter::MouseInside - filter mouse events from inside the covered area.
 *  \li \a MouseFilter::MouseOutside - filter mouse events from outside the covered area.
 * \endlist
 * The default behavior is set to filter both inside and outside events.
 */
UCMouseFilter::Filter UCMouseFilter::filter() const
{
    return m_filter;
}
void UCMouseFilter::setFilter(Filter filter)
{
    if (m_filter != filter) {
        m_filter = filter;
        Q_EMIT filterChanged();
    }
}

// sheck whether the mouse point is in OSK area
bool UCMouseFilter::pointInOSK(QPointF &point)
{
    QPointF scenePos = mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    return oskRect.contains(scenePos);
}

/*
 * Translate mouse event positions to component's local coordinates.
 */
QMouseEvent *UCMouseFilter::mapMouseToArea(QObject *target, QMouseEvent *event)
{
    QMouseEvent *result = event;
    if (target != this) {
        // translate coordinates to local
        result = new QMouseEvent(event->type(),
                               mapFromScene(event->windowPos()),
                               event->windowPos(),
                               event->screenPos(),
                               event->button(), event->buttons(), event->modifiers());
        // turn accepted off by default
        result->setAccepted(false);
    }
    return result;
}

bool UCMouseFilter::eventFilter(QObject *object, QEvent *event)
{
    if (!isEnabled()) {
        return false;
    }
    if ((object != this) && isMouseEvent(event->type())) {
        QMouseEvent *mappedEvent = mapMouseToArea(object, static_cast<QMouseEvent*>(event));
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        {
            // sort out whether the press is in the area, and only forward it if we have a press
            // outside the area, otherwise inside presses will be handled twice
            if (!contains(mappedEvent->localPos())) {
                mousePressEvent(mappedEvent);
            }
        } break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(mappedEvent);
            break;
        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent(mappedEvent);
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(mappedEvent);
            break;
        default:
            break;
        }
        if (mappedEvent != event) {
            event->setAccepted(mappedEvent->isAccepted());
            delete mappedEvent;
        }
        if (event->isAccepted()) {
            // consume event
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}

void UCMouseFilter::mousePressEvent(QMouseEvent *event)
{
    m_moved = false;
    if (isEnabled() || (event->button() & acceptedMouseButtons())) {
        // check the event type
        saveEvent(event);
        if (m_filter & m_lastPosType) {
            if (m_lastPosType == MouseInside) {
                m_pressedInside |= m_lastButton;
            } else {
                m_pressedOutside |= m_lastButton;
            }

            m_longPress = false;
            m_doubleClicked = false;
            setHovered(m_lastPosType);
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             (m_lastPosType == MouseInside), m_lastPosOverOsk, false, m_longPress);
            Q_EMIT pressed(&mev);
            event->setAccepted(mev.isAccepted());

            // start long press timer
            m_pressAndHoldTimer.start(m_pressAndHoldDelay, this);
            return;
        }
    }
    QQuickItem::mousePressEvent(event);
}

void UCMouseFilter::mouseMoveEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        if (m_filter & m_lastPosType) {
            setHovered(m_lastPosType);
            m_moved = true;
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             (m_lastPosType == MouseInside), m_lastPosOverOsk, false, m_longPress);
            Q_EMIT positionChanged(&mev);
            event->setAccepted(mev.isAccepted());
            return;
        }
    }
    QQuickItem::mouseMoveEvent(event);
}

void UCMouseFilter::mouseReleaseEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        if (m_filter & m_lastPosType) {
            // stop long press timer event
            m_pressAndHoldTimer.stop();
            bool isClicked = ((m_lastPosType == MouseInside) ? (m_pressedInside & m_lastButton) : (m_pressedOutside & m_lastButton))
                    && !m_longPress && !m_doubleClicked;
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             (m_lastPosType == MouseInside), m_lastPosOverOsk, isClicked, m_longPress);
            Q_EMIT released(&mev);
            // remove button from both inside and outside press
            m_pressedInside &= ~m_lastButton;
            m_pressedOutside &= ~m_lastButton;
            if (isClicked) {
                // emit clicked
                Q_EMIT clicked(&mev);
            }
            event->setAccepted(mev.isAccepted());

            if (!m_pressedInside && !acceptHoverEvents()) {
                setHovered(MouseOutside);
            }
            return;
        }
    }
    QQuickItem::mouseReleaseEvent(event);
}

void UCMouseFilter::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (isEnabled() && (m_pressedInside || m_pressedOutside)) {
        saveEvent(event);
        if (m_filter & m_lastPosType) {
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             (m_lastPosType == MouseInside), m_lastPosOverOsk, true, false);
            Q_EMIT doubleClicked(&mev);
            // if double click connected, suppress release() and click() signals
            if (isDoubleClickConnected()) {
                m_doubleClicked = true;
            }
            event->setAccepted(mev.isAccepted());
            return;
        }
    }
    QQuickItem::mouseDoubleClickEvent(event);
}

void UCMouseFilter::timerEvent(QTimerEvent *event)
{
    if (isEnabled() && event->timerId() == m_pressAndHoldTimer.timerId()) {
        m_pressAndHoldTimer.stop();
        if ((m_filter & m_lastPosType) && (m_pressedInside || m_pressedOutside)) {
            m_longPress = true;
            UCMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             (m_lastPosType == MouseInside), m_lastPosOverOsk, false, true);
            Q_EMIT pressAndHold(&mev);
            // if the event wasn't handled, allow click
            if (!mev.isAccepted()) {
                m_longPress = false;
            }
            event->setAccepted(mev.isAccepted());
        }
    } else {
        QQuickItem::timerEvent(event);
    }
}

UCMouseFilter::FilterType UCMouseFilter::eventType(QMouseEvent *event)
{
    return contains(event->localPos()) ? MouseInside : MouseOutside;
}

bool UCMouseFilter::isMouseEvent(QEvent::Type type)
{
    return (type == QEvent::MouseButtonPress) || (type == QEvent::MouseButtonRelease)
            || (type == QEvent::MouseButtonDblClick) || (type == QEvent::MouseMove);
}

void UCMouseFilter::saveEvent(QMouseEvent *event)
{
    m_lastPos = event->localPos();
    m_lastScenePos = event->windowPos();
    m_lastButton = event->button();
    m_lastButtons = event->buttons();
    m_lastModifiers = event->modifiers();
    m_lastPosType = eventType(event);
    m_lastPosOverOsk = pointInOSK(m_lastPos);
}

void UCMouseFilter::setHovered(FilterType type)
{
    bool value = (type == MouseInside);
    if (m_hovered != value) {
        m_hovered = value;
        if (m_hovered) {
            Q_EMIT entered();
        } else {
            Q_EMIT exited();
        }
    }
}

bool UCMouseFilter::isDoubleClickConnected()
{
    IS_SIGNAL_CONNECTED(this, UCMouseFilter, doubleClicked, (UCMouseEvent*));
}
