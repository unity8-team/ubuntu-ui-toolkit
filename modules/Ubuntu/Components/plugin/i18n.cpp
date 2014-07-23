/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Tim Peeters <tim.peeters@canonical.om>
 */

#include <gio/gio.h>
#include "i18n.h"
#include <QtCore/QDir>

namespace C {
#include <libintl.h>
}

#include <stdlib.h>
#include <locale.h>

extern "C" {
#include <SystemSettings/service.h>
}

void setSettings(GObject* object, GAsyncResult* result, gpointer user_data);
void setSessionLanguage(GObject* object, GAsyncResult* result, gpointer user_data);
void setSessionLocale(GObject* object, GAsyncResult* result, gpointer user_data);
void sessionLanguageChanged(USSSettings* settings, const gchar* language, UbuntuI18n* that);
void sessionLocaleChanged(USSSettings* settings, const gchar* locale, UbuntuI18n* that);

/*!
 * \qmltype i18n
 * \instantiates UbuntuI18n
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief i18n is a context property that provides internationalization support.
 *
 * i18n cannot be instantiated, and is already available as a context property.
 * It is based on \l {https://www.gnu.org/software/gettext/} {gettext}, and thus the standard gettext tools can be used for translating
 * a project. Example:
 * \qml
 * import QtQuick 2.0
 * import Ubunut.Components 0.1
 *
 * Item {
 *      width: units.gu(40)
 *      height: units.gu(50)
 *
 *      Button {
 *          anchors.centerIn: parent
 *          text: i18n.tr("Press me")
 *      }
 * }
 * \endqml
 */
UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
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
    m_language = setlocale(LC_ALL, "");

    m_settings = NULL;
    m_cancellable = g_cancellable_new();
    uss_settings_proxy_new_for_bus(G_BUS_TYPE_SESSION,
                                   G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                   "com.canonical.UbuntuSystemSettings",
                                   "/com/canonical/UbuntuSystemSettings",
                                   m_cancellable,
                                   ::setSettings,
                                   this);

    connect(this, SIGNAL(sessionLanguageChanged()), SLOT(updateLanguage()), Qt::UniqueConnection);
    connect(this, SIGNAL(sessionLocaleChanged()), SLOT(updateLanguage()), Qt::UniqueConnection);
}

UbuntuI18n::~UbuntuI18n()
{
    setSettings(NULL);

    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }
}

void setSettings(GObject* object, GAsyncResult* result, gpointer user_data)
{
    Q_UNUSED(object);

    GError* error = NULL;
    USSSettings* settings = uss_settings_proxy_new_for_bus_finish(result, &error);

    if (settings) {
        UbuntuI18n* that = static_cast<UbuntuI18n*>(user_data);
        that->setSettings(settings);
        g_object_unref(settings);
    } else {
        qWarning("No settings proxy: %s", error->message);
        g_error_free(error);
    }
}

void UbuntuI18n::setSettings(USSSettings* settings)
{
    if (settings != m_settings) {
        if (m_settings) {
            g_signal_handlers_disconnect_by_data(m_settings, this);
            g_object_unref(m_settings);
        }

        m_settings = settings;

        if (m_settings) {
            g_object_ref(m_settings);
            g_signal_connect(m_settings, "language-changed", G_CALLBACK(::sessionLanguageChanged), this);
            g_signal_connect(m_settings, "locale-changed", G_CALLBACK(::sessionLocaleChanged), this);
            uss_settings_call_get_language(m_settings, m_cancellable, ::setSessionLanguage, this);
            uss_settings_call_get_locale(m_settings, m_cancellable, ::setSessionLocale, this);
        }
    }
}

void setSessionLanguage(GObject* object, GAsyncResult* result, gpointer user_data)
{
    USSSettings* settings = USS_SETTINGS(object);
    gchar* language;
    GError* error = NULL;

    if (uss_settings_call_get_language_finish(settings, &language, result, &error)) {
        UbuntuI18n* that = static_cast<UbuntuI18n*>(user_data);
        that->m_sessionLanguage = language;
        g_free(language);
    } else {
        qWarning("GetLanguage() call failed: %s", error->message);
        g_error_free(error);
    }
}

void setSessionLocale(GObject* object, GAsyncResult* result, gpointer user_data)
{
    USSSettings* settings = USS_SETTINGS(object);
    gchar* locale;
    GError* error = NULL;

    if (uss_settings_call_get_language_finish(settings, &locale, result, &error)) {
        UbuntuI18n* that = static_cast<UbuntuI18n*>(user_data);
        that->m_sessionLocale = locale;
        g_free(locale);
    } else {
        qWarning("GetLocale() call failed: %s", error->message);
        g_error_free(error);
    }
}

void sessionLanguageChanged(USSSettings* settings, const gchar* language, UbuntuI18n* that)
{
    Q_UNUSED(settings);

    that->m_sessionLanguage = language;
    Q_EMIT that->sessionLanguageChanged();
}

