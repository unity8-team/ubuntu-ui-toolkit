// Copyright © 2016 Canonical Ltd.
// Authors: Loïc Molinari <loic.molinari@canonical.com>
//          Albert Astals Cid <albert.astals@canonical.com>
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

#ifndef QUICKPLUSLTTNGGLOBAL_H
#define QUICKPLUSLTTNGGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QUICK_PLUS_LTTNG_BUILD)
#define QUICK_PLUS_LTTNG_EXPORT Q_DECL_EXPORT
#else
#define QUICK_PLUS_LTTNG_EXPORT Q_DECL_IMPORT
#endif

#endif  // QUICKPLUSLTTNGGLOBAL_H
