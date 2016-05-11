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

#include "metricslogger_p.h"
#include "quickplusglobal_p.h"
#include <QtCore/QDir>

QuickPlusFileMetricsLogger::QuickPlusFileMetricsLogger(const QString& fileName)
    : d_ptr(new FileMetricsLoggerPrivate(fileName))
{
    DLOG_FUNC();
}

FileMetricsLoggerPrivate::FileMetricsLoggerPrivate(const QString& fileName)
{
    DLOG_FUNC();

    if (QDir::isRelativePath(fileName)) {
        m_file.setFileName(QString(QDir::currentPath() + QDir::separator() + fileName));
    } else {
        m_file.setFileName(fileName);
    }

    if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_open = true;
    } else {
        m_open = false;
        WARN("QuickPlusFileMetricsLogger: Can't open file %s '%s'.",
             fileName.toLatin1().constData(), m_file.errorString().toLatin1().constData());
    }
}

QuickPlusFileMetricsLogger::QuickPlusFileMetricsLogger(FILE* fileHandle)
    : d_ptr(new FileMetricsLoggerPrivate(fileHandle))
{
    DLOG_FUNC();
}

FileMetricsLoggerPrivate::FileMetricsLoggerPrivate(FILE* fileHandle)
{
    DLOG_FUNC();

    if (m_file.open(fileHandle, QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_open = true;
    } else {
        m_open = false;
        WARN("QuickPlusFileMetricsLogger: Can't open file handle '%s'.",
             m_file.errorString().toLatin1().constData());
    }
}

QuickPlusFileMetricsLogger::~QuickPlusFileMetricsLogger()
{
    DLOG_FUNC();

    delete d_ptr;
}

bool QuickPlusFileMetricsLogger::isOpen()
{
    DLOG_FUNC();

    return d_func()->m_open;
}

void QuickPlusFileMetricsLogger::log(const QuickPlusMetrics& metrics)
{
    DLOG_FUNC();

    d_func()->log(metrics);
}

void FileMetricsLoggerPrivate::log(const QuickPlusMetrics& metrics)
{
    DLOG_FUNC();

    if (m_open) {
        m_textStream << metrics.timeStamp << ' '
                     << metrics.frameNumber << ' '
                     << metrics.frameWidth << ' '
                     << metrics.frameHeight << ' '
                     << metrics.syncTime << ' '
                     << metrics.renderTime << ' '
                     << metrics.gpuTime << ' '
                     << metrics.swapTime << ' '
                     << metrics.cpuUsage << ' '
                     << metrics.vszMemory << ' '
                     << metrics.rssMemory << ' '
                     << metrics.threadCount << '\n'
                     << flush;
    }
}
