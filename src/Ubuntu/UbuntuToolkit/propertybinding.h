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

#ifndef PROPERTYBINDING_H
#define PROPERTYBINDING_H

#include <QtCore/QObject>
#include <functional>
#include <initializer_list>
#include <tuple>
#include "ubuntutoolkitglobal.h"

namespace UbuntuToolkit {

class Binding;
typedef std::initializer_list<Binding> Bindings;

class UBUNTUTOOLKIT_EXPORT AbstractBinding
{
public:
    explicit AbstractBinding() {}
    virtual ~AbstractBinding() {}

    virtual void evaluate() {}
};

template<typename T>
class UBUNTUTOOLKIT_EXPORT PropertyBinding : public AbstractBinding
{
public:
    typedef T ElementType;

    typedef std::function<ElementType()> GetterFunc;

    typedef std::function<void()> Notifier;

    explicit PropertyBinding(GetterFunc getter, Notifier notifier, const Bindings &bindings);
    ~PropertyBinding();

    T value();
    void evaluate() override;

private:
    class Changer {
        explicit Changer(QObject *object, const QMetaMethod &signal);
        ~Changer();

        QObject *object;
        const QMetaMethod signal;
        QMetaObject::Connection *connection;
    };
    void init(const Bindings &changerList);

    std::function<PropertyBinding::ElementType()> getter;
    QVarLengthArray< Changer, 4 > changers;
    Notifier notifier;
    PropertyBinding::ElementType propertyValue;
};

class UBUNTUTOOLKIT_EXPORT Binding
{
public:
    Binding(QObject *object, QMetaMethod method)
        : m_object(object), m_binding(nullptr), m_signal(methid)
    {}

    template<typename T>
    Binding(PropertyBinding<T> *binding)
        : m_object(nullptr), m_binding(binding)
    {}

    QObject *object()
    {
        return m_object;
    }

    QMetaMethod signal()
    {
        return m_signal;
    }

    AbstractBinding *binding()
    {
        return m_binding;
    }

private:
    QObject *m_object;
    AbstractBinding *m_binding;
    QMetaMethod m_signal;
};

} // namespace UbuntuToolkit

#endif // PROPERTYBINDING_H
