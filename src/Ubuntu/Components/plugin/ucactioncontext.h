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

#ifndef UCACTIONCONTEXT_H
#define UCACTIONCONTEXT_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QSet>
#include <QtQml>

class UCAction;
class UCActionContext : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool overlay READ isOverlay WRITE setOverlay NOTIFY overlayChanged FINAL REVISION 1)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionContext(QObject *parent = 0);
    ~UCActionContext();

    void classBegin(){}
    void componentComplete();
    void clear();
    void markActionsPublished(bool mark);

    QQmlListProperty<UCAction> actions();

    inline bool isActive()
    {
        return m_active;
    }
    void setActive(bool active);
    inline bool isOverlay()
    {
        return m_overlay;
    }
    void setOverlay(bool overlay);

Q_SIGNALS:
    void activeChanged(bool);
    Q_REVISION(1) void overlayChanged(bool);

public Q_SLOTS:
    void addAction(UCAction *action);
    void removeAction(UCAction *action);

private:
    QSet<UCAction*> m_actions;
    bool m_active:1;
    bool m_overlay:1;
    friend class UCActionManager;

    static void append(QQmlListProperty<UCAction> *list, UCAction *action);
    static void clear(QQmlListProperty<UCAction> *list);
    static int count(QQmlListProperty<UCAction> *list);
};

QML_DECLARE_TYPE(UCActionContext)

#endif // UCACTIONCONTEXT_H
