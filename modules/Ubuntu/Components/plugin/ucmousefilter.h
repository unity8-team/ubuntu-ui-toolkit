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

#ifndef UCMOUSEFILTER_H
#define UCMOUSEFILTER_H

#include <QtQuick/QQuickItem>
#include <private/qquickmousearea_p.h>
#include <private/qquickevents_p_p.h>
#include <QtCore/qbasictimer.h>

class UCMouseEvent : public QQuickMouseEvent
{
    Q_OBJECT
    Q_PROPERTY(bool inside READ inside)
    Q_PROPERTY(bool overInputPanel READ overInputPanel)
public:
    UCMouseEvent(QPointF pos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers
                  , bool insidePress, bool overOsk, bool isClick, bool wasHeld)
        : QQuickMouseEvent(pos.x(), pos.y(), button, buttons, modifiers, isClick, wasHeld)
        , m_insidePress(insidePress)
        , m_overOsk(overOsk)
    {
        // contrary to the original class, do not accept events by default
        setAccepted(false);
    }

    bool inside() const
    {
        return m_insidePress;
    }
    bool overInputPanel() const
    {
        return m_overOsk;
    }
private:
    bool m_insidePress;
    bool m_overOsk;
};

class UCMouseFilter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(int pressAndHoldDelay READ pressAndHoldDelay WRITE setPressAndHoldDelay NOTIFY pressAndHoldDelayChanged)
    Q_PROPERTY(Filter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_ENUMS(FilterType)
    Q_FLAGS(Filter)
public:
    enum FilterType {
        Nowhere         = 0x00,
        MouseInside     = 0x01,
        MouseOutside    = 0x02
    };
    Q_DECLARE_FLAGS(Filter, FilterType)
    explicit UCMouseFilter(QQuickItem *parent = 0);

    Qt::MouseButtons acceptedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons buttons);

    int pressAndHoldDelay() const;
    void setPressAndHoldDelay(int delay);

    Filter filter() const;
    void setFilter(Filter filter);

Q_SIGNALS:
    void acceptedButtonsChanged();
    void pressAndHoldDelayChanged();
    void filterChanged();

    void pressed(UCMouseEvent *mouse);
    void positionChanged(UCMouseEvent *mouse);
    void released(UCMouseEvent *mouse);
    void clicked(UCMouseEvent *mouse);
    void pressAndHold(UCMouseEvent *mouse);
    void doubleClicked(UCMouseEvent *mouse);
    void entered();
    void exited();

public Q_SLOTS:

protected:
    bool pointInOSK(QPointF &point);
    QMouseEvent *mapMouseToArea(QObject *target, QMouseEvent *event);

    // overrides
    virtual bool eventFilter(QObject *, QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    QPointF m_lastPos;
    QPointF m_lastScenePos;
    QBasicTimer m_pressAndHoldTimer;
    Qt::MouseButton m_lastButton;
    Qt::MouseButtons m_lastButtons;
    Qt::KeyboardModifiers m_lastModifiers;
    Qt::MouseButtons m_pressedInside;
    Qt::MouseButtons m_pressedOutside;
    int m_pressAndHoldDelay;
    Filter m_filter;

    FilterType m_lastPosType;
    bool m_lastPosOverOsk:1;
    bool m_moved:1;
    bool m_longPress:1;
    bool m_hovered:1;
    bool m_doubleClicked:1;

    FilterType eventType(QMouseEvent *event);
    bool isMouseEvent(QEvent::Type type);
    void saveEvent(QMouseEvent *event);
    void setHovered(FilterType type);
    bool isDoubleClickConnected();
};

QML_DECLARE_TYPE(UCMouseEvent)
Q_DECLARE_OPERATORS_FOR_FLAGS(UCMouseFilter::Filter)

#endif // UCMOUSEFILTER_H
