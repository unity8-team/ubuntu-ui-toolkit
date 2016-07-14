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

#ifndef EVENTS_H
#define EVENTS_H

#include <quickplus/quickplusglobal.h>

// FIXME(loicm) Using alignas() in that public header requires users to have a
//     C++11 compiler. That also makes the use of Q_NULLPTR and Q_OVERRIDE in
//     other public headers useless.

struct QUICK_PLUS_EXPORT QuickPlusProcessEvent
{
    // Virtual size of the process in kilobytes.
    quint32 vszMemory;

    // Resident set size (RSS) of the process in kilobytes.
    quint32 rssMemory;

    // CPU usage of the process as a percentage. 100%, for instance, if all the
    // cores are at 100% usage, 50% if half of the cores are at 100%.
    quint16 cpuUsage;

    // Number of threads at buffer swap.
    quint16 threadCount;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*12 bytes taken,*/ 100 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickPlusProcessEvent) == 112);

struct QUICK_PLUS_EXPORT QuickPlusWindowEvent
{
    // Window id.
    quint32 id;

    // Width of the window.
    quint16 width;

    // Height of the window.
    quint16 height;

    // State of the window. 0 for "Hidden", 1 for "Shown", 2 for "Resized".
    quint8 state;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*9 bytes taken,*/ 103 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickPlusWindowEvent) == 112);

struct QUICK_PLUS_EXPORT QuickPlusFrameEvent
{
    // The id of the window on which the frame has been rendered.
    quint32 window;

    // The frame number. Corresponds to the number of frames rendered after
    // scene graph initialisation.
    quint32 number;

    // Time in nanoseconds since the last frame swap.
    quint64 deltaTime;

    // Time in nanoseconds taken by the QtQuick scene graph synchronisation
    // pass.
    quint64 syncTime;

    // Time in nanoseconds taken by the QtQuick scene graph render pass.
    quint64 renderTime;

    // Time in nanoseconds taken by the GPU to execute the graphics commands
    // pushed during the QtQuick scene graph render pass.
    quint64 gpuTime;

    // Time in nanoseconds taken by the graphics subsystem's buffer swap call.
    quint64 swapTime;

    // The whole struct must take 112 bytes to allow future additions and best
    // memory alignment, don't forget to update when adding new metrics.
    quint8 __reserved[/*48 bytes taken,*/ 64 /*bytes free*/];
};
Q_STATIC_ASSERT(sizeof(QuickPlusFrameEvent) == 112);

struct QUICK_PLUS_EXPORT QuickPlusEvent
{
    enum Type { Process = 0, Window = 1, Frame = 2, TypeCount = 3 };

    // Event type.
    Type type;

    // Time stamp in nanoseconds.
    alignas(8) quint64 timeStamp;

    union {
        QuickPlusProcessEvent process;
        QuickPlusWindowEvent window;
        QuickPlusFrameEvent frame;
    };
};
Q_STATIC_ASSERT(sizeof(QuickPlusEvent) == 128);

class EventUtilsPrivate;

// Utilities to manipulate events.
class QUICK_PLUS_EXPORT QuickPlusEventUtils
{
public:
    QuickPlusEventUtils();
    ~QuickPlusEventUtils();

    // Fill the given event with updated process metrics.
    void updateProcessEvent(QuickPlusEvent* event);

    // Get a time stamp in nanoseconds. The timer is started at the first call,
    // returning 0.
    static quint64 timeStamp();

private:
    EventUtilsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(EventUtils);
};

#endif  // EVENTS_H
