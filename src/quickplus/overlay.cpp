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

#include "overlay_p.h"
#include "quickplusglobal_p.h"
#include <QtGui/QGuiApplication>
#include <unistd.h>
#include <fcntl.h>

static const QPointF position = QPointF(5.0f, 5.0f);
static const float opacity = 0.85f;

// Keep in sync with corresponding enum!
static const struct {
    const char* const name;
    quint16 size;
} keywordInfo[]= {
    { "qtVersion",  sizeof("qtVersion") - 1  },
    { "qtPlatform", sizeof("qtPlatform") - 1 },
    { "glVersion",  sizeof("glVersion") - 1  },
    { "cpuModel",   sizeof("cpuModel") - 1   },
    { "gpuModel",   sizeof("gpuModel") - 1   }
};
enum {
    QtVersion = 0, QtPlatform, GlVersion, CpuModel, GpuModel, KeywordCount
};
Q_STATIC_ASSERT(ARRAY_SIZE(keywordInfo) == KeywordCount);

// Keep in sync with corresponding enum!
static const struct {
    const char* const name;
    quint16 size;
    quint16 defaultWidth;
    QuickPlusEvent::Type type;
} metricInfo[] = {
    { "cpuUsage",    sizeof("cpuUsage") - 1,    3, QuickPlusEvent::Process },
    { "threadCount", sizeof("threadCount") - 1, 3, QuickPlusEvent::Process },
    { "vszMemory",   sizeof("vszMemory") - 1,   8, QuickPlusEvent::Process },
    { "rssMemory",   sizeof("rssMemory") - 1,   8, QuickPlusEvent::Process },
    { "windowId",    sizeof("windowId") - 1,    2, QuickPlusEvent::Window  },
    { "windowSize",  sizeof("windowSize") - 1,  9, QuickPlusEvent::Window  },
    { "frameNumber", sizeof("frameNumber") - 1, 7, QuickPlusEvent::Frame   },
    { "deltaTime",   sizeof("deltaTime") - 1,   7, QuickPlusEvent::Frame   },
    { "syncTime",    sizeof("syncTime") - 1,    7, QuickPlusEvent::Frame   },
    { "renderTime",  sizeof("renderTime") - 1,  7, QuickPlusEvent::Frame   },
    { "gpuTime",     sizeof("gpuTime") - 1,     7, QuickPlusEvent::Frame   },
    { "totalTime",   sizeof("totalTime") - 1,   7, QuickPlusEvent::Frame   }
};
enum {
    CpuUsage = 0, ThreadCount, VszMemory, RssMemory, WindowId, WindowSize, FrameNumber, DeltaTime,
    SyncTime, RenderTime, GpuTime, TotalTime, MetricCount
};
Q_STATIC_ASSERT(ARRAY_SIZE(metricInfo) == MetricCount);

const int maxMetricWidth = 32;
const int maxKeywordStringSize = 128;
const int bufferSize = 128;
Q_STATIC_ASSERT(
    bufferSize >= maxMetricWidth
    && bufferSize >= maxKeywordStringSize);
const int bufferAlignment = 64;

const int maxParsedTextSize = 1024;  // Including '\0'.

static char cpuModelString[maxKeywordStringSize] = { 0 };
static int cpuModelStringSize = 0;

Overlay::Overlay(const char* text, int windowId)
    : m_parsedText(new char [maxParsedTextSize])
#if !defined QT_NO_DEBUG
    , m_context(nullptr)
#endif
    , m_text(text)
    , m_metricsSize{}
    , m_frameSize(0, 0)
    , m_windowId(windowId)
    , m_flags(DirtyText | DirtyProcessEvent)
{
    DLOG_FUNC();
    DASSERT(text);

    m_buffer = aligned_alloc(bufferAlignment, bufferSize);
    memset(&m_processEvent, 0, sizeof(m_processEvent));
    m_processEvent.type = QuickPlusEvent::Process;
}

Overlay::~Overlay()
{
    DLOG_FUNC();
    DASSERT(!(m_flags & Initialised));

    free(m_buffer);
    delete [] m_parsedText;
}

bool Overlay::initialise()
{
    DLOG_FUNC();
    DASSERT(!(m_flags & Initialised));
    DASSERT(QOpenGLContext::currentContext());

#if !defined QT_NO_DEBUG
    m_context = QOpenGLContext::currentContext();
#endif

    const bool initialised = m_bitmapText.initialise();
    if (initialised) {
        m_bitmapText.bindProgram();
        m_bitmapText.setOpacity(opacity);
        m_flags |= Initialised;
        return true;
    } else {
        return false;
    }
}

void Overlay::finalise()
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    DASSERT(m_context == QOpenGLContext::currentContext());

    m_bitmapText.finalise();
    m_flags &= ~Initialised;

