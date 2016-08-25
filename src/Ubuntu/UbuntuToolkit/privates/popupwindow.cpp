/*
 * Copyright 2016 Canonical Ltd.
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

#include "popupwindow_p.h"

#include <QQuickItem>
#include <QGuiApplication>
#include <QtQuick/QQuickRenderControl>
#include <qpa/qwindowsysteminterface.h>

PopupWindow::PopupWindow(QWindow *parentWindow)
    : m_mouseMoved(false)
    , m_dismissed(true)
    , m_needsActivatedEvent(true)
{
    setFlags(Qt::Popup);
    qCDebug(ucMenu, "PopupWindow::PopupWindow(this=%p, parentWindow=%p)", this, parentWindow);

    connect(contentItem(), &QQuickItem::childrenRectChanged, this, &PopupWindow::updateGeometry);

    // Popup of a popup will not get focus.
    connect(this, &QQuickWindow::activeChanged, this, [this]() {
        QWindow *focus = QGuiApplication::focusWindow();
        if (isActive() && this == focus && contentItem()) {
            contentItem()->setFocus(true);
        }
    });
}

PopupWindow::~PopupWindow()
{
    m_positions.clear();
    qCDebug(ucMenu, "PopupWindow::~PopupWindow(this=%p)", this);
}

QQmlListProperty<PopupPosition> PopupWindow::positions()
{
    return QQmlListProperty<PopupPosition>(this, nullptr,
                                           &PopupWindow::position_append,
                                           &PopupWindow::position_count,
                                           &PopupWindow::position_at,
                                           &PopupWindow::position_clear);
}

void PopupWindow::show()
{
    qCDebug(ucMenu, "PopupWindow::show(this=%p, parent=%p)", this, transientParent());

    if (!qobject_cast<PopupWindow*>(transientParent())) { // No need for parent menu windows
        if (QQuickWindow *w = qobject_cast<QQuickWindow *>(transientParent())) {
            if (QQuickItem *mg = w->mouseGrabberItem()) {
                mg->ungrabMouse();
            }
        }
    }

    QQuickWindow::show();
    updateGeometry();

    setMouseGrabEnabled(true); // Needs to be done after calling show()
    setKeyboardGrabEnabled(true);
}

void PopupWindow::hide()
{
    if (qobject_cast<PopupWindow*>(transientParent())) {
        transientParent()->setMouseGrabEnabled(true);
        transientParent()->setKeyboardGrabEnabled(true);
    }
    QQuickWindow::hide();
}

void PopupWindow::dismiss()
{
    qCDebug(ucMenu, "UCPopupWindow::dismiss(this=%p)", this);

    m_dismissed = true;
    Q_EMIT dismissed();

    close();
    this->deleteLater();
}

namespace {
    struct PopupPositional {
        int position;
        bool fits;
    };

    // if the new positional fits, add it before the first existing positional that doesnt fit.
    void addPosition(QList<PopupPositional> &list, const PopupPositional& positional) {
        QMutableListIterator<PopupPositional> iter(list);
        while(iter.hasNext()) {
            auto existing = iter.peekNext();
            if (positional.fits && !existing.fits) {
                iter.insert(positional);
                return;
            }
            iter.next();
        }
        list.append(positional);
    }
}

void PopupWindow::updateGeometry()
{
    QWindow* parent = transientParent();
    QScreen* screen = parent ? parent->screen() : nullptr;
    if (!screen) return;

    int newWidth = contentItem()->childrenRect().width();
    int newHeight = contentItem()->childrenRect().height();

    QRect parentRect(parent ? parent->x() : 0,
                     parent ? parent->y() : 0,
                     parent->width(),
                     parent->height());
    QPoint centerPoint = parentRect.center();

    QList<PopupPositional> priorityX;
    QList<PopupPositional> priorityY;
    if (m_positions.isEmpty()) {
        addPosition(priorityX, PopupPositional{ centerPoint.x() - newWidth / 2, true });
        addPosition(priorityY, PopupPositional{ centerPoint.y() - newHeight / 2, true });
    } else {
        // QPoint(0,0) default
        Q_FOREACH(PopupPosition* position, m_positions) {
            int x = parentRect.x() + position->distanceFromCaller().x();
            int y = parentRect.y() + position->distanceFromCaller().y();
            Qt::Alignment alignment = position->align();

            if (alignment & Qt::AlignLeft) {
                x -= newWidth;
                addPosition(priorityX, PopupPositional{ x, x >= screen->geometry().left() });
            } else if (alignment & Qt::AlignRight) {
                x += parentRect.width();
                addPosition(priorityX, PopupPositional{ x, x + newWidth <= screen->geometry().right() });
            } else if (alignment & Qt::AlignVCenter) {
                x = centerPoint.x() - newWidth / 2;
                addPosition(priorityX, PopupPositional{ x, true });
            } else {
                addPosition(priorityX, PopupPositional{ x, x + newWidth <= screen->geometry().right() });
            }

            if (alignment & Qt::AlignTop) {
                y -= newHeight;
                addPosition(priorityY, PopupPositional{ y, true });
            } else if (alignment & Qt::AlignBottom) {
                y += parentRect.height();
                addPosition(priorityY, PopupPositional{ y, true });
            } else if (alignment & Qt::AlignVCenter) {
                y = centerPoint.y() - newHeight / 2;
                addPosition(priorityY, PopupPositional{ y, true });
            } else {
                qDebug() << "ALIGN PARENT Y" << y << (y + newHeight <= screen->geometry().bottom());
                addPosition(priorityY, PopupPositional{ y, y + newHeight <= screen->geometry().bottom() });
            }
        }
    }

    setPosition(priorityX.first().position, priorityY.first().position);
    resize(newWidth, newHeight);
}

void PopupWindow::showEvent(QShowEvent *e)
{
    if (PopupWindow* mw = qobject_cast<PopupWindow*>(transientParent())) {
        connect(mw, &PopupWindow::dismissed, this, &PopupWindow::dismiss);
    }
    QQuickWindow::showEvent(e);
}

void PopupWindow::mousePressEvent(QMouseEvent *e)
{
    QRect rect = QRect(QPoint(), size());
    if (rect.contains(e->pos()))
        QQuickWindow::mousePressEvent(e);
    else
        forwardMouseEvent(e);
}

void PopupWindow::mouseReleaseEvent(QMouseEvent *e)
{
    QRect rect = QRect(QPoint(), size());
    if (rect.contains(e->pos())) {
        // drag moving overv the menu. Count the release as a press.
//        if (m_mouseMoved) {
//            QMouseEvent pe = QMouseEvent(QEvent::MouseButtonPress, e->pos(), e->button(), e->buttons(), e->modifiers());
//            QQuickWindow::mousePressEvent(&pe);
//            if (!m_dismissed) {
                QQuickWindow::mouseReleaseEvent(e);
//            }
//        }
        // count an initial release as a move.
        m_mouseMoved = true;
    } else {
        forwardMouseEvent(e);
    }
}

void PopupWindow::mouseMoveEvent(QMouseEvent *e)
{
    QRect rect = QRect(QPoint(), size());

    m_mouseMoved = true;
    if (rect.contains(e->pos()))
        QQuickWindow::mouseMoveEvent(e);
    else
        forwardMouseEvent(e);
}

void PopupWindow::keyPressEvent(QKeyEvent *e)
{
    QQuickWindow::keyPressEvent(e);

    if (!e->isAccepted()) {
        forwardKeyEvent(e);
    }
}

void PopupWindow::hideEvent(QHideEvent *e)
{
    if (QWindow *tp = !m_needsActivatedEvent ? transientParent() : 0) {
        m_needsActivatedEvent = true;
        QWindowSystemInterface::handleWindowActivated(tp);
    }

    QQuickWindow::hideEvent(e);
}

void PopupWindow::forwardKeyEvent(QKeyEvent *e)
{
    bool forward = qobject_cast<PopupWindow*>(transientParent());
    if (forward && e->key() != Qt::Key_Left &&
                   e->key() != Qt::Key_Right &&
                   e->key() != Qt::Key_Escape) {
        forward = false;
    }

    // send events to parent menus.
    if (forward) {
        QKeyEvent pe(e->type(), e->key(), e->modifiers(), e->text(), e->isAutoRepeat(), e->count());
        QGuiApplication::sendEvent(transientParent(), &pe);
    }
}

void PopupWindow::forwardMouseEvent(QMouseEvent *e)
{
    bool forward = transientParent();

    if (forward && !qobject_cast<PopupWindow*>(transientParent())) {
        // check that the mouse has moved in this window. otherwise it may just be the
        // release from activation.
        if ((m_mouseMoved && e->type() == QEvent::MouseButtonRelease) || e->type() == QEvent::MouseButtonPress) {
            dismiss(); // clicked outside popup.
            forward = false;
        }
    }

    if (forward) {
        QPoint parentPos = transientParent()->mapFromGlobal(mapToGlobal(e->pos()));
        QMouseEvent pe(e->type(), parentPos, e->button(), e->buttons(), e->modifiers());
        QGuiApplication::sendEvent(transientParent(), &pe);
    }
}

void PopupWindow::position_append(QQmlListProperty<PopupPosition> *prop, PopupPosition *o)
{
    auto window = qobject_cast<PopupWindow*>(prop->object);
    connect(o, &PopupPosition::alignChanged, window, &PopupWindow::updateGeometry);
    connect(o, &PopupPosition::distanceFromCallerChanged, window, &PopupWindow::updateGeometry);
    window->m_positions << o;
}

int PopupWindow::position_count(QQmlListProperty<PopupPosition> *prop)
{
    auto window = qobject_cast<PopupWindow*>(prop->object);
    return window->m_positions.count();
}

PopupPosition *PopupWindow::position_at(QQmlListProperty<PopupPosition> *prop, int index)
{
    auto window = qobject_cast<PopupWindow*>(prop->object);

    if (index < 0 || index >= window->m_positions.count()) return nullptr;
    return window->m_positions.at(index);
}

void PopupWindow::position_clear(QQmlListProperty<PopupPosition> *prop)
{
    auto window = qobject_cast<PopupWindow*>(prop->object);
    window->m_positions.clear();
}

PopupPosition::PopupPosition(QObject *parent)
    : QObject(parent)
    , m_align(0)
{
}

Qt::Alignment PopupPosition::align() const
{
    return m_align;
}

void PopupPosition::setAlign(Qt::Alignment alignment)
{
    if (m_align == alignment) {
        return;
    }
    m_align = alignment;
    Q_EMIT alignChanged();
}

QPoint PopupPosition::distanceFromCaller() const
{
    return m_distanceFromCaller;
}

void PopupPosition::setDistanceFromCaller(const QPoint &distanceFromCaller)
{
    if (m_distanceFromCaller == distanceFromCaller) {
        return;
    }
    m_distanceFromCaller = distanceFromCaller;
    Q_EMIT distanceFromCallerChanged();
}
