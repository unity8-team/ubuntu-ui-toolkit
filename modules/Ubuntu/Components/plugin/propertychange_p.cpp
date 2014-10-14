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
#include <QtQuick/private/qquickanimation_p.h>

#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach

/*
 * The class is used to save properties and their bindings while the property is
 * altered temporarily.
 */
PropertyChange::PropertyChange(QObject *item, const char *property, QObject *parent)
    : QObject(parent)
    , m_backedUp(false)
    , qmlProperty(item, property, qmlContext(item))
    , animation(0)
{
}
PropertyChange::~PropertyChange()
{
    restore(this);
}

void PropertyChange::completeRestore()
{
    QObject::disconnect(animation, SIGNAL(stopped()), this, SLOT(completeRestore()));
    if (m_backedUp) {
        // if there was a binding, restore it
        if (backup.first) {
            QQmlAbstractBinding *prevBinding = QQmlPropertyPrivate::setBinding(qmlProperty, backup.first);
            if (prevBinding && prevBinding != backup.first) {
                prevBinding->destroy();
            }
        } else {
            // there was no binding, restore previous value
            qmlProperty.write(backup.second);
        }
        m_backedUp = false;
        Q_EMIT restoreCompleted();
    }
}

/*
 * Set animation to be used when value is changed.
 */
void PropertyChange::setAnimation(PropertyChange *change, QQuickPropertyAnimation *animation)
{
    if (change == NULL) {
        return;
    }
    if (change->animation && change->animation->isRunning()) {
        change->animation->complete();
    }
    change->animation = animation;
}

/*
 * Sets a value to the property. Will back up the original values if it wasn't yet.
 * This function can be called many times, it will not destroy the backed up value/binding.
 */
void PropertyChange::setValue(PropertyChange *change, const QVariant &value)
{
    if (!change) {
        return;
    }
    if (!change->m_backedUp) {
        change->backup.first = QQmlPropertyPrivate::setBinding(change->qmlProperty, 0);
        change->backup.second = change->qmlProperty.read();
        change->m_backedUp = true;
    }
    if (change->animation) {
        change->animation->complete();
        change->animation->setFrom(change->qmlProperty.read());
        change->animation->setTo(value);
        change->animation->start();
    } else {
        change->qmlProperty.write(value);
    }
}

/*
 * Restore backed up value or binding.
 */
void PropertyChange::restore(PropertyChange *change)
{
    if (!change) {
        return;
    }
    if (change->animation) {
        change->animation->stop();
        change->animation->setFrom(change->qmlProperty.read());
        change->animation->setTo(change->backup.second);
        QObject::connect(change->animation, SIGNAL(stopped()), change, SLOT(completeRestore()));
        change->animation->start();
    } else {
        change->completeRestore();
    }
}

/*
 * Return teh original value of the property.
 */
QVariant PropertyChange::originalValue(PropertyChange *change)
{
    return change ? change->backup.second : QVariant();
}