#if !defined QT_NO_DEBUG
    m_context = nullptr;
#endif
}

void Overlay::setProcessEvent(const QuickPlusEvent& processEvent)
{
    DLOG_FUNC();
    DASSERT(processEvent.type == QuickPlusEvent::Process);

    memcpy(&m_processEvent, &processEvent, sizeof(m_processEvent));
    m_flags |= DirtyProcessEvent;
}

void Overlay::render(const QuickPlusEvent& frameEvent, const QSize& frameSize)
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    DASSERT(m_context == QOpenGLContext::currentContext());

    m_bitmapText.bindProgram();
    if (m_flags & DirtyText) {
        parseText();
        m_bitmapText.setText(m_parsedText);
        m_flags &= ~DirtyText;
    }
    if (m_frameSize != frameSize) {
        updateWindowMetrics(m_windowId, frameSize);
        m_bitmapText.setTransform(frameSize, position);
        m_frameSize = frameSize;
    }
    if (m_flags & DirtyProcessEvent) {
        updateProcessMetrics();
        m_flags &= ~DirtyProcessEvent;
    }
    updateFrameMetrics(frameEvent);
    m_bitmapText.render();
}

// Writes a 64-bit unsigned integer as text. The string is right
// aligned. Returns the remaining width.
static int integerMetricToText(quint64 metric, char* text, int width)
{
    DLOG_FUNC();
    DASSERT(text);
    DASSERT(width > 0);

    do {
        text[--width] = (metric % 10) + '0';
        if (width == 0) return 0;
        metric /= 10;
    } while (metric != 0);

    return width;
}

// Writes a 64-bit unsigned integer representing time in nanoseconds as text in
// milliseconds with two decimal digits. The string is right aligned. Returns
// the remaining width.
static int timeMetricToText(quint64 metric, char* text, int width)
{
    DLOG_FUNC();
    DASSERT(text);
    DASSERT(width > 0);

    metric /= 10000;  // 10^−9 to 10^−5 (to keep 2 valid decimal digits).
    const int decimalCount = 2;
    const char decimalPoint = '.';
    int i = 0;

    do {
        // Handle the decimal digits part.
        text[--width] = (metric % 10) + '0';
        if (width == 0) return 0;
        metric /= 10;
    } while (++i < decimalCount && metric != 0);
    if (metric != 0) {
        // Handle the decimal point and integer parts.
        text[--width] = decimalPoint;
        if (width > 0) {
            do {
                text[--width] = (metric % 10) + '0';
                metric /= 10;
            } while (metric != 0 && width > 0);
        }
    } else {
        // Handle metric ms value less than decimalCount digits.
        if (i == 1) {
            text[--width] = '0';
            if (width == 0) return 0;
        }
        text[--width] = decimalPoint;
        if (width > 0) {
            text[--width] = '0';
        }
    }

    return width;
}

void Overlay::updateFrameMetrics(const QuickPlusEvent& event)
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    Q_STATIC_ASSERT(IS_POWER_OF_TWO(maxMetricWidth));

    char* text = static_cast<char*>(m_buffer);
    for (int i = 0; i < m_metricsSize[QuickPlusEvent::Frame]; i++) {
        int textWidth = m_metrics[QuickPlusEvent::Frame][i].width;
        DASSERT(textWidth <= maxMetricWidth);
        memset(text, ' ', maxMetricWidth);

        switch (m_metrics[QuickPlusEvent::Frame][i].index) {
        case FrameNumber:
            integerMetricToText(event.frame.number, text, textWidth);
            break;
        case DeltaTime:
            timeMetricToText(event.frame.deltaTime, text, textWidth);
            break;
        case SyncTime:
            timeMetricToText(event.frame.syncTime, text, textWidth);
            break;
        case RenderTime:
            timeMetricToText(event.frame.renderTime, text, textWidth);
            break;
        case GpuTime:
            if (event.frame.gpuTime > 0) {
                timeMetricToText(event.frame.gpuTime, text, textWidth);
            } else {
                const char* const na = "N/A";
                int naSize = sizeof("N/A") - 1;
                do { text[--textWidth] = na[--naSize]; } while (textWidth > 0 && naSize > 0);
            }
            break;
        case TotalTime: {
            const quint64 time =
                event.frame.syncTime + event.frame.renderTime + event.frame.gpuTime;
            timeMetricToText(time, text, textWidth);
            break;
        }
        default:
            DNOT_REACHED();
            break;
        }

        m_bitmapText.updateText(
            text, m_metrics[QuickPlusEvent::Frame][i].textIndex,
            m_metrics[QuickPlusEvent::Frame][i].width);
    }
}

