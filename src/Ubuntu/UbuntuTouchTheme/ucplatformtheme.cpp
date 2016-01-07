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

#include "ucplatformtheme.h"
#include "ucplatformmenu.h"
#include <QtCore/QVariant>
#include <QtCore/QString>

UCPlatformTheme::UCPlatformTheme()
{
}

UCPlatformTheme::~UCPlatformTheme()
{

}

QVariant UCPlatformTheme::themeHint(ThemeHint hint) const
{
    if (hint == QPlatformTheme::SystemIconThemeName) {
        QByteArray iconTheme = qgetenv("QTUBUNTU_ICON_THEME");
        if (iconTheme.isEmpty()) {
            return QVariant(QStringLiteral("ubuntu-mobile"));
        } else {
            return QVariant(QString(iconTheme));
        }
    } else {
        return QGenericUnixTheme::themeHint(hint);
    }
}

QPlatformMenuItem* UCPlatformTheme::createPlatformMenuItem() const
{
    return new UCPlatformMenuItem();
}

QPlatformMenu* UCPlatformTheme::createPlatformMenu() const
{
    return new UCPlatformMenu;
}
