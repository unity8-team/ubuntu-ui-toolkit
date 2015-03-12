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

#include "propertychange_p.h"

#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach

/*
 * The class is used to save properties and their bindings while the property is
 * altered temporarily.
 */
PropertyChange::PropertyChange(QObject *item, const char *property, BackupType type)
    : backedUp(false)
    , type(type)
    , qmlProperty(item, property, qmlContext(item))
    , backupBinding(0)
{
}

PropertyChange::~PropertyChange()
{
    restore();
}

void PropertyChange::backup()
{
    if (!backedUp) {
        backupValue = qmlProperty.read();
        if (type == BindingBackup) {
            backupBinding = QQmlPropertyPrivate::binding(qmlProperty);
        }
        backedUp = true;
    }
}

/*
 * Sets a value to the property. Will back up the original values if it wasn't yet.
 * This function can be called many times, it will not destroy the backed up value/binding.
 */
void PropertyChange::setValue(const QVariant &value)
{
    backup();
    // write using QQmlPropertyPrivate so we can keep the bindings
    QQmlPropertyPrivate::write(qmlProperty,
                               value,
                               QQmlPropertyPrivate::BypassInterceptor | QQmlPropertyPrivate::DontRemoveBinding);
}
// static obverload
void PropertyChange::setValue(PropertyChange *change, const QVariant &value)
{
    if (!change) {
        return;
    }
    change->setValue(value);
}

// binding overload function
void PropertyChange::setValue(QQmlAbstractBinding *binding)
{
    if (!binding) {
        return;
    }
    backup();

    QQmlAbstractBinding *old = QQmlPropertyPrivate::setBinding(qmlProperty, binding);
    if (old && old != binding && old != backupBinding) {
        old->destroy();
    }
}

/*
 * Restore backed up value or binding.
 */
void PropertyChange::restore()
{
    if (backedUp) {
        // write using QQmlPropertyPrivate to keep the bindings
        QQmlPropertyPrivate::write(qmlProperty,
                                   backupValue,
                                   QQmlPropertyPrivate::BypassInterceptor | QQmlPropertyPrivate::DontRemoveBinding);
        if (backupBinding) {
            QQmlAbstractBinding *old = QQmlPropertyPrivate::setBinding(qmlProperty, backupBinding);
            if (old && old != backupBinding) {
                old->destroy();
            }
        }
        backedUp = false;
    }
}
// static overload
void PropertyChange::restore(PropertyChange *change)
{
    if (!change) {
        return;
    }
    change->restore();
}

// returns the qml property
QQmlProperty PropertyChange::property() const
{
    return qmlProperty;
}

