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
 * Author: Tim Peeters <tim.peeters@canonical.com>
 */

#ifndef UBUNTU_COMPONENTS_I18N_H
#define UBUNTU_COMPONENTS_I18N_H

#include <QtCore/QObject>

typedef char gchar;
typedef void* gpointer;
typedef struct _GObject GObject;
typedef struct _GParamSpec GParamSpec;
typedef struct _GAsyncResult GAsyncResult;
typedef struct _GCancellable GCancellable;
typedef struct _USSSettings USSSettings;

class QQmlContext;
class QQmlEngine;

class UbuntuI18n : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString sessionLanguage READ sessionLanguage NOTIFY sessionLanguageChanged)
    Q_PROPERTY(QString sessionLocale READ sessionLocale NOTIFY sessionLocaleChanged)

private:
    Q_DISABLE_COPY(UbuntuI18n)
    explicit UbuntuI18n(QObject* parent = 0);
    virtual ~UbuntuI18n();

public:
    static UbuntuI18n& instance() {
        static UbuntuI18n instance;
        return instance;
    }

    Q_INVOKABLE void bindtextdomain(const QString& domain_name, const QString& dir_name);
    Q_INVOKABLE QString tr(const QString& text);
    Q_INVOKABLE QString tr(const QString& singular, const QString& plural, int n);
    Q_INVOKABLE QString dtr(const QString& domain, const QString& text);
    Q_INVOKABLE QString dtr(const QString& domain, const QString& singular, const QString& plural, int n);

    // getter
    QString domain() const;
    QString language() const;
    QString sessionLanguage() const;
    QString sessionLocale() const;

    // setter
    void setDomain(const QString& domain);
    void setLanguage(const QString& lang);

Q_SIGNALS:
    void domainChanged();
    void languageChanged();
    void sessionLanguageChanged();
    void sessionLocaleChanged();

private:
    QString m_domain;
    QString m_language;
    QString m_sessionLanguage;
    QString m_sessionLocale;
    USSSettings* m_settings;
    GCancellable* m_cancellable;

    friend void setSettings(GObject* object, GAsyncResult* result, gpointer that);
    void setSettings(USSSettings* settings);
    friend void setSessionLanguage(GObject* object, GAsyncResult* result, gpointer user_data);
    friend void setSessionLocale(GObject* object, GAsyncResult* result, gpointer user_data);
    friend void sessionLanguageChanged(USSSettings* settings, const gchar* language, UbuntuI18n* that);
    friend void sessionLocaleChanged(USSSettings* settings, const gchar* locale, UbuntuI18n* that);
    Q_SLOT void updateLanguage();
};

#endif // UBUNTU_COMPONENTS_I18N_H
