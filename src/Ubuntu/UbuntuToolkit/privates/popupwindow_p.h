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

#ifndef UCPOPUPWINDOW_P_H
#define UCPOPUPWINDOW_P_H

#include <QQuickWindow>
#include <QPointer>
#include <QQmlParserStatus>

#include "menu_p.h"

class PopupPosition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment align READ align WRITE setAlign NOTIFY alignChanged)
    Q_PROPERTY(QPoint distanceFromCaller READ distanceFromCaller WRITE setDistanceFromCaller NOTIFY distanceFromCallerChanged)
public:
    PopupPosition(QObject *parent = 0);

    Qt::Alignment align() const;
    void setAlign(Qt::Alignment alignment);

    QPoint distanceFromCaller() const;
    void setDistanceFromCaller(const QPoint &point);

Q_SIGNALS:
    void alignChanged();
    void distanceFromCallerChanged();

private:
    Qt::Alignment m_align;
    QPoint m_distanceFromCaller;
};

class PopupWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<PopupPosition> positions READ positions NOTIFY positionsChanged)
public:
    PopupWindow(QWindow *parent = 0);
    ~PopupWindow();

    QQmlListProperty<PopupPosition> positions();

Q_SIGNALS:
    void positionsChanged();
    void dismissed();

public Q_SLOTS:
    void show();
    void hide();
    void dismiss();

private Q_SLOTS:
    void updateGeometry();

protected:
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;

private:
    void forwardKeyEvent(QKeyEvent *e);
    void forwardMouseEvent(QMouseEvent *e);

    static void position_append(QQmlListProperty<PopupPosition> *prop, PopupPosition *o);
    static int position_count(QQmlListProperty<PopupPosition> *prop);
    static PopupPosition *position_at(QQmlListProperty<PopupPosition> *prop, int index);
    static void position_clear(QQmlListProperty<PopupPosition> *prop);

    bool m_mouseMoved;
    bool m_dismissed;
    bool m_needsActivatedEvent;
    QList<PopupPosition*> m_positions;
};

#endif // UCPOPUPWINDOW_P_H
