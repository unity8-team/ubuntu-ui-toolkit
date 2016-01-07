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

#include "ucplatformthemeplugin.h"
#include "ucplatformtheme.h"
#include <QDebug>

const char *UCPlatformThemePlugin::m_themeName = "ubuntutouchtheme";

UCPlatformThemePlugin::UCPlatformThemePlugin(QObject *parent)
    : QPlatformThemePlugin(parent)
{
}

QPlatformTheme *UCPlatformThemePlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(paramList);
    if (key.compare(m_themeName, Qt::CaseInsensitive)) {
        return Q_NULLPTR;
    }
    return new UCPlatformTheme;
}

