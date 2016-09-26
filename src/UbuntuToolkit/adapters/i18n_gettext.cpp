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
 * Author: Tim Peeters <tim.peeters@canonical.com>
 *         Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "i18n_p_p.h"

namespace C {
#include <libintl.h>
#include <glib.h>
#include <glib/gi18n.h>
}

UT_NAMESPACE_BEGIN

class GettextAdaptation : public UbuntuI18nPrivate
{
public:
    GettextAdaptation()
    {
        /*
         * setlocale
         * category = LC_ALL: All types of i18n: LC_MESSAGES, LC_NUMERIC, LC_TIME
         * locale = "": Lookup the environment for $LC_ALL, $LC_* and $LANG in that order
         * Returns: for example en_US.utf8, da_DK or POSIX
         *
         * Note: $LANGUAGE is implicitly respected by gettext() calls and
         *   defines the order of multiple locales
         */
        m_language = QString::fromLocal8Bit(setlocale(LC_ALL, ""));
    }

    void bindtextdomain(const QString& domain_name, const QString& dir_name) override
    {
        C::bindtextdomain(domain_name.toUtf8(), dir_name.toUtf8());
    }

    bool setDomain(const QString &domain) override
    {
        if (m_domain == domain) {
            return false;
        }

        m_domain = domain;
        C::textdomain(domain.toUtf8());
        /*
         The default is /usr/share/locale if we don't set a folder
         For click we use APP_DIR/share/locale
         e.g. /usr/share/click/preinstalled/com.example.foo/current/share/locale
         */
        QString appDir = QString::fromLocal8Bit(getenv("APP_DIR"));
        if (!QDir::isAbsolutePath (appDir)) {
            appDir = QStringLiteral("/usr");
        }
        QString localePath(QDir(appDir).filePath(QStringLiteral("share/locale")));
        C::bindtextdomain(domain.toUtf8(), localePath.toUtf8());
        return true;
    }

    bool setLanguage(const QString &lang) override
    {
        if (m_language == lang)
            return false;

        m_language = lang;

        /*
         This is needed for LP: #1263163.

         LANGUAGE may be set to one or more languages for example "fi" or
         "sv:de". gettext prioritizes LANGUAGE over LC_ALL, LC_*, and
         LANG, so if the session has already set LANGUAGE, calls to
         gettext will only use that.  We must override it here so that
         future calls to gettext are done in the new language.

         This only affects the current process. It does not override the
         user's session LANGUAGE.
         */
        setenv("LANGUAGE", lang.toUtf8().constData(), 1);

        /*
         The inverse form of setlocale as used in the constructor, passing
         a valid locale string updates all category type defaults.
         */
        setlocale(LC_ALL, lang.toUtf8());
        return true;
    }

    QString tr(const QString &text) override
    {
        return QString::fromUtf8(C::gettext(text.toUtf8()));
    }

    QString tr(const QString &singular, const QString &plural, int n) override
    {
        return QString::fromUtf8(C::ngettext(singular.toUtf8(), plural.toUtf8(), n));
    }

    QString dtr(const QString& domain, const QString& text) override
    {
        if (domain.isNull()) {
            return QString::fromUtf8(C::dgettext(NULL, text.toUtf8()));
        } else {
            return QString::fromUtf8(C::dgettext(domain.toUtf8(), text.toUtf8()));
        }
    }

    QString dtr(const QString& domain, const QString& singular, const QString& plural, int n) override
    {
        if (domain.isNull()) {
            return QString::fromUtf8(C::dngettext(NULL, singular.toUtf8(), plural.toUtf8(), n));
        } else {
            return QString::fromUtf8(C::dngettext(domain.toUtf8(), singular.toUtf8(), plural.toUtf8(), n));
        }
    }

    QString ctr(const QString& context, const QString& text) override
    {
        return dctr(QString(), context, text);
    }

    QString dctr(const QString& domain, const QString& context, const QString& text) override
    {
        if (domain.isNull()) {
            return QString::fromUtf8(C::g_dpgettext2(NULL, context.toUtf8(), text.toUtf8()));
        } else {
            return QString::fromUtf8(C::g_dpgettext2(domain.toUtf8(), context.toUtf8(), text.toUtf8()));
        }
    }
};

UbuntuI18n::UbuntuI18n(QObject* parent)
    : QObject(*(new GettextAdaptation), parent)
{
}

UT_NAMESPACE_END
