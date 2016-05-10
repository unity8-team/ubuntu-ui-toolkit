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

#ifndef METRICS_H
#define METRICS_H

#include "quickplusglobal.h"

struct QUICK_PLUS_EXPORT QuickPlusMetrics
{
    // Time stamp in nanosecond. Filled by QuickPlusMetricsTracker at buffer
    // swap, so the difference between it and the previous generated metrics
    // gives the delta between the two frames.
    quint64 timeStamp;

    // Time in nanosecond taken by the QtQuick scene graph synchronisation pass.
    quint64 syncTime;

    // Time in nanosecond taken by the QtQuick scene graph render pass.
    quint64 renderTime;

    // Time in nanosecond taken by the GPU to execute the graphics commands
    // pushed during the QtQuick scene graph render pass.
    quint64 gpuTime;

    // Time in nanosecond taken by the graphics subsystem's buffer swap call.
    quint64 swapTime;

    // The frame number. Corresponds to the number of frame rendered.
    quint32 frameNumber;

    // CPU usage of the process as a percentage. 100% if all the cores are at
    // 100% usage, 50% if half of the cores are at 100%.
    quint32 cpuUsage;

    // Virtual size of the process in kilobyte.
    quint32 vszMemory;

    // Resident set size of the process in kilobyte.
    quint32 rssMemory;

    // Width of the frame.
    quint16 frameWidth;

    // Height of the frame.
    quint16 frameHeight;

    // Number of threads at buffer swap.
    quint16 threadCount;

    // The whole struct takes 128 bytes to allow for future additions, don't
    // forget to update it when adding new metrics.
    quint8 __reserved[/*62 bytes taken +*/ 66 /* bytes free */];
};

#endif  // METRICS_H