void sessionLocaleChanged(USSSettings* settings, const gchar* locale, UbuntuI18n* that)
{
    Q_UNUSED(settings);

    that->m_sessionLocale = locale;
    Q_EMIT that->sessionLocaleChanged();
}

void UbuntuI18n::updateLanguage()
{
    setLanguage(sessionLanguage());
    setDomain(domain());
}

/*!
 * \qmlproperty string i18n::domain
 * The gettext domain to be used for the translation. The default domain
 * is the applicationName specified in the application's \l MainView, or the empty string "" if
 * no applicationName was given or no \l MainView is used.
 * Use dtr() functions instead of tr() to use a different domain for a single translation
 * that ignores i18n.domain.
 */
QString UbuntuI18n::domain() const {
    return m_domain;
}

/*!
 * \qmlproperty string i18n::language
 * The language that is used for the translation. The default value is
 * the user's locale dending on $LC_ALL, $LC_MESSAGES and $LANG at the time
 * of running the application. See the gettext manual for details.
 */
QString UbuntuI18n::language() const {
    return m_language;
}

/*!
 * \qmlproperty string i18n::sessionLanguage
 * The language derived from Ubuntu System Settings. If it cannot be obtained,
 * it falls back to the value from language().
 */
QString UbuntuI18n::sessionLanguage() const {
    return !m_sessionLanguage.isEmpty() ? m_sessionLanguage : language();
}

/*!
 * \qmlproperty string i18n::sessionLocale
 * The locale derived from Ubuntu System Settings. If it cannot be obtained,
 * it falls back to the value from language().
 */
QString UbuntuI18n::sessionLocale() const {
    return !m_sessionLocale.isEmpty() ? m_sessionLocale : language();
}

/**
 * \qmlmethod void i18n::bindtextdomain(string domain_name, string dir_name)
 * Specify that the domain_name message catalog can be found
 * in dir_name rather than in the system locale data base.
 */
void UbuntuI18n::bindtextdomain(const QString& domain_name, const QString& dir_name) {
    C::bindtextdomain(domain_name.toUtf8(), dir_name.toUtf8());
    Q_EMIT domainChanged();
}

void UbuntuI18n::setDomain(const QString &domain) {
    m_domain = domain;
    C::textdomain(domain.toUtf8());
    /*
     The default is /usr/share/locale if we don't set a folder
     For click we use APP_DIR/share/locale
     e.g. /usr/share/click/preinstalled/com.example.foo/current/share/locale
     */
    QString appDir(getenv("APP_DIR"));
    if (!QDir::isAbsolutePath (appDir)) {
        appDir = "/usr";
    }
    QString localePath(QDir(appDir).filePath("share/locale"));
    C::bindtextdomain(domain.toUtf8(), localePath.toUtf8());
    Q_EMIT domainChanged();
}

void UbuntuI18n::setLanguage(const QString &lang) {
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
    Q_EMIT languageChanged();
}

/*!
 * \qmlmethod string i18n::tr(string text)
 * Translate \a text using gettext and return the translation.
 */
QString UbuntuI18n::tr(const QString& text)
{
    return QString::fromUtf8(C::gettext(text.toUtf8()));
}

/*!
 * \qmlmethod string i18n::tr(string singular, string plural, int n)
 * Translate the given input string \a singular or \a plural (depending on the number of items \a n)
 * using gettext. Should be called like this:
 *          tr("%n file", "%n files", count)
 */
QString UbuntuI18n::tr(const QString &singular, const QString &plural, int n)
{
    return QString::fromUtf8(C::ngettext(singular.toUtf8(), plural.toUtf8(), n));
}

/*!
 * \qmlmethod string i18n::dtr(string domain, string text)
 * Translate \a text using gettext. Uses the specified domain \a domain instead of i18n.domain.
 */
QString UbuntuI18n::dtr(const QString& domain, const QString& text)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dgettext(NULL, text.toUtf8()));
    } else {
        return QString::fromUtf8(C::dgettext(domain.toUtf8(), text.toUtf8()));
    }
}

/*!
 * \qmlmethod string i18n::dtr(string domain, string singular, string plural, int n)
 * Translate the given text using gettext. Should be called like this:
 *          tr(domain, "%n file", "%n files", count)
 * Uses \a domain for the domain instead of i18n.domain, and \a singular or \a plural
 * as input for the translation depending on the number of items \a n.
 */
QString UbuntuI18n::dtr(const QString& domain, const QString& singular, const QString& plural, int n)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dngettext(NULL, singular.toUtf8(), plural.toUtf8(), n));
    } else {
        return QString::fromUtf8(C::dngettext(domain.toUtf8(), singular.toUtf8(), plural.toUtf8(), n));
    }
}