void Overlay::updateWindowMetrics(quint32 windowId, const QSize& frameSize)
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    Q_STATIC_ASSERT(IS_POWER_OF_TWO(maxMetricWidth));

    char* text = static_cast<char*>(m_buffer);
    for (int i = 0; i < m_metricsSize[QuickPlusEvent::Window]; i++) {
        int textWidth = m_metrics[QuickPlusEvent::Window][i].width;
        DASSERT(textWidth <= maxMetricWidth);
        memset(text, ' ', maxMetricWidth);

        switch (m_metrics[QuickPlusEvent::Window][i].index) {
        case WindowId:
            textWidth = integerMetricToText(windowId, text, textWidth);
            break;
        case WindowSize: {
            textWidth = integerMetricToText(frameSize.height(), text, textWidth);
            if (textWidth >= 2) {
                text[textWidth - 1] = 'x';
                integerMetricToText(frameSize.width(), text, textWidth - 1);
            } else if (textWidth == 1) {
                text[textWidth - 1] = 'x';
            }
            break;
        }
        default:
            DNOT_REACHED();
            break;
        }

        m_bitmapText.updateText(
            text, m_metrics[QuickPlusEvent::Window][i].textIndex,
            m_metrics[QuickPlusEvent::Window][i].width);
    }
}

void Overlay::updateProcessMetrics()
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    Q_STATIC_ASSERT(IS_POWER_OF_TWO(maxMetricWidth));

    char* text = static_cast<char*>(m_buffer);
    for (int i = 0; i < m_metricsSize[QuickPlusEvent::Process]; i++) {
        int textWidth = m_metrics[QuickPlusEvent::Process][i].width;
        DASSERT(textWidth <= maxMetricWidth);
        memset(text, ' ', maxMetricWidth);

        switch (m_metrics[QuickPlusEvent::Process][i].index) {
        case CpuUsage:
            integerMetricToText(m_processEvent.process.cpuUsage, text, textWidth);
            break;
        case ThreadCount:
            integerMetricToText(m_processEvent.process.threadCount, text, textWidth);
            break;
        case VszMemory:
            integerMetricToText(m_processEvent.process.vszMemory, text, textWidth);
            break;
        case RssMemory:
            integerMetricToText(m_processEvent.process.rssMemory, text, textWidth);
            break;
        default:
            DNOT_REACHED();
            break;
        }

        m_bitmapText.updateText(
            text, m_metrics[QuickPlusEvent::Process][i].textIndex,
            m_metrics[QuickPlusEvent::Process][i].width);
    }
}

// That's the easy way, a more involved one would be to use CPUID.
static int cpuModel(char* buffer, int bufferSize)
{
    DLOG_FUNC();
    DASSERT(buffer);
    DASSERT(bufferSize > 0);

    int fd = open("/proc/cpuinfo", O_RDONLY);
    if (fd == -1) {
        DWARN("ApplicationMonitor: Can't open '/proc/cpuinfo'.");
        return 0;
    }
    const int sourceBufferSize = 128;
    char sourceBuffer[sourceBufferSize];
    if (read(fd, sourceBuffer, sourceBufferSize) != sourceBufferSize) {
        DWARN("ApplicationMonitor: Can't read '/proc/cpuinfo'.");
        close(fd);
        return 0;
    }

    // Skip the five first ': ' occurences to reach model name.
    int sourceIndex = 0, colonCount = 0;
    while (colonCount < 5) {
        if (sourceIndex < sourceBufferSize - 1) {
            if (sourceBuffer[sourceIndex] != ':' || sourceBuffer[sourceIndex + 1] != ' ') {
                sourceIndex++;
            } else {
                sourceIndex += 2;
                colonCount++;
            }
        } else {
            DNOT_REACHED();  // Consider increasing sourceBufferSize.
            close(fd);
            return 0;
        }
    }

    int index = 0;
    while (sourceBuffer[sourceIndex] != '\n' && index < bufferSize) {
        if (sourceIndex < sourceBufferSize) {
            buffer[index++] = sourceBuffer[sourceIndex++];
        } else {
            DNOT_REACHED();  // Consider increasing sourceBufferSize.
            index = 0;
            break;
        }
    }

    // Add the core count.
    const int cpuOnlineCores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpuOnlineCores > 1) {
        const int maxSize = sizeof(" (%d cores)") - 2 + 3;  // Adds space for a 3 digits core count.
        const int size = snprintf(sourceBuffer, maxSize, " (%d cores)", cpuOnlineCores);
        if (index + size < bufferSize) {
            memcpy(&buffer[index], sourceBuffer, size);
            index += size;
        }
    }

    close(fd);
    return index;
}

