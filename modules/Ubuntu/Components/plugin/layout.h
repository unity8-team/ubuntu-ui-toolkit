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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QQuickItem>

class LayoutPrivate;
class LayoutAttachedPrivate;
class LayoutAttached : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QQmlListProperty<QString> items READ items DESIGNABLE false)
    Q_PROPERTY(QString item READ item WRITE setItem NOTIFY itemChanged)

public:
    explicit LayoutAttached(QObject *parent = 0);

//    QQmlListProperty<QString> items();
    QString item() const;
    void setItem(const QString&);

Q_SIGNALS:
    void itemChanged();

private:
    friend class LayoutPrivate;
    friend class LayoutAttachedPrivate;
//    QList<QString*> m_items;
    QString m_item;
};



class QQmlBinding;
class LayoutManager;
class Layout : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QQmlBinding *when READ when WRITE setWhen)
    Q_PROPERTY(QQmlListProperty<QQuickItem> items READ items)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    explicit Layout(QObject *parent = 0);
    virtual ~Layout();

    QString name() const;
    void setName(const QString &);
    bool isNamed() const;

    /* 'when' is a QQmlBinding to limit state changes oscillation
     * due to the unpredictable order of evaluation of bound expressions*/
    bool isWhenKnown() const;
    QQmlBinding *when() const;
    void setWhen(QQmlBinding *);

    QQmlListProperty<QQuickItem> items();

    LayoutManager* layoutManager() const;
    void setLayoutManager(LayoutManager *);
    
    static LayoutAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void completed();

private:
    friend class LayoutPrivate;
    friend class LayoutManagerPrivate;
    LayoutPrivate* d;
    QList<QQuickItem*> m_items;
    Q_DISABLE_COPY(Layout)
};

QML_DECLARE_TYPEINFO(Layout, QML_HAS_ATTACHED_PROPERTIES)

#endif // LAYOUT_H
