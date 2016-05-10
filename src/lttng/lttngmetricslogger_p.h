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

#ifndef LTTNGMETRICSLOGGER_P_H
#define LTTNGMETRICSLOGGER_P_H

#include <QtCore/qglobal.h>

struct TraceData
{
    TraceData(quint64 _timeStamp, quint32 _frameNumber, quint32 _vszMemory, quint32 _rssMemory,
              float _syncTime, float _renderTime, float _gpuTime, float _swapTime, float _totalTime,
              quint16 _frameWidth, quint16 _frameHeight, quint16 _cpuUsage, quint16 _threadCount)
        : timeStamp(_timeStamp), frameNumber(_frameNumber), vszMemory(_vszMemory)
        , rssMemory(_rssMemory), syncTime(_syncTime), renderTime(_renderTime), gpuTime(_gpuTime)
        , swapTime(_swapTime), totalTime(_totalTime), frameWidth(_frameWidth)
        , frameHeight(_frameHeight), cpuUsage(_cpuUsage), threadCount(_threadCount) {}

    quint64 timeStamp;
    quint32 frameNumber;
    quint32 vszMemory;
    quint32 rssMemory;
    float syncTime;
    float renderTime;
    float gpuTime;
    float swapTime;
    float totalTime;
    quint16 frameWidth;
    quint16 frameHeight;
    quint16 cpuUsage;
    quint16 threadCount;
};

#endif // LTTNGMETRICSLOGGER_P_H
