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

#ifndef APPLICATIONMONITOR_H
#define APPLICATIONMONITOR_H

#include <quickplus/quickplusglobal.h>
#include <quickplus/logger.h>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QMutex>

class QQuickWindow;
class LoggingThread;
class ShowFilter;
class QuickPlusEventUtils;
class WindowMonitor;

// Monitor a QtQuick application by automatically tracking QtQuick windows and
// process metrics. The metrics gathered can be logged and displayed by an
// overlay rendered on top of each frame.
class QUICK_PLUS_EXPORT QuickPlusApplicationMonitor
{
public:
    enum MonitorFlag {
        // Render an overlay of real-time metrics on top of each QtQuick frame.
        Overlay = (1 << 0),
        // Pass all the events to the loggers.
        Logging = (1 << 1),
        // Continuously update QtQuick windows.
        ContinuousUpdates = (1 << 2)
    };
    Q_DECLARE_FLAGS(MonitorFlags, MonitorFlag)

    // Start/Stop tracking of QtQuick windows and process metrics. start()
    // returns false in case there's no QCoreApplication instance running or if
    // the monitor has already been started. stop() is automatically called when
    // the QCoreApplication instance is about to be stopped.
    static bool start();
    static void stop();

    // Set flags. Default is Overlay.
    static void setFlags(MonitorFlags flags);
    static MonitorFlags flags();

    // Set the time in milliseconds between two process event updates. -1 to
    // disable automatic updates. Default is 1000, lower value 100, higher value
    // 10000.
    static void setUpdateInterval(int interval);
    static int updateInterval();

    // Set the loggers. Empty by default, max number of loggers is 8.
    static QList<QuickPlusLogger*> loggers();
    static bool installLogger(QuickPlusLogger* logger);
    static bool removeLogger(QuickPlusLogger* logger, bool free = true);
    static void clearLoggers(bool free = true);

    static const int maxMonitors = 16;
    static const int maxLoggers = 8;

private:
    static void startMonitoring(QQuickWindow* window);
    static void stopMonitoring(WindowMonitor* monitor);
    static bool hasMonitor(WindowMonitor* monitor);
    static bool removeMonitor(WindowMonitor* monitor);
    static void update();

    static LoggingThread* m_loggingThread;
    static QuickPlusEventUtils* m_eventUtils;
    static ShowFilter* m_showFilter;
    static WindowMonitor* m_monitors[maxMonitors];
    static QuickPlusLogger* m_loggers[maxLoggers];
    static QMutex m_monitorsMutex;
    static QMetaObject::Connection m_aboutToQuitConnection;
    static QMetaObject::Connection m_lastWindowClosedConnection;
    static QTimer m_updateTimer;
    static int m_monitorCount;
    static int m_loggerCount;
    static int m_updateInterval;
    static quint16 m_flags;
    static QuickPlusEvent m_processEvent;

    // Prevent instantiation, copy and assign.
    QuickPlusApplicationMonitor();
    Q_DISABLE_COPY(QuickPlusApplicationMonitor);

    void* __reserved;

    friend class WindowMonitor;
    friend class WindowMonitorDeleter;
    friend class WindowMonitorFlagSetter;
    friend class ShowFilter;
};

#endif  // APPLICATIONMONITOR_H
