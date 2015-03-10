/*
 * Copyright 2015 Canonical Ltd.
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

#include "ucpalettechanges.h"
#include "i18n.h"

#include <QtQml/private/qqmlcompiler_p.h>

UCPaletteChanges::UCPaletteChanges(QObject *parent)
    : QObject(parent)
    , m_values("")
{
}


void UCPaletteChangesParser::verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    Q_UNUSED(qmlUnit);
    Q_UNUSED(bindings);
    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
//        QString propName = qmlUnit->stringAt(binding->propertyNameIndex);
//        if (!propName.isEmpty()) {
//            error(binding, "ERROR");
//            return;
//        }
//        qDebug() << propName;
        verifyProperty(qmlUnit, binding);
    }
}

void UCPaletteChangesParser::applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    Q_UNUSED(obj);
    Q_UNUSED(cdata);
    Q_UNUSED(bindings);
//    UCPaletteChanges *paletteChanges = static_cast<UCPaletteChanges*>(obj);
    const QV4::CompiledData::Unit *qmlUnit = cdata->compilationUnit->data;
    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
        if (binding->type != QV4::CompiledData::Binding::Type_Object) {
            qDebug() << "PROP" << qmlUnit->stringAt(binding->propertyNameIndex);
            continue;
        }
    }
}

void UCPaletteChangesParser::verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding)
{
    if (binding->type == QV4::CompiledData::Binding::Type_Object) {
        error(qmlUnit->objectAt(binding->value.objectIndex), UbuntuI18n::instance().tr("PaletteChanges does not support creating state-specific objects."));
        return;
    }

    if (binding->type == QV4::CompiledData::Binding::Type_GroupProperty
        || binding->type == QV4::CompiledData::Binding::Type_AttachedProperty) {
        const QV4::CompiledData::Object *subObj = qmlUnit->objectAt(binding->value.objectIndex);
        const QV4::CompiledData::Binding *subBinding = subObj->bindingTable();
        for (quint32 i = 0; i < subObj->nBindings; ++i, ++subBinding) {
            verifyProperty(qmlUnit, subBinding);
        }
    }
}

void UCPaletteChangesParser::applyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding, int outterElementIndex)
{
    Q_UNUSED(qmlUnit);
    Q_UNUSED(binding);
    Q_UNUSED(outterElementIndex);
}
