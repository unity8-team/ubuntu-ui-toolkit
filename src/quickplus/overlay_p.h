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

#ifndef OVERLAY_P_H
#define OVERLAY_P_H

#include "bitmaptext_p.h"
#include "events.h"
#include <QtCore/QSize>

#if !defined QT_NO_DEBUG
class QOpenGLContext;
#endif

// 
class Overlay
{
public:
    Overlay(const char* text, int windowId);
    ~Overlay();

    // Allocates/Deletes the OpenGL resources. finalise() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialise().
    bool initialise();
    void finalise();

    // Sets the process event.
    void setProcessEvent(const QuickPlusEvent& processEvent);

    // Renders the overlay. Must be called in a thread with the same OpenGL
    // context bound than at initialise().
    void render(const QuickPlusEvent& frameEvent, const QSize& frameSize);

private:
    void updateFrameMetrics(const QuickPlusEvent& frameEvent);
    void updateWindowMetrics(quint32 windowId, const QSize& frameSize);
    void updateProcessMetrics();
    int keywordString(int index, char* buffer, int bufferSize);
    void parseText();

    enum {
        Initialised       = (1 << 0),
        DirtyText         = (1 << 1),
        DirtyProcessEvent = (1 << 2)
    };

    static const int maxMetricsPerType = 16;

    void* m_buffer;
    char* m_parsedText;
#if !defined QT_NO_DEBUG
    QOpenGLContext* m_context;
#endif
    QString m_text;
    struct {
      quint16 index;
      quint16 textIndex;
      quint8 width;
    } m_metrics[QuickPlusEventType::Count][maxMetricsPerType];
    quint8 m_metricsSize[QuickPlusEventType::Count];
    BitmapText m_bitmapText;
    QSize m_frameSize;
    quint32 m_windowId;
    quint8 m_flags;
    alignas(64) QuickPlusEvent m_processEvent;
};

#endif  // OVERLAY_P_H
