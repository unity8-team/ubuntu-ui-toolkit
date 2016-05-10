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

#include "lttngmetricslogger_p.h"

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER quickplus
#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./tracepoint_p.h"
#if !defined(TRACEPOINT_P_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define TRACEPOINT_P_H
#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    quickplus, metrics,
    TP_ARGS(
        TraceData*, traceData
    ),
    TP_FIELDS(
        ctf_integer(quint64, timestamp, traceData->timeStamp)
        ctf_integer(quint32, frame_number, traceData->frameNumber)
        ctf_integer(quint16, frame_width, traceData->frameWidth)
        ctf_integer(quint16, frame_height, traceData->frameHeight)
        ctf_float(float, sg_sync, traceData->syncTime)
        ctf_float(float, sg_render, traceData->renderTime)
        ctf_float(float, gpu, traceData->gpuTime)
        ctf_float(float, swap, traceData->swapTime)
        ctf_float(float, total, traceData->totalTime)
        ctf_integer(quint16, cpu_usage, traceData->cpuUsage)
        ctf_integer(quint32, vsz_memory, traceData->vszMemory)
        ctf_integer(quint32, rss_memory, traceData->rssMemory)
        ctf_integer(quint16, thread_count, traceData->threadCount)
    )
)

#endif  // TRACEPOINT_P_H
#include <lttng/tracepoint-event.h>
