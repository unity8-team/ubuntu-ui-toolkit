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

#ifndef UCPOPUP_H
#define UCPOPUP_H

#include <QtCore/QObject>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlListProperty>
#include "ucthemingextension.h"

class UCPopupPosition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment align READ align WRITE setAlign NOTIFY alignChanged FINAL)
    Q_PROPERTY(QPointF distanceFromCaller READ distanceFromCaller WRITE setDistanceFromCaller NOTIFY distanceFromCallerChanged FINAL)
    Q_PROPERTY(qreal pointerOffset READ pointerOffset WRITE setPointerOffset NOTIFY pointerOffsetChanged FINAL)
public:
    explicit UCPopupPosition(QObject *parent = 0);

    Qt::Alignment align() const;
    void setAlign(Qt::Alignment align);
    QPointF distanceFromCaller() const;
    void setDistanceFromCaller(const QPointF &distance);
    qreal pointerOffset() const;
    void setPointerOffset(qreal offset);


Q_SIGNALS:
    void alignChanged();
    void distanceFromCallerChanged();
    void pointerOffsetChanged();

private:
    QPointF m_distanceFromCaller;
    qreal m_pointerOffset = -1;
    Qt::Alignment m_align = 0;
};

class UCPopupPositioning : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UCPopupPosition> positions READ positions)
public:
    explicit UCPopupPositioning(QObject *parent = 0);

    QList<UCPopupPosition*> positionsList();
    QQmlListProperty<UCPopupPosition> positions();

private:
    QList<UCPopupPosition*> m_positions;
};

// FIXME: when we move on Qt 5.5, make sure we derive from QQuickWindowQmlImpl!
class UCPopup : public QQuickWindow//, public UCThemingExtension
{
    Q_OBJECT
//    Q_INTERFACES(UCThemingExtension)
//    Q_PROPERTY(UCTheme theme READ getTheme WRITE setTheme NOTIFY themeChanged FINAL)
    Q_PROPERTY(QString styleName READ styleName WRITE setStyleName NOTIFY styleNameChanged FINAL)
    Q_PROPERTY(UCPopupPositioning *positioning READ positioning WRITE setPositioning NOTIFY positioningChanged FINAL)
    Q_PROPERTY(QQuickItem *caller READ caller WRITE setCaller NOTIFY callerChanged FINAL)
    Q_PROPERTY(bool hasPointer READ hasPointer WRITE setHasPointer NOTIFY hasPointerChanged FINAL)
public:
    UCPopup(QWindow *parent = 0);
    explicit UCPopup(QQuickRenderControl *renderControl);

    QString styleName() const;
    void setStyleName(const QString &name);
    UCPopupPositioning *positioning() const;
    void setPositioning(UCPopupPositioning *positioning);
    QQuickItem *caller() const;
    void setCaller(QQuickItem *caller);
    bool hasPointer() const;
    void setHasPointer(bool hasPointer);

protected:
//    void preThemeChanged() override {}
//    void postThemeChanged() override {}

public Q_SLOTS:
    void open();
    void close();

Q_SIGNALS:
    void themeChanged();
    void styleNameChanged();
    void positioningChanged();
    void callerChanged();
    void hasPointerChanged();

    void aboutToOpen();
    void opened();
    void aboutToClose();
    void closed();

private:
    QString m_styleName;
    UCPopupPositioning *m_positioning = nullptr;
    QQuickItem *m_caller = nullptr;
    bool m_hasPointer = true;
};

#endif // UCPOPUP_H
