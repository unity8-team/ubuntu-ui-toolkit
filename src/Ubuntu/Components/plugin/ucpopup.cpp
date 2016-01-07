/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucpopup.h"

UCPopupPosition::UCPopupPosition(QObject *parent)
    : QObject(parent)
{
}

Qt::Alignment UCPopupPosition::align() const
{
    return m_align;
}
void UCPopupPosition::setAlign(Qt::Alignment align)
{
    if (m_align == align) {
        return;
    }
    m_align = align;
    Q_EMIT alignChanged();
}

QPointF UCPopupPosition::distanceFromCaller() const
{
    return m_distanceFromCaller;
}
void UCPopupPosition::setDistanceFromCaller(const QPointF &distance)
{
    if (m_distanceFromCaller == distance) {
        return;
    }
    m_distanceFromCaller = distance;
    Q_EMIT distanceFromCallerChanged();
}

qreal UCPopupPosition::pointerOffset() const
{
    return m_pointerOffset;
}
void UCPopupPosition::setPointerOffset(qreal offset)
{
    if (m_pointerOffset == offset) {
        return;
    }
    m_pointerOffset = offset;
    Q_EMIT pointerOffsetChanged();
}


UCPopupPositioning::UCPopupPositioning(QObject *parent)
    : QObject(parent)
{
}

QList<UCPopupPosition*> UCPopupPositioning::positionsList()
{
    return m_positions;
}

QQmlListProperty<UCPopupPosition> UCPopupPositioning::positions()
{
    return QQmlListProperty<UCPopupPosition>(this, m_positions);
}


UCPopup::UCPopup(QWindow *parent)
    : QQuickWindow(parent)
{
}
UCPopup::UCPopup(QQuickRenderControl *renderControl)
    : QQuickWindow(renderControl)
{
}

QString UCPopup::styleName() const
{
    return m_styleName;
}
void UCPopup::setStyleName(const QString &name)
{
    if (m_styleName == name) {
        return;
    }
    m_styleName = name;
    Q_EMIT styleNameChanged();
}

UCPopupPositioning *UCPopup::positioning() const
{
    return m_positioning;
}
void UCPopup::setPositioning(UCPopupPositioning *positioning)
{
    if (m_positioning == positioning) {
        return;
    }
    m_positioning = positioning;
    Q_EMIT positioningChanged();
}

QQuickItem *UCPopup::caller() const
{
    return m_caller;
}
void UCPopup::setCaller(QQuickItem *caller)
{
    if (m_caller == caller) {
        return;
    }
    m_caller = caller;
    Q_EMIT callerChanged();
}

bool UCPopup::hasPointer() const
{
    return m_hasPointer;
}
void UCPopup::setHasPointer(bool hasPointer)
{
    if (m_hasPointer == hasPointer) {
        return;
    }
    m_hasPointer = hasPointer;
    Q_EMIT hasPointerChanged();
}

void UCPopup::open()
{
    setVisible(true);
}

void UCPopup::close()
{
    setVisible(false);
}

#include "moc_ucpopup.cpp"
