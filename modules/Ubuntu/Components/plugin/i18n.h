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
#include <act/act.h>

class QQmlContext;
class QQmlEngine;

class UbuntuI18n : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

private:
    Q_DISABLE_COPY(UbuntuI18n)
    explicit UbuntuI18n(QObject* parent = 0);

public:
    ~UbuntuI18n();

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

    // setter
    void setDomain(const QString& domain);
    void setLanguage(const QString& lang);

Q_SIGNALS:
    void domainChanged();
    void languageChanged();

private:
    void userManagerLoaded();
    void userLoaded();
    void userLanguageChanged();

    friend void user_manager_loaded(GObject* object, GParamSpec* pspec, gpointer user_data);
    friend void user_loaded(GObject* object, GParamSpec* pspec, gpointer user_data);
    friend void user_language_changed(GObject* object, GParamSpec* pspec, gpointer user_data);

    QString m_domain;
    QString m_language;

    ActUserManager* m_manager;
    ActUser* m_user;
};

#endif // UBUNTU_COMPONENTS_I18N_H
