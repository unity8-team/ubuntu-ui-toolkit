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

#include "lttnglogger.h"
#include "lttnglogger_p.h"
#include "events.h"
#include "quickplusglobal_p.h"
#include "tracepoint_p.h"

void QuickPlusLTTNGLogger::log(const QuickPlusEvent& e)
{
    DLOG_FUNC();

    switch (e.type) {
    case QuickPlusEvent::Process: {
        TraceProcess traceProcess(
            e.process.vszMemory, e.process.rssMemory, e.process.cpuUsage, e.process.threadCount);
        tracepoint(quickplus, process, &traceProcess);
        break;
    }

    case QuickPlusEvent::Frame: {
        TraceFrame traceFrame(
            e.frame.window, e.frame.number, e.frame.deltaTime * 0.000001f,
            e.frame.syncTime * 0.000001f, e.frame.renderTime * 0.000001f,
            e.frame.gpuTime * 0.000001f, e.frame.swapTime * 0.000001f);
        tracepoint(quickplus, frame, &traceFrame);
        break;
    }

    case QuickPlusEvent::Window: {
        const char* stateString[] = { "Hidden", "Shown", "Resized" };
        Q_STATIC_ASSERT(ARRAY_SIZE(stateString) == QuickPlusEvent::TypeCount);
        TraceWindow traceWindow(
            e.window.id, stateString[e.window.state], e.window.width, e.window.height);
        tracepoint(quickplus, window, &traceWindow);
        break;
    }

    default:
        DNOT_REACHED();
        break;
    }
}

bool QuickPlusLTTNGLogger::isOpen()
{
    DLOG_FUNC();

    return true;
}
