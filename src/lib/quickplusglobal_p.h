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

#ifndef QUICKPLUSGLOBAL_P_H
#define QUICKPLUSGLOBAL_P_H

#include "quickplusglobal.h"

// Debug flag. Set to 0 to log each func call.
#define INHIBIT_LOG_FUNC 1

// Logging macros, debug macros are compiled out for release builds.
#define LOG(...) qDebug(__VA_ARGS__)
#if INHIBIT_LOG_FUNC != 1
#define LOG_FUNC() LOG("%s (%s, line %d)", __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define LOG_FUNC() qt_noop()
#endif
#define WARN(...) qWarning(__VA_ARGS__)
#define ASSERT(cond) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion `"#cond"' failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define NOT_REACHED() \
    qFatal("Assertion `not reached' failed in file %s, line %d", __FILE__, __LINE__);

#if !defined(QT_NO_DEBUG)
#define DLOG(...) LOG(__VA_ARGS__)
#define DLOG_FUNC() LOG_FUNC()
#define DWARN(...) WARN(__VA_ARGS__)
#define DNOT_REACHED(...) NOT_REACHED()
#define DASSERT(cond) ASSERT(cond)
#else
#define DLOG(...) qt_noop()
#define DLOG_FUNC() qt_noop()
#define DWARN(...) qt_noop()
#define DNOT_REACHED(...) qt_noop()
#define DASSERT(cond) qt_noop()
#endif

// Compile-time constant representing the number of elements in an array.
template<typename T, size_t N> constexpr size_t ARRAY_SIZE(T (&)[N]) { return N; }

#define QUICK_PLUS_PRIVATE_EXPORT QUICK_PLUS_EXPORT

#endif  // QUICKPLUSGLOBAL_P_H
