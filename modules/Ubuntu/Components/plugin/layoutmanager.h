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

#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include <QObject>
#include <qqml.h>
#include <QQuickItem>

class LayoutManager;
class LayoutManagerAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString itemName READ itemName WRITE setItemName NOTIFY itemNameChanged)

public:
    explicit LayoutManagerAttached(QObject *parent = 0);

    QString itemName() const;
    void setItemName(const QString&);

Q_SIGNALS:
    void itemNameChanged();

private:
    QString m_itemName;
};


class Layout;
class LayoutManagerPrivate;
class LayoutManager : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString layout READ layout NOTIFY layoutChanged)
    Q_PROPERTY(QQmlListProperty<Layout> layouts READ layouts DESIGNABLE false)

public:
    explicit LayoutManager(QQuickItem *parent = 0);

    QString layout() const;
    void setLayout(const QString&);
    QQmlListProperty<Layout> layouts();

    static LayoutManagerAttached *qmlAttachedProperties(QObject *);
    
Q_SIGNALS:
    void layoutChanged();
    
private:
    friend class LayoutManagerPrivate;
    friend class Layout;
    bool updateAutoState();
    LayoutManagerPrivate *d;
};

QML_DECLARE_TYPEINFO(LayoutManager, QML_HAS_ATTACHED_PROPERTIES)
#endif // LAYOUTMANAGER_H
