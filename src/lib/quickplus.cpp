// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Quick+.
//
// Quick+ is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// Quick+ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Quick+. If not, see <http://www.gnu.org/licenses/>.

#include "quickplus_p.h"
#include "dropshadow_p.h"

static void initResources()
{
    Q_INIT_RESOURCE(lib);
}

void QuickPlusModule::defineModule()
{
    initResources();

    const char* uri = "QuickPlus";
    qmlRegisterType<QuickPlusDropShadow>(uri, 0, 1, "DropShadow");
}

void QuickPlusModule::undefineModule()
{
}
