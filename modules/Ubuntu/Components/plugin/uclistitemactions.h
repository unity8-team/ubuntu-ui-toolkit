/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef UCLISTITEMACTIONS_H
#define UCLISTITEMACTIONS_H

#include <QtCore/QObject>
#include "uclistitem_p.h"

class UCListItemActions;
class UCListItemActionsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCListItemActions *list MEMBER m_listItemActions NOTIFY listChanged)
    Q_PROPERTY(qreal offsetVisible READ offsetVisible NOTIFY offsetVisibleChanged)
public:
    UCListItemActionsAttached(QObject *parent = 0);
    ~UCListItemActionsAttached();
    void setList(UCListItemActions *list);
    qreal offsetVisible();

Q_SIGNALS:
    void listChanged();
    void offsetVisibleChanged();

private:
    UCListItemActions *m_listItemActions;
};

class QQmlComponent;
class UCAction;
class UCListItemActionsPrivate;
class UCListItemActions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QQmlComponent *customPanel READ customPanel WRITE setCustomPanel NOTIFY customPanelChanged)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions CONSTANT)
    Q_PROPERTY(QQuickItem *panelItem READ panelItem NOTIFY panelItemChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(UCListItem *connectedItem READ connectedItem NOTIFY connectedItemChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
    Q_ENUMS(Status)
public:
    enum Status {
        Disconnected = 0,
        Leading,
        Trailing
    };

    explicit UCListItemActions(QObject *parent = 0);
    ~UCListItemActions();

    static UCListItemActionsAttached *qmlAttachedProperties(QObject *owner);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);
    QQmlComponent *customPanel() const;
    void setCustomPanel(QQmlComponent *panel);
    QQmlListProperty<UCAction> actions();
    QQuickItem *panelItem() const;
    Status status() const;
    UCListItem *connectedItem() const;
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QColor foregroundColor() const;
    void setForegroundColor(const QColor &color);
    QQmlListProperty<QObject> data();

Q_SIGNALS:
    void delegateChanged();
    void customPanelChanged();
    void panelItemChanged();
    void statusChanged();
    void connectedItemChanged();
    void backgroundColorChanged();
    void foregroundColorChanged();

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCListItemActions)
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelDrag())
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelWidth())
};

QML_DECLARE_TYPEINFO(UCListItemActions, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCLISTITEMACTIONS_H
