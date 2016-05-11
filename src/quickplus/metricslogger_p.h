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

#include "metricslogger.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>

class FileMetricsLoggerPrivate
{
public:
    FileMetricsLoggerPrivate(const QString& fileName);
    FileMetricsLoggerPrivate(FILE* fileHandle);

    void log(const QuickPlusMetrics& metrics);

    QFile m_file;
    QTextStream m_textStream;
    bool m_open;
};
