/*
 * Copyright 2012-2015 Canonical Ltd.
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

#ifndef I18N_P_P_H
#define I18N_P_P_H

#include <QtCore/private/qobject_p.h>

#include <UbuntuToolkit/private/i18n_p.h>

UT_NAMESPACE_BEGIN

class UbuntuI18nPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UbuntuI18n)
public:
    UbuntuI18nPrivate();
    virtual ~UbuntuI18nPrivate();

    virtual void bindtextdomain(const QString& domain_name, const QString& dir_name)
    {
        Q_UNUSED(domain_name);
        Q_UNUSED(dir_name);
    }
    virtual bool setDomain(const QString &domain)
    {
        m_domain = domain;
        return true;
    }
    virtual bool setLanguage(const QString &lang)
    {
        m_language = lang;
        return true;
    }

    virtual QString tr(const QString &text)
    {
        return text;
    }

    virtual QString tr(const QString &singular, const QString &plural, int n)
    {
        return (n == 1) ? singular : plural;
    }

    virtual QString dtr(const QString& domain, const QString& text)
    {
        Q_UNUSED(domain);
        return text;
    }
    virtual QString dtr(const QString& domain, const QString& singular, const QString& plural, int n)
    {
        Q_UNUSED(domain);
        return tr(singular, plural, n);
    }

    virtual QString ctr(const QString& context, const QString& text)
    {
        Q_UNUSED(context);
        return text;
    }

    virtual QString dctr(const QString& domain, const QString& context, const QString& text)
    {
        Q_UNUSED(domain);
        Q_UNUSED(context);
        return text;
    }

    static UbuntuI18n *m_i18;
    QString m_domain;
    QString m_language;
};

UT_NAMESPACE_END

#endif // I18N_P_P_H
