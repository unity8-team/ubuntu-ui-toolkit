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

#include "logger_p.h"
#include "quickplusglobal_p.h"
#include "events.h"
#include <QtCore/QDir>
#include <QtCore/QTime>

QuickPlusFileLogger::QuickPlusFileLogger(const QString& fileName)
    : d_ptr(new FileLoggerPrivate(fileName))
{
    DLOG_FUNC();
}

FileLoggerPrivate::FileLoggerPrivate(const QString& fileName)
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
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        m_flags = Open;
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file %s '%s'.", fileName.toLatin1().constData(),
             m_file.errorString().toLatin1().constData());
    }
}

QuickPlusFileLogger::QuickPlusFileLogger(FILE* fileHandle)
    : d_ptr(new FileLoggerPrivate(fileHandle))
{
    DLOG_FUNC();
}

FileLoggerPrivate::FileLoggerPrivate(FILE* fileHandle)
{
    DLOG_FUNC();

    if (m_file.open(fileHandle, QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        if ((fileHandle == stdout || fileHandle == stderr) &&
            !qEnvironmentVariableIsSet("QUICKPLUS_NO_LOGGER_COLOR")) {
            m_flags = Open | Colored;
        } else {
            m_flags = Open;
        }
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file handle '%s'.",
             m_file.errorString().toLatin1().constData());
    }
}

QuickPlusFileLogger::~QuickPlusFileLogger()
{
    DLOG_FUNC();

    delete d_ptr;
}

bool QuickPlusFileLogger::isOpen()
{
    DLOG_FUNC();

    return !!(d_func()->m_flags & FileLoggerPrivate::Open);
}

// FIXME(loicm) We should maybe get rid of QTextStream and directly write to the
//     device for efficiency reasons.

void QuickPlusFileLogger::log(const QuickPlusEvent& event)
{
    DLOG_FUNC();

    d_func()->log(event);
}

void FileLoggerPrivate::log(const QuickPlusEvent& event)
{
    DLOG_FUNC();

    if (m_flags & Open) {
        // ANSI/VT100 terminal codes.
        const char* const dim = m_flags & Colored ? "\033[02m" : "";
        const char* const reset = m_flags & Colored ? "\033[00m" : "";
        const char* const dimColon = m_flags & Colored ? "\033[02m:\033[00m" : "=";

        QTime timeStamp = QTime(0, 0).addMSecs(event.timeStamp / 1000000);
        QString timeString = timeStamp.toString(!timeStamp.hour() ? "mm:ss:zzz" : "hh:mm:ss:zzz");

        switch (event.type) {
        case QuickPlusEventType::Process: {
            if (m_flags & Minimal) {
                m_textStream
                    << "P "
                    << event.timeStamp << ' '
                    << event.process.cpuUsage << ' '
                    << event.process.vszMemory << ' '
                    << event.process.rssMemory << ' '
                    << event.process.threadCount << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[33mP\033[00m " : "P ")
                    << dim << timeString << reset << ' '
                    << "CPU" << dimColon << event.process.cpuUsage << "% "
                    << "VSZ" << dimColon << event.process.vszMemory << "kB "
                    << "RSS" << dimColon << event.process.rssMemory << "kB "
                    << "Threads" << dimColon << event.process.threadCount
                    << '\n' << flush;
            }
            break;
        }

        case QuickPlusEventType::Frame:
            if (m_flags & Minimal) {
                m_textStream
                    << "F "
                    << event.timeStamp << ' '
                    << event.frame.window << ' '
                    << event.frame.number << ' '
                    << event.frame.deltaTime << ' '
                    << event.frame.syncTime << ' '
                    << event.frame.renderTime << ' '
                    << event.frame.gpuTime << ' '
                    << event.frame.swapTime << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[36mF\033[00m " : "F ")
                    << dim << timeString << reset << ' '
                    << "Win" << dimColon << event.frame.window << ' '
                    << "N" << dimColon << event.frame.number << ' '
                    << "Delta" << dimColon << event.frame.deltaTime / 1000000.0f << "ms "
                    << "Sync" << dimColon << event.frame.syncTime / 1000000.0f << "ms "
                    << "Render" << dimColon << event.frame.renderTime / 1000000.0f << "ms "
                    << "GPU" << dimColon << event.frame.gpuTime / 1000000.0f << "ms "
                    << "Swap" << dimColon << event.frame.swapTime / 1000000.0f << "ms\n" << flush;
            }
            break;

        case QuickPlusEventType::Window: {
            if (m_flags & Minimal) {
                m_textStream
                    << "W "
                    << event.timeStamp << ' '
                    << event.window.id << ' '
                    << event.window.state << ' '
                    << event.window.width << ' '
                    << event.window.height << '\n' << flush;
            } else {
                const char* const stateString[] = { "Hidden", "Shown", "Resized" };
                Q_STATIC_ASSERT(ARRAY_SIZE(stateString) == QuickPlusEventType::Count);
                m_textStream
                    << (m_flags & Colored ? "\033[35mW\033[00m " : "W ")
                    << dim << timeString << reset << ' '
                    << "Id" << dimColon << event.window.id << ' '
                    << "State" << dimColon << stateString[event.window.state] << ' '
                    << "Size" << dimColon << event.window.width << 'x' << event.window.height
                    << '\n' << flush;
            }
            break;
        }

        default:
            DNOT_REACHED();
            break;
        }
    }
}

void QuickPlusFileLogger::setMinimal(bool minimal)
{
    DLOG_FUNC();
    Q_D(FileLogger);

    if (minimal) {
        d->m_flags |= FileLoggerPrivate::Minimal;
    } else {
        d->m_flags &= ~FileLoggerPrivate::Minimal;
    }
}

bool QuickPlusFileLogger::minimal()
{
    DLOG_FUNC();

    return !!(d_func()->m_flags & FileLoggerPrivate::Open);
}
