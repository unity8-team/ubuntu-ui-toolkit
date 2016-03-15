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
 * Binding
 */

Binding::Binding(const Bindings &bindings, Binding *parent)
    : m_parent(parent)
{
    for (Binding *b : bindings) {
        addChild(b);
    }
}

Binding::~Binding()
{
    if (m_parent) {
        int idx = m_parent->m_children.indexOf(this);
        if (idx >= 0) {
            m_parent->m_children.remove(idx);
        }
    }
}

Binding *Binding::parent()
{
    return m_parent;
}
void Binding::addChild(Binding *binding)
{
    m_children.append(binding);
    binding->m_parent = this;
}

void Binding::connectExpression(BindingUpdateExpression *expression)
{
    m_expression = expression;
    // TODO: connect the binding to the expression's update()
}

void Binding::reconnect()
{
    Q_FOREACH(Binding *b, m_children) {
        b->reconnect();
    }
}


/******************************************************************************
 * UpdateNotifier
 */

/******************************************************************************
 * Binding
 */

} // namespace
