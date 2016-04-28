// Copyright © 2016 Canonical Ltd.
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

#include "lttngloggingdevice.h"

#include <QDebug>

#include <dlfcn.h>

#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE
#include "lttng/upmrenderingtimes_tp.h"

QuickPlusLTTNGLoggingDevice::QuickPlusLTTNGLoggingDevice()
 : m_lttngLib(nullptr),
   m_lttngLibLoadTried(false)
{
}

QuickPlusLTTNGLoggingDevice::~QuickPlusLTTNGLoggingDevice()
{
    dlclose(m_lttngLib);
}

void QuickPlusLTTNGLoggingDevice::log(const Counters &counters)
{
    if (!m_lttngLib && !m_lttngLibLoadTried) {
        m_lttngLibLoadTried = true;

        m_lttngLib = dlopen(LTTNG_METRICS_INSTALL_LIB_PATH, RTLD_NOW);
        if (!m_lttngLib) {
            qWarning() << "Failed to load installed tracepoint provider: " << dlerror();
            qWarning() << "Trying to load the builddir one";

            m_lttngLib = dlopen(LTTNG_METRICS_BUILD_LIB_PATH, RTLD_NOW);
            if (!m_lttngLib) {
                qWarning() << "Failed to load build time tracepoint provider: " << dlerror();
            }
        }

        if (m_lttngLib) {
            qDebug() << "Tracing frame rendering times to lttng";
        }
    }
    if (m_lttngLib) {
        tracepoint(quick_plus, metrics, counters.renderTime);
    }
}
