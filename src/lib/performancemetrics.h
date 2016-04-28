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

#ifndef PERFORMANCEMETRICS_H
#define PERFORMANCEMETRICS_H

#include "quickplusglobal.h"
#include <QtQuick/QQuickWindow>

class PerformanceMetricsPrivate;

class QuickPlusLoggingDevice;

// 
class QUICK_PLUS_EXPORT QuickPlusPerformanceMetrics : public QObject
{
    Q_OBJECT
    Q_ENUMS(UpdatePolicy)

  public:
    QuickPlusPerformanceMetrics(QQuickWindow* window=Q_NULLPTR, bool overlayVisible=false);
    ~QuickPlusPerformanceMetrics();

    enum UpdatePolicy { Live, Continuous };

    // 
    void setWindow(QQuickWindow* window);
    QQuickWindow* window() const;

    // 
    void setOverlayText(const QString& text);
    const QString& overlayText() const;

    // 
    void setOverlayVisible(bool visible);
    bool overlayVisible();

    // 
    void setWindowUpdatePolicy(UpdatePolicy updatePolicy);
    UpdatePolicy windowUpdatePolicy();

    // takes ownership of loggingDevice
    void setLoggingDevice(QuickPlusLoggingDevice* loggingDevice);
    QuickPlusLoggingDevice* loggingDevice() const;

    // 
    void setLogging(bool logging);
    bool logging();

private Q_SLOTS:
    void windowDestroyed(QObject*);
    void windowSizeChanged(int);
    void windowSceneGraphInitialised();
    void windowSceneGraphInvalidated();
    void windowBeforeSynchronising();
    void windowAfterSynchronising();
    void windowBeforeRendering();
    void windowAfterRendering();

private:
    PerformanceMetricsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(PerformanceMetrics);
};

#endif  // PERFORMANCEMETRICS_H
