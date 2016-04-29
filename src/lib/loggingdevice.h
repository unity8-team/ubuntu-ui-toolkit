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

#ifndef LOGGINGDEVICE_H
#define LOGGINGDEVICE_H

#include "quickplusglobal.h"

//
class QUICK_PLUS_EXPORT QuickPlusLoggingDevice
{
public:
    virtual ~QuickPlusLoggingDevice() {}

    struct Counters {
        uint64_t syncTime;       // ns
        uint64_t renderTime;     // ns
        uint64_t gpuRenderTime;  // ns
        uint32_t frameCount;
        uint32_t cpuUsage;       // %
        uint32_t vszMemory;      // kB
        uint32_t rssMemory;      // kB
    };

    virtual void log(const Counters &counters) = 0;

};

#endif
