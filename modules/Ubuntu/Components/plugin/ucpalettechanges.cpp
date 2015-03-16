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
#include "uctheme.h"
#include "propertychange_p.h"

#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>

void UCPaletteChangesParser::verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
        verifyProperty(qmlUnit, binding);
    }
}

void UCPaletteChangesParser::applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    UCPaletteChanges *changes = static_cast<UCPaletteChanges*>(obj);
    if (!changes->palette()) {
        qmlInfo(changes->theme()) << UbuntuI18n::instance().tr("ThemeSettings does not define a palette.");
        return;
    }

    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
        changes->applyProperty(NULL, QString(), cdata->compilationUnit->data, binding);
    }

    changes->m_cdata = cdata;
    changes->m_decoded = true;
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

void UCPaletteChanges::applyProperty(QObject *paletteSet, const QString &propertyPrefix, const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding)
{
    QString propertyName = propertyPrefix + qmlUnit->stringAt(binding->propertyNameIndex);

    if (binding->type == QV4::CompiledData::Binding::Type_GroupProperty
            || binding->type == QV4::CompiledData::Binding::Type_AttachedProperty) {

        // check if the palette has this value set
        int propIndex = palette()->metaObject()->indexOfProperty(propertyName.toUtf8());
        if (propIndex < 0) {
            qmlInfo(this) << UbuntuI18n::instance().tr("Palette has no valueset %1").arg(propertyName);
            return;
        }

        QString pre = propertyName + QLatin1Char('.');
        const QV4::CompiledData::Object *subObj = qmlUnit->objectAt(binding->value.objectIndex);
        const QV4::CompiledData::Binding *subBinding = subObj->bindingTable();
        for (quint32 i = 0; i < subObj->nBindings; ++i, ++subBinding) {
            applyProperty(valueSet(propertyName), pre, qmlUnit, subBinding);
        }
        return;
    }

    // check if palette has the given property
    if (!paletteSet) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Palette has no property called '%1'.").arg(propertyName);
        return;
    }
    if (paletteSet->metaObject()->indexOfProperty(qmlUnit->stringAt(binding->propertyNameIndex).toUtf8()) < 0) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Palette has no property called '%1'.").arg(propertyName);
        return;
    }

    switch (binding->type) {
    case QV4::CompiledData::Binding::Type_Script:
    {
        QString expression = binding->valueAsScriptString(qmlUnit);
        QUrl url = QUrl();
        int line = -1;
        int column = -1;

        QQmlData *ddata = QQmlData::get(this);
        if (ddata && ddata->outerContext && !ddata->outerContext->url.isEmpty()) {
            url = ddata->outerContext->url;
            line = ddata->lineNumber;
            column = ddata->columnNumber;
        }
        m_expressions << Expression(propertyName, binding->value.compiledScriptIndex, expression, url, line, column);
        break;
    }
    case QV4::CompiledData::Binding::Type_String:
    {
        m_values << qMakePair(propertyName, binding->valueAsString(qmlUnit));
        break;
    }
    default:
        qmlInfo(this) << UbuntuI18n::instance().tr("Not a valid color value.");
        break;
    }
}


/******************************************************************************
 * PaletteChanges
 */
/*!
 * \qmltype PaletteChanges
 * \instantiates UCPaletteChanges
 * \inqmlmodule Ubuntu.Components 1.3
 * \since Ubuntu.Components 1.3
 * \ingroup theming
 * \brief The component is used to apply changes on a ThemeSettings individual
 * palette values.
 *
 * The component can be declared only inside a ThemeSettings.
 */

/*!
 * \qmlproperty string PaletteChanges::invertValues
 * The property specifies the palette values to be inverted between \c normal and
 * \c selected PaletteValue instances. Defaults to an empty string. Palette values
 * to be inverted must be specified with comas.
 * \qml
 * ThemeSettings {
 *     PaletteChanges {
 *         invertValues: "foregroundText, overlayText"
 *     }
 * }
 * \endqml
 * In vase all values must be inverted, use '*'.
 * \qml
 * ThemeSettings {
 *     PaletteChanges {
 *         invertValues: "*"
 *     }
 * }
 * \endqml
 */
