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

#include "lttnglogger_p.h"

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER quickplus
#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./tracepoint_p.h"
#if !defined(TRACEPOINT_P_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define TRACEPOINT_P_H
#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    quickplus, process,
    TP_ARGS(
        TraceProcess*, traceProcess
    ),
    TP_FIELDS(
        ctf_integer(quint16, cpu_usage, traceProcess->cpuUsage)
        ctf_integer(quint32, vsz_memory, traceProcess->vszMemory)
        ctf_integer(quint32, rss_memory, traceProcess->rssMemory)
        ctf_integer(quint16, thread_count, traceProcess->threadCount)
    )
)

TRACEPOINT_EVENT(
    quickplus, frame,
    TP_ARGS(
        TraceFrame*, traceFrame
    ),
    TP_FIELDS(
        ctf_integer(quint32, window, traceFrame->window)
        ctf_integer(quint32, number, traceFrame->number)
        ctf_float(float, delta_time, traceFrame->deltaTime)
        ctf_float(float, sync_time, traceFrame->syncTime)
        ctf_float(float, render_time, traceFrame->renderTime)
        ctf_float(float, gpu_time, traceFrame->gpuTime)
        ctf_float(float, swap_time, traceFrame->swapTime)
    )
)

TRACEPOINT_EVENT(
    quickplus, window,
    TP_ARGS(
        TraceWindow*, traceWindow
    ),
    TP_FIELDS(
        ctf_integer(quint32, id, traceWindow->id)
        ctf_string(state, traceWindow->state)
        ctf_integer(quint16, width, traceWindow->width)
        ctf_integer(quint16, height, traceWindow->height)
    )
)

#endif  // TRACEPOINT_P_H
#include <lttng/tracepoint-event.h>
