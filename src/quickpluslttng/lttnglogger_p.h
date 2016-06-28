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

#ifndef LTTNGLOGGER_P_H
#define LTTNGLOGGER_P_H

#include <QtCore/qglobal.h>

struct TraceProcess
{
    TraceProcess(quint32 _vszMemory, quint32 _rssMemory, quint16 _cpuUsage, quint16 _threadCount)
        : vszMemory(_vszMemory), rssMemory(_rssMemory), cpuUsage(_cpuUsage)
        , threadCount(_threadCount) {}

    quint32 vszMemory;
    quint32 rssMemory;
    quint16 cpuUsage;
    quint16 threadCount;
};

struct TraceFrame
{
    TraceFrame(quint32 _window, quint32 _number, float _deltaTime, float _syncTime,
               float _renderTime, float _gpuTime, float _swapTime)
        : window(_window), number(_number), deltaTime(_deltaTime), syncTime(_syncTime)
        , renderTime(_renderTime), gpuTime(_gpuTime), swapTime(_swapTime) {}

    quint32 window;
    quint32 number;
    float deltaTime;
    float syncTime;
    float renderTime;
    float gpuTime;
    float swapTime;
};

struct TraceWindow
{
    TraceWindow(quint32 _id, const char* _state, quint16 _width, quint16 _height)
        : state(_state), id(_id), width(_width), height(_height) {}

    const char* state;
    quint32 id;
    quint16 width;
    quint16 height;
};

#endif // LTTNGLOGGER_P_H
