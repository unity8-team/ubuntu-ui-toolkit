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
#include <QtQml/private/qqmlcompiler_p.h>

class UCPaletteChangesParser;

class UCTheme;
class PropertyChange;
class UCPaletteChanges : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString invertValues MEMBER m_invertValues NOTIFY invertValuesChanged)
public:
    explicit UCPaletteChanges(QObject *parent = 0);
    ~UCPaletteChanges();

    UCTheme *theme();
    QObject *palette();
    QObject *valueSet(const QString &name);

    void applyProperty(QObject *valueSet, const QString &propertyPrefix, const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);

Q_SIGNALS:
    void invertValuesChanged();

private Q_SLOTS:
    void _q_applyPaletteChanges();

protected:
    void classBegin();
    void componentComplete();

private:

    class Expression {
    public:
        Expression(const QString &name, QQmlBinding::Identifier id, const QString& expr,
                         const QUrl &url, int line, int column)
            : name(name), id(id), expression(expr), url(url), line(line), column(column) {}
        QString name;
        QQmlBinding::Identifier id;
        QString expression;
        QUrl url;
        int line;
        int column;
    };

    void saveAndSetProperty(const QString &property, const QVariant &value);
    void saveAndSetProperty(const QString &property, QQmlBinding *binding);
    void restorePaletteValues();

    QString m_invertValues;
    bool m_decoded:1;
    bool m_isExplicit:1;
    QList< QPair<QString, QString> > m_values;
    QList<Expression> m_expressions;
    QList<PropertyChange*> m_restoreList;
    // from parser
    QQmlRefPointer<QQmlCompiledData> m_cdata;

    friend class UCPaletteChangesParser;
};

class UCPaletteChangesParser : public QQmlCustomParser
{
public:
    UCPaletteChangesParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}

    virtual void verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings);
    virtual void applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings);

private:
    void verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);
};

#endif // UCPALETTECHANGES_H
