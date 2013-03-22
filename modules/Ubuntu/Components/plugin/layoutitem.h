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
 * Author: Gerry Boland <gerry.boland@canonical.com>
 */

#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QQuickItem>
#include "QtQuick/private/qquickanchors_p.h"
#include "QtQuick/private/qquickanchors_p_p.h"

class LayoutItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString itemName READ itemName WRITE setItemName NOTIFY itemNameChanged FINAL)

    Q_PROPERTY(QQuickItem *parent READ parentItem WRITE setParentItem NOTIFY parentChanged DESIGNABLE false FINAL)

    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged FINAL)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged FINAL)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY zChanged FINAL)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged RESET resetWidth FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged RESET resetHeight FINAL)

    Q_PROPERTY(QQuickAnchors * anchors READ anchors DESIGNABLE false CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine left READ left CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine right READ right CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine horizontalCenter READ horizontalCenter CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine top READ top CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine bottom READ bottom CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine verticalCenter READ verticalCenter CONSTANT FINAL)
    Q_PROPERTY(QQuickAnchorLine baseline READ baseline CONSTANT FINAL)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)

public:
    explicit LayoutItem(QObject *parent = 0);
    
    QString itemName() const;
    void setItemName(const QString&);

    QQuickItem *parentItem() const;
    void setParentItem(QQuickItem*);

    qreal x() const;
    qreal y() const;
    void setX(qreal);
    void setY(qreal);

    qreal width() const;
    void setWidth(qreal);
    void resetWidth();

    qreal height() const;
    void setHeight(qreal);
    void resetHeight();

    qreal z() const;
    void setZ(qreal);

    QQuickAnchors * anchors() const;
    QQuickAnchorLine left() const;
    QQuickAnchorLine right() const;
    QQuickAnchorLine top() const;
    QQuickAnchorLine bottom() const;
    QQuickAnchorLine horizontalCenter() const;
    QQuickAnchorLine verticalCenter() const;
    QQuickAnchorLine baseline() const;

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

Q_SIGNALS:
    void itemNameChanged();
    void parentChanged();
    void xChanged();
    void yChanged();
    void zChanged();
    void widthChanged();
    void heightChanged();
    void baselineOffsetChanged();

private:
    QQuickItem *d;
    QString m_itemName;
};

#endif // LAYOUTITEM_H
