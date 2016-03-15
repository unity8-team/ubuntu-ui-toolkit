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
#include <QtQml/QQmlProperty>
#include <functional>
#include <initializer_list>
#include <tuple>
#include "ubuntutoolkitglobal.h"

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT BindingUpdateExpression
{
public:
    virtual void update() {}
};

class UBUNTUTOOLKIT_EXPORT Binding
{
public:
    typedef std::initializer_list<Binding*> Bindings;

    explicit Binding(const Bindings &bindings = {}, Binding *parent = 0);
    virtual ~Binding();

    void connectExpression(BindingUpdateExpression *expression);

    // FIXME perhaps call it as update? would collide with the BindingExpression's update
    virtual void reconnect();

    Binding *parent();
protected:
    BindingUpdateExpression *m_expression = nullptr;
    QMetaObject::Connection *m_connection = nullptr;
    QVarLengthArray<Binding*, 4> m_children;

private:
    Binding *m_parent;
    void addChild(Binding *binding);
};

//
// Signal to slot connector
//
typedef std::function<QMetaObject::Connection(QObject *s, QObject *r)> Connector;
template <typename Sender, typename Receiver, typename Signal, typename Slot>
Connector createConnector (Signal sig, Slot sl)
{
    return [sig, sl] (QObject *s, QObject *r){
        Sender *sender = static_cast<Sender *>(s);
        Receiver *receiver = static_cast<Receiver *>(r);

        return QObject::connect(sender, sig, receiver, sl);
    };
}

//
// Signal to lambda connector
//
typedef std::function<QMetaObject::Connection(QObject *s)> LConnector;
template <typename Sender, typename Signal, typename Receiver>
LConnector createConnector (Signal sig, Receiver rec)
{
    return [sig, rec] (QObject *s){
        Sender *sender = static_cast<Sender *>(s);

        return QObject::connect(sender, sig, rec);
    };
}
//
// UpdateBinding (or maybe ReconnectBinding?)
//
class UBUNTUTOOLKIT_EXPORT UpdateBinding : public Binding
{
public:
    UpdateBinding(QObject *sender, Signal signal, const Bindings &bindings = {}, Binding *parent = 0)
        : Binding(bindings, parent)
        , m_sender(sender)
        , m_signal(signal)
    {
        // TODO: connect m_signal to reconnect()
    }

    void reconnect() override
    {
        // reconnect kids first
        Binding::reconnect();
        // ivoke expression's update too
        m_expression->update();
    }

private:
    QObject *m_sender;
    Signal m_signal;
};

//
// PropertyBinding
//
template<typename T>
class UBUNTUTOOLKIT_EXPORT ExpressionBinding : public Binding
{
public:
    typedef std::function<T()> Expression;

    explicit ExpressionBinding(Expression expression, LConnector connector, const Bindings &bindings = {}, Binding *parent = 0)
        : Binding(bindings, parent)
        , m_expression(expression)
    {
//        m_signal = createConnector<QObject>(Signal, &Binding::update);
    }

    void reconnect() override
    {
        if (connection) {
            QObject::disconnect(*connection);
        }
        // TODO oupdate connection
        Binding::reconnect();
    }
private:
    Expression m_expression;
    LConnector m_signal;
};


//
// BindingExpression
//

template<typename T>
class UBUNTUTOOLKIT_EXPORT BindingExpression : private BindingUpdateExpression
{
public:
    typedef T ElementType;

    typedef std::function<ElementType()> GetterFunc;

    typedef std::function<void()> BindingSignal;

    explicit BindingExpression(GetterFunc getter, BindingSignal signal, const Binding::Bindings &bindings)
        : getter(getter)
        , signal(signal)
        , propertyValue(getter())
    {
        init(bindings);
    }
    ~BindingExpression()
    {
        qDeleteAll(bindings);
        bindings.clear();
    }

    T value()
    {
        return propertyValue;
    }

private:
    void init(const Binding::Bindings &bindings)
    {
        Q_UNUSED(bindings);
        for (Binding *b : bindings) {
            this->bindings.append(b);
            // hook update
            b->connectExpression(this);
        }
    }

    void update() override
    {
        ElementType newValue = getter();
        if (newValue == propertyValue) {
            return;
        }
        propertyValue = newValue;
        signal();
    }

    GetterFunc getter;
    QVarLengthArray<Binding*, 4 > bindings;
    BindingSignal signal;
    ElementType propertyValue;
};

} // namespace UbuntuToolkit

#endif // PROPERTYBINDING_H
