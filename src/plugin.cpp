// Copyright © 2015 Canonical Ltd.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Loïc Molinari <loic.molinari@canonical.com>

#include <QtQml/QQmlExtensionPlugin>
#include "dropshadow.h"

class QuickPlusPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    virtual void registerTypes(const char* uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QuickPlus"));
        Q_UNUSED(uri);
        qmlRegisterType<QuickPlusDropShadow>("QuickPlus", 0, 1, "DropShadow");
    }

    ~QuickPlusPlugin()
    {
    }
};

#include "plugin.moc"
