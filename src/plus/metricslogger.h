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

#ifndef METRICSLOGGER_H
#define METRICSLOGGER_H

#include "quickplusglobal.h"
#include "metrics.h"
#include <QtCore/QFile>

class FileMetricsLoggerPrivate;

// Logs metrics to a specific device.
class QUICK_PLUS_EXPORT QuickPlusMetricsLogger
{
public:
    virtual ~QuickPlusMetricsLogger() {}

    // Log metrics.
    virtual void log(const QuickPlusMetrics& metrics) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// 
class QUICK_PLUS_EXPORT QuickPlusFileMetricsLogger : public QuickPlusMetricsLogger
{
public:
    QuickPlusFileMetricsLogger(const QString& filename);
    QuickPlusFileMetricsLogger(FILE* fileHandle);
    ~QuickPlusFileMetricsLogger();

    void log(const QuickPlusMetrics& metrics) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

private:
    FileMetricsLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(FileMetricsLogger);
};

#endif  // METRICSLOGGER_H
