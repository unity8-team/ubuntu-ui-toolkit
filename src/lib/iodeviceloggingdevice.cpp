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

#include "iodeviceloggingdevice.h"

#include <QtCore/QFile>

QuickPlusIODeviceLoggingDevice::QuickPlusIODeviceLoggingDevice(FILE *fh)
{
    auto file = new QFile();
    file->open(fh, QIODevice::WriteOnly);
    m_device = file;
    m_ts.setDevice(m_device);
}

QuickPlusIODeviceLoggingDevice::QuickPlusIODeviceLoggingDevice(QIODevice *device)
 : m_device(device)
{
    m_ts.setDevice(m_device);
}

QuickPlusIODeviceLoggingDevice::~QuickPlusIODeviceLoggingDevice()
{
    delete m_device;
}

void QuickPlusIODeviceLoggingDevice::log(const Counters &counters)
{
    // FIXME(loicm) Use a dedicated I/O thread.
    m_ts << counters.frameNumber << ' '
         << counters.syncTime << ' '
         << counters.renderTime << ' '
         << counters.gpuRenderTime << ' '
         << counters.cpuUsage << ' '
         << counters.vszMemory << ' '
         << counters.rssMemory << '\n';
}
