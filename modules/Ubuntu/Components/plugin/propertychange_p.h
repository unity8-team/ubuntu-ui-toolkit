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
class PropertyChange
{
public:
    enum BackupType {
        ValueBackup,
        BindingBackup,
    };

    PropertyChange(QObject *item, const char *property, BackupType type = ValueBackup);
    ~PropertyChange();

    void setValue(const QVariant &value);
    void setValue(QQmlAbstractBinding *binding);
    void restore();
    QQmlProperty property() const;
    static void setValue(PropertyChange* change, const QVariant &value);
    static void restore(PropertyChange* change);
private:
    bool backedUp;
    BackupType type;
    QQmlProperty qmlProperty;
    QVariant backupValue;
    QQmlAbstractBinding *backupBinding;

    void backup();
};

#endif // PROPERTYCHANGE_P_H