UCPaletteChanges::UCPaletteChanges(QObject *parent)
    : QObject(parent)
    , m_decoded(false)
    , m_explicit(false)
{
}

UCPaletteChanges::~UCPaletteChanges()
{
    restorePaletteValues();
}

void UCPaletteChanges::classBegin()
{
    if (!qobject_cast<UCTheme*>(parent())) {
        qmlInfo(this) << UbuntuI18n::instance().tr("PaletteChanges can only be declared in ThemeSettings components.");
    } else {
        connect(theme(), &UCTheme::paletteChanged, this, &UCPaletteChanges::_q_applyPaletteChanges);
    }
}
void UCPaletteChanges::componentComplete()
{
    if (palette() && m_decoded) {
        _q_applyPaletteChanges();
    }
}

UCTheme *UCPaletteChanges::theme()
{
    return qobject_cast<UCTheme*>(parent());
}
QObject *UCPaletteChanges::palette()
{
    UCTheme *set = theme();
    return set ? set->palette() : NULL;
}

QObject *UCPaletteChanges::valueSet(const QString &name)
{
    QObject *stylePalette = palette();
    return stylePalette ? stylePalette->property(name.toLocal8Bit()).value<QObject*>() : NULL;
}

void UCPaletteChanges::saveAndSetProperty(const QString &property, const QVariant &value)
{
    PropertyChange *change = new PropertyChange(palette(), property.toUtf8(), PropertyChange::BindingBackup);
    m_restoreList << change;
    change->setValue(value);
}

void UCPaletteChanges::saveAndSetProperty(const QString &property, QQmlBinding *binding)
{
    PropertyChange *change = new PropertyChange(palette(), property.toUtf8(), PropertyChange::BindingBackup);
    binding->setTarget(change->property());
    m_restoreList << change;
    change->setValue(binding);
}

void UCPaletteChanges::restorePaletteValues()
{
    qDeleteAll(m_restoreList);
    m_restoreList.clear();
}

void UCPaletteChanges::switchPaletteValues()
{
    QObject *themePalette = palette();
    if (m_switchColors.isEmpty() || !themePalette) {
        return;
    }
    if (m_switchColors == "*") {
        // switch normal with selected, and return
    }

    // switch individual property values
    QStringList properties;
    properties = m_switchColors.split(',', QString::SkipEmptyParts);
}

void UCPaletteChanges::_q_applyPaletteChanges()
{
    // restore previous values prior to apply changes
    restorePaletteValues();

    // first apply the switch
    switchPaletteValues();

    // as next, apply the value changes
    QObject *object = palette();
    QQmlContext *context = qmlContext(object);
    for (int i = 0; i < m_values.count(); i++) {
        saveAndSetProperty(m_values[i].first, m_values[i].second);
    }

    // then apply expressions/bindings
    for (int ii = 0; ii < m_expressions.count(); ii++) {
        Expression e = m_expressions[ii];
        QQmlProperty prop(object, e.name, context);
        if (!prop.isValid()) {
            continue;
        }

        // create a binding object from the expression using the palette context
        // override context to use this context
        context = qmlContext(this);
        QQmlContextData *cdata = QQmlContextData::get(context);
        QQmlBinding *newBinding = 0;
        if (e.id != QQmlBinding::Invalid) {
            QV4::Scope scope(QQmlEnginePrivate::getV4Engine(qmlEngine(this)));
            QV4::ScopedValue function(scope, QV4::QmlBindingWrapper::createQmlCallableForFunction(cdata, object, m_cdata->compilationUnit->runtimeFunctions[e.id]));
            newBinding = new QQmlBinding(function, object, cdata);
        }
        if (!newBinding) {
            newBinding = new QQmlBinding(e.expression, object, cdata, e.url.toString(), e.line, e.column);
        }

        if (m_explicit) {
            // in this case, we don't want to assign a binding, per se,
            // so we evaluate the expression and assign the result.
            saveAndSetProperty(e.name, newBinding->evaluate());
            newBinding->destroy();
        } else {
            saveAndSetProperty(e.name, newBinding);
        }
    }
}