// Stores the keyword string corresponding to the given index in a preallocated
// buffer of size bufferSize, the terminating null byte ('\0') is not
// written. Returns the number of characters written. Requires an OpenGL context
// to be bound to the current thread.
int Overlay::keywordString(int index, char* buffer, int bufferSize)
{
    DLOG_FUNC();
    DASSERT(index < KeywordCount);
    DASSERT(buffer);
    DASSERT(bufferSize > 0);

    int size = 0;

    switch (index) {
    case QtVersion: {
        const char* version = "Qt " QT_VERSION_STR;
        for (; size < bufferSize; size++) {
            if (version[size] == '\0') break;
            buffer[size] = version[size];
        }
        break;
    }
    case QtPlatform: {
        const char* platform = QGuiApplication::platformName().toLatin1().constData();
        for (; size < bufferSize; size++) {
            if (platform[size] == '\0') break;
            buffer[size] = platform[size];
        }
        break;
    }
    case GlVersion: {
        const char* gl = QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL
            ? "OpenGL " : "OpenGL ES ";
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
        const char* version = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));
        for (int i = 0; size < bufferSize; i++, size++) {
            if (gl[i] == '\0') break;
            buffer[size] = gl[i];
        }
        for (int i = 0; size < bufferSize; i++, size++) {
            if (version[i] == '\0') break;
            buffer[size] = version[i];
        }
        break;
    }
    case CpuModel: {
        if (cpuModelStringSize == 0) {
            cpuModelStringSize = cpuModel(cpuModelString, maxKeywordStringSize);
            if (cpuModelStringSize == 0) {
                const char* const defaultModel = "Unknown CPU";
                const int defaultModelSize = sizeof("Unknown CPU") - 1;
                memcpy(cpuModelString, defaultModel, defaultModelSize);
                cpuModelStringSize = defaultModelSize;
            }
        }
        memcpy(buffer, cpuModelString, cpuModelStringSize);
        size = cpuModelStringSize;
        break;
    }
    case GpuModel: {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
        const char* renderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
        for (; size < bufferSize; size++) {
            if (renderer[size] == '\0') break;
            buffer[size] = renderer[size];
        }
        break;
    }
    default:
        DNOT_REACHED();
        break;
    }

    return size;
}

void Overlay::parseText()
{
    DLOG_FUNC();

    QByteArray textLatin1 = m_text.toLatin1();
    const char* const text = textLatin1.constData();
    const int textSize = textLatin1.size();
    char* keywordBuffer = static_cast<char*>(m_buffer);
    int characters = 0;

    for (int i = 0; i <= textSize; i++) {
        const char character = text[i];
        if (character != '%') {
            // Common case.
            m_parsedText[characters++] = character;
        } else if (text[i+1] == '%') {
            // "%%" outputs "%".
            m_parsedText[characters++] = '%';
            i++;
        } else {
            bool keywordFound = false;
            // Search for keywords.
            for (int j = 0; j < KeywordCount; j++) {
                if (!strncmp(&text[i+1], keywordInfo[j].name, keywordInfo[j].size)) {
                    const int stringSize = keywordString(j, keywordBuffer, maxKeywordStringSize);
                    if (stringSize < maxParsedTextSize - characters) {
                        strcpy(&m_parsedText[characters], keywordBuffer);
                        characters += stringSize;
                        i += keywordInfo[j].size;
                    }
                    keywordFound = true;
                    break;
                }
            }
            // Search for metrics.
            if (!keywordFound) {
                int width, widthOffset = 0;
                if (!isdigit(text[i+1+widthOffset])) {
                    width = -1;
                } else {
                    width = text[i+1+widthOffset] - '0';
                    widthOffset++;
                    if (isdigit(text[i+1+widthOffset])) {
                        width = width * 10 + text[i+1+widthOffset] - '0';
                        widthOffset++;
                    }
                    width = qBound(1, width, maxMetricWidth);
                }
                for (int j = 0; j < MetricCount; j++) {
                    const int type = metricInfo[j].type;
                    DASSERT(type >= 0);
                    DASSERT(type < QuickPlusEvent::TypeCount);
                    if (m_metricsSize[type] < maxMetricsPerType &&
                        !strncmp(&text[i+1+widthOffset], metricInfo[j].name, metricInfo[j].size)) {
                        if (width == -1) {
                            width = metricInfo[j].defaultWidth;
                        }
                        if (width < maxParsedTextSize - characters) {
                            m_metrics[type][m_metricsSize[type]].index = j;
                            m_metrics[type][m_metricsSize[type]].textIndex = characters;
                            m_metrics[type][m_metricsSize[type]].width = width;
                            // Must be initialised since it might contain non
                            // printable characters and break setText otherwise.
                            memset(&m_parsedText[characters], '?', width);
                            characters += width;
                            i += widthOffset + metricInfo[j].size;
                            m_metricsSize[type]++;
                        }
                        break;
                    }
                }
            }
        }
        // Set string terminator and quit once the max size is reached.
        if (characters >= (maxParsedTextSize - 1)) {
            m_parsedText[maxParsedTextSize - 1] = '\0';
            break;
        }
    }
}
