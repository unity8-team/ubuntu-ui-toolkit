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

#include "lttngmetricslogger.h"
#include "lttngmetricslogger_p.h"
#include "quickplusglobal_p.h"
#include "tracepoint_p.h"

void QuickPlusLTTNGMetricsLogger::log(const QuickPlusMetrics& m)
{
    DLOG_FUNC();

    TraceData traceData(
        m.timeStamp, m.frameNumber, m.vszMemory, m.rssMemory, m.syncTime * 0.000001f,
        m.renderTime * 0.000001f, m.gpuTime * 0.000001f, m.swapTime * 0.000001f,
        (m.syncTime + m.renderTime + m.gpuTime + m.swapTime) * 0.000001f, m.frameWidth,
        m.frameHeight, m.cpuUsage, m.threadCount);
    tracepoint(quickplus, metrics, &traceData);
}

bool QuickPlusLTTNGMetricsLogger::isOpen()
{
    DLOG_FUNC();

    return true;
}
