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

#ifndef LOGGER_H
#define LOGGER_H

#include <quickplus/quickplusglobal.h>
#include <QtCore/QFile>

class QuickPlusEvent;
class FileLoggerPrivate;

// Log events to a specific device.
class QUICK_PLUS_EXPORT QuickPlusLogger
{
public:
    virtual ~QuickPlusLogger() {}

    // Log events.
    virtual void log(const QuickPlusEvent& event) = 0;

    // Get whether the target device has been opened successfully or not.
    virtual bool isOpen() = 0;
};

// Log events to a file.
class QUICK_PLUS_EXPORT QuickPlusFileLogger : public QuickPlusLogger
{
public:
    QuickPlusFileLogger(const QString& filename);
    QuickPlusFileLogger(FILE* fileHandle);
    ~QuickPlusFileLogger();

    void log(const QuickPlusEvent& event) Q_DECL_OVERRIDE;
    bool isOpen() Q_DECL_OVERRIDE;

    void setMinimal(bool minimal);
    bool minimal();

private:
    FileLoggerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(FileLogger);
};

#endif  // LOGGER_H
