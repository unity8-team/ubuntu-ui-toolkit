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

#ifndef PROPERTYCHANGE_P_H
#define PROPERTYCHANGE_P_H

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtQml/QQmlProperty>

class QQmlAbstractBinding;
class QQuickPropertyAnimation;
class PropertyChange : public QObject
{
    Q_OBJECT
public:
    PropertyChange(QObject *item, const char *property, QObject *parent = 0);
    ~PropertyChange();

    static void setAnimation(PropertyChange *change, QQuickPropertyAnimation *animation);
    static void setValue(PropertyChange* change, const QVariant &value);
    static void restore(PropertyChange* change);
    static QVariant originalValue(PropertyChange *change);

Q_SIGNALS:
    void restoreCompleted();

private Q_SLOTS:
    void completeRestore();

private:
    bool m_backedUp;
    QQmlProperty qmlProperty;
    QQuickPropertyAnimation *animation;
    QPair<QQmlAbstractBinding*, QVariant> backup;
};

#endif // PROPERTYCHANGE_P_H
