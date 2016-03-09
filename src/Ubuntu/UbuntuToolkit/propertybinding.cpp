/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "propertybinding.h"
#include <QtCore/QMetaMethod>

namespace UbuntuToolkit {

/******************************************************************************
 * Changer
 */
template<typename T>
PropertyBinding<T>::Changer::Changer(QObject *object, const QMetaMethod &signal)
    : object(object)
    , signal(signal)
    , connection(nullptr)
{
}
template<typename T>
PropertyBinding<T>::Changer::~Changer()
{
    if (connection) {
        QObject::disconnect(*connection);
    }
    delete connection;
}

/******************************************************************************
 * Private
 */
template<typename T>
void PropertyBinding<T>::init(const Bindings &bindings)
{
    // connect changers
    for (Binding binding: bindings) {

        // create the changer
        QObject *object = binding.object();
        QMetaMethod signal = binding.signal();
        Changer *changer = new Changer(object, signal);

        // connect
        changer->connection = new QMetaObject::Connection;
        *(changer->connection) = QObject::connect(object, signal, std::bind(&evaluate, this));
        // then append to the rest of the changers
        changers.append(changer);
    }
}

/******************************************************************************
 * PropertyBinding
 */
template<typename T>
PropertyBinding<T>::PropertyBinding(GetterFunc getter, Notifier notifier, const Bindings &bindings)
    : getter(getter)
    , notifier(notifier)
    , propertyValue(getter())
{
    init(bindings);
}

template<typename T>
PropertyBinding<T>::~PropertyBinding()
{
    qDeleteAll(changers);
    changers.clear();
}

template<typename T>
T PropertyBinding<T>::value()
{
    return propertyValue;
}

template<typename T>
void PropertyBinding<T>::evaluate()
{
    T newValue = getter();
    if (newValue == propertyValue) {
        return;
    }
    propertyValue = newValue;
    notifier();
}

} // namespace
