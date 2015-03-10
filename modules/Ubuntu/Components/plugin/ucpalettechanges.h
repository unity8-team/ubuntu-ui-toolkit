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

#ifndef UCPALETTECHANGES_H
#define UCPALETTECHANGES_H

#include <QtCore/QObject>

#define foreach Q_FOREACH
#include <QtQml/private/qqmlcustomparser_p.h>
#include <private/qv4engine_p.h>
#include <private/qpodvector_p.h>
#undef foreach

class UCPaletteChanges : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString values MEMBER m_values NOTIFY valuesChanged)
public:
    explicit UCPaletteChanges(QObject *parent = 0);

Q_SIGNALS:
    void valuesChanged();

private:
    QString m_values;
};

class UCPaletteChangesParser : public QQmlCustomParser
{
public:
    UCPaletteChangesParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}

    virtual void verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings);
    virtual void applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings);

private:
    void verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);
    void applyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding, int outterElementIndex);
};

#endif // UCPALETTECHANGES_H
