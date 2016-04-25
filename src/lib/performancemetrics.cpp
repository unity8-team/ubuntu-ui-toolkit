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

#include "performancemetrics_p.h"
#include "bitmaptextfont_p.h"
#include "quickplusglobal_p.h"
#include <QtCore/QTextStream>
#include <unistd.h>

// --- BitmapText ---

static const GLchar* bitmapTextVertexShaderSource =
#if !defined(QT_OPENGL_ES_2)
    "#define highp \n"
    "#define mediump \n"
    "#define lowp \n"
#endif
    "attribute highp vec4 positionAttrib; \n"
    "attribute mediump vec2 textureCoordAttrib; \n"
    "varying mediump vec2 textureCoord; \n"
    "uniform highp vec4 transform; \n"
    "void main(void) \n"
    "{ \n"
    "    gl_Position = vec4((positionAttrib.xy * transform.xy) + transform.zw, 0.0, 1.0); \n"
    "    textureCoord = textureCoordAttrib; \n"
    "} \n";

static const GLchar* bitmapTextFragmentShaderSource =
#if !defined(QT_OPENGL_ES_2)
    "#define highp \n"
    "#define mediump \n"
    "#define lowp \n"
#endif
    "varying mediump vec2 textureCoord; \n"
    "uniform sampler2D texture; \n"
    "void main() \n"
    "{ \n"
    "    gl_FragColor = texture2D(texture, textureCoord); \n"
    "} \n";

const int defaultFontSize = 14;

BitmapText::BitmapText()
    : m_functions(Q_NULLPTR)
    , m_vertexBuffer(Q_NULLPTR)
    , m_textToVertexBuffer(Q_NULLPTR)
    , m_viewportSize(1, 1)
    , m_position(0.0f, 0.0f)
    , m_transform()
    , m_textLength(0)
    , m_characterCount(0)
    , m_flags(DirtyTransform)
{
    DLOG_FUNC();

    // Set current font based on requested font size.
    const int fontSize = qBound(
        static_cast<int>(g_bitmapTextFont.font[0].size), defaultFontSize & (INT_MAX - 1),
        static_cast<int>(g_bitmapTextFont.font[g_bitmapTextFont.fontCount-1].size));
    for (int i = 0; i < g_bitmapTextFont.fontCount; i++) {
        if (static_cast<int>(g_bitmapTextFont.font[i].size) == fontSize) {
            m_currentFont = i;
            break;
        }
    }
}

BitmapText::~BitmapText()
{
    DLOG_FUNC();

    delete [] m_vertexBuffer;
    delete [] m_textToVertexBuffer;
}

static GLuint createProgram(QOpenGLFunctions* functions, const char* vertexShaderSource,
                            const char* fragmentShaderSource, GLuint* vertexShaderObject,
                            GLuint* fragmentShaderObject)
{
    DLOG_FUNC();

    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;

    vertexShader = functions->glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = functions->glCreateShader(GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragmentShader == 0) {
        DWARN("QuickPlusPerformanceMetrics: glCreateShader() failed (OpenGL error: %d)",
              glGetError());
        return 0;
    }

    functions->glShaderSource(vertexShader, 1, &vertexShaderSource, Q_NULLPTR);
    functions->glCompileShader(vertexShader);
    functions->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetShaderInfoLog(vertexShader, 2048, Q_NULLPTR, infoLog);
        WARN("QuickPlusPerformanceMetrics: vertex shader compilation failed:\n%s\n", infoLog);
#endif
        return 0;
    }

    functions->glShaderSource(fragmentShader, 1, &fragmentShaderSource, Q_NULLPTR);
    functions->glCompileShader(fragmentShader);
    functions->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetShaderInfoLog(fragmentShader, 2048, Q_NULLPTR, infoLog);
        WARN("QuickPlusPerformanceMetrics: fragment shader compilation failed:\n%s\n", infoLog);
#endif
        return 0;
    }

    program = functions->glCreateProgram();
    functions->glAttachShader(program, vertexShader);
    functions->glAttachShader(program, fragmentShader);
    functions->glLinkProgram(program);
    functions->glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetProgramInfoLog(program, 2048, Q_NULLPTR, infoLog);
        WARN("QuickPlusPerformanceMetrics: shader linking failed:\n%s", infoLog);
#endif
        return 0;
    }

    *vertexShaderObject = vertexShader;
    *fragmentShaderObject = fragmentShader;

    return program;
}

bool BitmapText::initialise()
{
    DLOG_FUNC();
    DASSERT(!(m_flags & Initialised));

    m_functions = QOpenGLContext::currentContext()->functions();

    m_functions->glGenTextures(1, &m_texture);
    m_functions->glBindTexture(GL_TEXTURE_2D, m_texture);
    m_functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_bitmapTextFont.textureWidth,
                              g_bitmapTextFont.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                              g_bitmapTextFont.textureData);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_program = createProgram(
        m_functions, bitmapTextVertexShaderSource, bitmapTextFragmentShaderSource,
        &m_vertexShaderObject, &m_fragmentShaderObject);
    if (m_program != 0) {
        m_functions->glBindAttribLocation(m_program, 0, "positionAttrib");
        m_functions->glBindAttribLocation(m_program, 1, "textureCoordAttrib");
        m_functions->glUniform1i(m_functions->glGetUniformLocation(m_program, "texture"), 0);
        m_programTransform = m_functions->glGetUniformLocation(m_program, "transform");
    }

    m_functions->glGenBuffers(1, &m_indexBuffer);

    if (m_texture && m_program && m_indexBuffer) {
        m_flags |= Initialised;
        return true;
    } else {
        return false;
    }
}

void BitmapText::finalise()
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);

    if (m_texture) {
        m_functions->glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }

    if (m_program) {
        m_functions->glDeleteProgram(m_program);
        m_functions->glDeleteShader(m_vertexShaderObject);
        m_functions->glDeleteShader(m_fragmentShaderObject);
        m_program = 0;
        m_vertexShaderObject = 0;
        m_fragmentShaderObject = 0;
    }

    if (m_indexBuffer) {
        m_functions->glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
    }

    m_functions = Q_NULLPTR;
    m_flags &= ~Initialised;
}

void BitmapText::setText(const char* text)
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);

    int textLength = 0;
    int characterCount = 0;

    // Count the number of printable characters and get the given text length.
    if (text) {
        while (text[textLength] != '\0') {
            const char character = text[textLength++];
            if (character >= 32 && character <= 126) {  // Printable characters.
                characterCount++;
            }
        }
    }

    // Clean up and update info.
    if (m_characterCount) {
        delete [] m_vertexBuffer;
        delete [] m_textToVertexBuffer;
    }
    if (characterCount) {
        m_textLength = textLength;
        m_characterCount = characterCount;
        m_flags |= NotEmpty;
    } else {
        // Early exit if the given text is null, empty or filled with
        // non-printable characters.
        m_vertexBuffer = Q_NULLPTR;
        m_textToVertexBuffer = Q_NULLPTR;
        m_textLength = 0;
        m_characterCount = 0;
        m_flags &= ~NotEmpty;
        return;
    }

    // Fill the index buffer. The GL_TRIANGLES primitive mode requires 3 indices
    // per triangle, so 6 per character.
    GLushort* indices = new GLushort [6 * characterCount];
    for (int i = 0; i < characterCount; i++) {
        const GLushort currentIndex = i * 6;
        const GLushort currentVertex = i * 4;
        indices[currentIndex] = currentVertex;
        indices[currentIndex+1] = currentVertex + 1;
        indices[currentIndex+2] = currentVertex + 2;
        indices[currentIndex+3] = currentVertex + 2;
        indices[currentIndex+4] = currentVertex + 1;
        indices[currentIndex+5] = currentVertex + 3;
    }
    m_functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    m_functions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * characterCount * sizeof(GLushort),
                              indices, GL_STATIC_DRAW);  // Deletes and replaces the old data.
    delete [] indices;

    // Allocate and fill the vertex buffer and the text to vertex buffer array.
    m_vertexBuffer = new Vertex [characterCount * 4];
    m_textToVertexBuffer = new int [textLength];
    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalised = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalised = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;
    float x = 0.0f;
    float y = 0.0f;
    characterCount = 0;
    for (int i = 0; i < textLength; i++) {
        char character = text[i];
        if (character >= 32 && character <= 126) {  // Printable characters.
            const int index = characterCount * 4;
            // The atlas stores 2 lines per font size, second line starts at
            // ASCII character 80 at position 49 in the bitmap.
            const float s = ((character - 32) % 48) * fontWidthNormalised;
            const float t = (character < 80) ? t1 : t2;
            m_vertexBuffer[index].x = x;
            m_vertexBuffer[index].y = y;
            m_vertexBuffer[index].s = s;
            m_vertexBuffer[index].t = t;
            m_vertexBuffer[index+1].x = x;
            m_vertexBuffer[index+1].y = y + 1.0f;
            m_vertexBuffer[index+1].s = s;
            m_vertexBuffer[index+1].t = t + fontHeightNormalised;
            m_vertexBuffer[index+2].x = x + 1.0f;
            m_vertexBuffer[index+2].y = y;
            m_vertexBuffer[index+2].s = s + fontWidthNormalised;
            m_vertexBuffer[index+2].t = t;
            m_vertexBuffer[index+3].x = x + 1.0f;
            m_vertexBuffer[index+3].y = y + 1.0f;
            m_vertexBuffer[index+3].s = s + fontWidthNormalised;
            m_vertexBuffer[index+3].t = t + fontHeightNormalised;
            x += 1.0f;
            m_textToVertexBuffer[i] = characterCount++;
        } else if (character == '\n') {
            x = 0.0f;
            y += 1.0f;
            m_textToVertexBuffer[i] = -1;
        } else {
            m_textToVertexBuffer[i] = -1;
        }
    }
}

void BitmapText::updateText(const char* text, int index, int length)
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);
    DASSERT(text);
    DASSERT(index >= 0 && index <= m_textLength);
    DASSERT(index + length <= m_textLength);

    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalised = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalised = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;

    for (int i = index, j = 0; i < index + length; i++, j++) {
        int vertexBufferIndex = m_textToVertexBuffer[i];
        const char character = text[j];
        if (vertexBufferIndex != -1 && character >= 32 && character <= 126) {
            const float s = ((character - 32) % 48) * fontWidthNormalised;
            const float t = (character < 80) ? t1 : t2;
            vertexBufferIndex *= 4;
            m_vertexBuffer[vertexBufferIndex].s = s;
            m_vertexBuffer[vertexBufferIndex].t = t;
            m_vertexBuffer[vertexBufferIndex+1].s = s;
            m_vertexBuffer[vertexBufferIndex+1].t = t + fontHeightNormalised;
            m_vertexBuffer[vertexBufferIndex+2].s = s + fontWidthNormalised;
            m_vertexBuffer[vertexBufferIndex+2].t = t;
            m_vertexBuffer[vertexBufferIndex+3].s = s + fontWidthNormalised;
            m_vertexBuffer[vertexBufferIndex+3].t = t + fontHeightNormalised;
        }
    }
}

void BitmapText::setViewportSize(const QSize& viewportSize)
{
    DLOG_FUNC();

    if (viewportSize != m_viewportSize) {
        m_viewportSize = viewportSize;
        m_flags |= DirtyTransform;
    }
}

void BitmapText::setPosition(const QPointF& position)
{
    DLOG_FUNC();

    if (position != m_position) {
        m_position = position;
        m_flags |= DirtyTransform;
    }
}

void BitmapText::render()
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);

    if (m_flags & NotEmpty) {
        m_functions->glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<char*>(m_vertexBuffer));
        m_functions->glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            reinterpret_cast<char*>(m_vertexBuffer) + (2 * sizeof(float)));
        m_functions->glEnableVertexAttribArray(0);
        m_functions->glEnableVertexAttribArray(1);
        m_functions->glUseProgram(m_program);

        // Update transformation vector. It stores a scale (in (x, y)) and translate
        // (in (z, w)) transform used to put vertices in the right space ((-1, 1),
        // (-1, 1)), at the right position.
        if (m_flags & DirtyTransform) {
            m_transform = QVector4D(
                (2.0f * g_bitmapTextFont.font[m_currentFont].width) / m_viewportSize.width(),
                -(2.0f * g_bitmapTextFont.font[m_currentFont].height) / m_viewportSize.height(),
                ((2.0f * m_position.x()) / m_viewportSize.width()) - 1.0f,
                ((2.0f * -m_position.y()) / m_viewportSize.height()) + 1.0f);
            m_functions->glUniform4fv(
                m_programTransform, 1, reinterpret_cast<const float*>(&m_transform));
            m_flags &= ~DirtyTransform;
        }

        m_functions->glBindTexture(GL_TEXTURE_2D, m_texture);
        m_functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        m_functions->glDisable(GL_DEPTH_TEST);  // QtQuick renderers restore that at each draw call.
        m_functions->glDrawElements(GL_TRIANGLES, 6 * m_characterCount, GL_UNSIGNED_SHORT, 0);
    }
}

// --- GpuTimer ---

#if !defined(QT_OPENGL_ES) && !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF  // For GL_EXT_timer_query.
#endif

bool GPUTimer::initialise()
{
    DLOG_FUNC();
    DASSERT(m_type == None);

#if defined(QT_OPENGL_ES)
    QList<QByteArray> eglExtensions = QByteArray(
        static_cast<const char*>(
            eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');

    // KHRFence.
    if (eglExtensions.contains("EGL_KHR_fence_sync")
        && (glExtensions.contains("GL_OES_EGL_sync")
            || glExtensions.contains("GL_OES_egl_sync") /*PowerVR fix*/)) {
        m_fenceSyncKHR.createSyncKHR = reinterpret_cast<
            EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
                eglGetProcAddress("eglCreateSyncKHR"));
        m_fenceSyncKHR.destroySyncKHR = reinterpret_cast<
            EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
                eglGetProcAddress("eglDestroySyncKHR"));
        m_fenceSyncKHR.clientWaitSyncKHR = reinterpret_cast<
            EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
                eglGetProcAddress("eglClientWaitSyncKHR"));
        m_type = KHRFence;
        DLOG("QuickPlusPerformanceMetrics: GpuTimer is using GL_OES_EGL_sync");
        return true;

    // NVFence.
    } else if (glExtensions.contains("GL_NV_fence")) {
        m_fenceNV.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            eglGetProcAddress("glGenFencesNV"));
        m_fenceNV.deleteFencesNV =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                eglGetProcAddress("glDeleteFencesNV"));
        m_fenceNV.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            eglGetProcAddress("glSetFenceNV"));
        m_fenceNV.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
            eglGetProcAddress("glFinishFenceNV"));
        m_fenceNV.genFencesNV(2, m_fence);
        m_type = NVFence;
        DLOG("QuickPlusPerformanceMetrics: GpuTimer is using GL_NV_fence");
        return true;

    } else {
        m_type = None;
        return false;
    }
#else
    // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack
    // of a method to check the presence of glQueryCounter() would force us to
    // inspect OpenGL version and extensions, which is basically as annoying as
    // doing the whole thing here.
    // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    QSurfaceFormat format = context->format();

    // ARBTimerQuery.
    if (qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
        && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"))) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                context->getProcAddress("glDeleteQueries"));
        m_timerQuery.getQueryObjectui64v =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
                context->getProcAddress("glGetQueryObjectui64v"));
        m_timerQuery.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            context->getProcAddress("glQueryCounter"));
        m_timerQuery.genQueries(2, m_timer);
        m_type = ARBTimerQuery;
        DLOG("QuickPlusPerformanceMetrics: GpuTimer is using GL_ARB_timer_query");
        return true;

    // EXTTimerQuery.
    } else if (context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"))) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                context->getProcAddress("glDeleteQueries"));
        m_timerQuery.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
            context->getProcAddress("glBeginQuery"));
        m_timerQuery.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
            context->getProcAddress("glEndQuery"));
        m_timerQuery.getQueryObjectui64vExt =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
                context->getProcAddress("glGetQueryObjectui64vEXT"));
        m_timerQuery.genQueries(1, m_timer);
        m_type = EXTTimerQuery;
        DLOG("QuickPlusPerformanceMetrics: GpuTimer is using GL_EXT_timer_query");
        return true;

    } else {
        m_type = None;
        return false;
    }
#endif
}

void GPUTimer::finalise()
{
    DLOG_FUNC();
    DASSERT(m_type != None);

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        if (m_beforeSync != EGL_NO_SYNC_KHR) {
            m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), m_beforeSync);
        }
        m_type = None;

    // NVFence.
    } else if (m_type == NVFence) {
        m_fenceNV.deleteFencesNV(2, m_fence);
        m_type = None;
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_timerQuery.deleteQueries(2, m_timer);
        m_type = None;

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_timerQuery.deleteQueries(1, m_timer);
        m_type = None;
    }
#endif
}

void GPUTimer::start()
{
    DLOG_FUNC();
    DASSERT(m_type != None);
    DASSERT(!m_started);

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        m_started = true;
        m_beforeSync = m_fenceSyncKHR.createSyncKHR(
            eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);

    // NVFence.
    } else if (m_type == NVFence) {
        m_started = true;
        m_fenceNV.setFenceNV(m_fence[0], GL_ALL_COMPLETED_NV);
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_started = true;
        m_timerQuery.queryCounter(m_timer[0], GL_TIMESTAMP);

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_started = true;
        m_timerQuery.beginQuery(GL_TIME_ELAPSED, m_timer[0]);
    }
#endif
}

quint64 GPUTimer::stop()
{
    DLOG_FUNC();
    DASSERT(m_type != None);
    DASSERT(m_started);

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        m_started = false;
        QElapsedTimer timer;
        EGLDisplay dpy = eglGetCurrentDisplay();
        EGLSyncKHR afterSync = m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
        EGLint beforeSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(dpy, m_beforeSync, 0, EGL_FOREVER_KHR);
        quint64 beforeTime = timer.nsecsElapsed();
        EGLint afterSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
        quint64 afterTime = timer.nsecsElapsed();
        m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
        m_fenceSyncKHR.destroySyncKHR(dpy, m_beforeSync);
        m_beforeSync = EGL_NO_SYNC_KHR;
        if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
            && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
            return afterTime - beforeTime;
        } else {
            return 0;
        }

    // NVFence.
    } else if (m_type == NVFence) {
        m_started = false;
        QElapsedTimer timer;
        m_fenceNV.setFenceNV(m_fence[1], GL_ALL_COMPLETED_NV);
        m_fenceNV.finishFenceNV(m_fence[0]);
        quint64 beforeTime = timer.nsecsElapsed();
        m_fenceNV.finishFenceNV(m_fence[1]);
        quint64 afterTime = timer.nsecsElapsed();
        return afterTime - beforeTime;
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_started = false;
        GLuint64 time[2] = { 0, 0 };
        m_timerQuery.queryCounter(m_timer[1], GL_TIMESTAMP);
        m_timerQuery.getQueryObjectui64v(m_timer[0], GL_QUERY_RESULT, &time[0]);
        m_timerQuery.getQueryObjectui64v(m_timer[1], GL_QUERY_RESULT, &time[1]);
        if (time[0] != 0 && time[1] != 0) {
            return time[1] - time[0];
        } else {
            return -1;
        }

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_started = false;
        GLuint64EXT time;
        m_timerQuery.endQuery(GL_TIME_ELAPSED);
        m_timerQuery.getQueryObjectui64vExt(m_timer[0], GL_QUERY_RESULT, &time);
        return time;
    }
#endif

    DNOT_REACHED();
    return 0;
}

// --- QuickPlusPerformanceMetrics and PerformanceMetricsPrivate ---

static const char* const defaultOverlayText =
    "CPUs: %cpuUsage%%  Vsz: %vszMemory kB  Rss: %rssMemory kB\n"
    "\n"
    "Frame .....: %frameCount\n"
    "Sync(Cpu) .: %syncTime ms\n"
    "Render(Cpu): %renderTime ms\n"
    "Render(Gpu): %gpuRenderTime ms";

const int maxOverlayTextParsedSize = 1024;  // Including '\0'.

// Keep in sync with maxCounterWidth!
static const struct {
    const char* const name;
    const char* const format;
    quint16 nameSize;
    quint16 width;
} counterInfo[] = {
    { "cpuUsage",      "%3d",   sizeof("cpuUsage") - 1,      3 },
    { "vszMemory",     "%8d",   sizeof("vszMemory") - 1,     8 },
    { "rssMemory",     "%8d",   sizeof("rssMemory") - 1,     8 },
    { "frameCount",    "%7d",   sizeof("frameCount") - 1,    7 },
    { "syncTime",      "%7.2f", sizeof("syncTime") - 1,      7 },
    { "renderTime",    "%7.2f", sizeof("renderTime") - 1,    7 },
    { "gpuRenderTime", "%7.2f", sizeof("gpuRenderTime") - 1, 7 }
};

// Keep in sync with counterInfo!
const int maxCounterWidth = 8;

enum {
    CpuUsage = 0,
    VszMemory,
    RssMemory,
    FrameCount,
    SyncTime,
    RenderTime,
    GpuRenderTime
};

static void connectWindowSignals(QQuickWindow* window, QuickPlusPerformanceMetrics* metrics)
{
    DLOG_FUNC();

    QObject::connect(window, SIGNAL(destroyed(QObject*)), metrics, SLOT(windowDestroyed(QObject*)));
    QObject::connect(window, SIGNAL(widthChanged(int)), metrics, SLOT(windowSizeChanged(int)));
    QObject::connect(window, SIGNAL(heightChanged(int)), metrics, SLOT(windowSizeChanged(int)));
    QObject::connect(window, SIGNAL(sceneGraphInitialized()), metrics,
                     SLOT(windowSceneGraphInitialised()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(sceneGraphInvalidated()), metrics,
                     SLOT(windowSceneGraphInvalidated()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(beforeSynchronizing()), metrics,
                     SLOT(windowBeforeSynchronising()));
    QObject::connect(window, SIGNAL(afterSynchronizing()), metrics,
                     SLOT(windowAfterSynchronising()));
    QObject::connect(window, SIGNAL(beforeRendering()), metrics, SLOT(windowBeforeRendering()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(afterRendering()), metrics, SLOT(windowAfterRendering()),
                     Qt::DirectConnection);
}

static void disconnectWindowSignals(QQuickWindow* window, QuickPlusPerformanceMetrics* metrics)
{
    DLOG_FUNC();

    QObject::disconnect(window, SIGNAL(destroyed(QObject*)), metrics,
                        SLOT(windowDestroyed(QObject*)));
    QObject::disconnect(window, SIGNAL(widthChanged(int)), metrics, SLOT(windowSizeChanged(int)));
    QObject::disconnect(window, SIGNAL(heightChanged(int)), metrics, SLOT(windowSizeChanged(int)));
    QObject::disconnect(window, SIGNAL(sceneGraphInitialized()), metrics,
                        SLOT(windowSceneGraphInitialised()));
    QObject::disconnect(window, SIGNAL(sceneGraphInvalidated()), metrics,
                        SLOT(windowSceneGraphInvalidated()));
    QObject::disconnect(window, SIGNAL(beforeSynchronizing()), metrics,
                        SLOT(windowBeforeSynchronising()));
    QObject::disconnect(window, SIGNAL(afterSynchronizing()), metrics,
                        SLOT(windowAfterSynchronising()));
    QObject::disconnect(window, SIGNAL(beforeRendering()), metrics, SLOT(windowBeforeRendering()));
    QObject::disconnect(window, SIGNAL(afterRendering()), metrics, SLOT(windowAfterRendering()));
}

QuickPlusPerformanceMetrics::QuickPlusPerformanceMetrics(QQuickWindow* window, bool overlayVisible)
    : d_ptr(new PerformanceMetricsPrivate(window, overlayVisible))
{
    DLOG_FUNC();

    if (window) {
        connectWindowSignals(window, this);
    }
}

PerformanceMetricsPrivate::PerformanceMetricsPrivate(QQuickWindow* window, bool overlayVisible)
    : m_window(window)
    , m_loggingDevice(Q_NULLPTR)
    , m_overlayTextParsed(new char [maxOverlayTextParsedSize])
    , m_overlayIndicesSize(0)
    , m_overlayText(defaultOverlayText)
    , m_defaultLoggingDevice()
    , m_bitmapText()
    , m_gpuTimer()
    , m_syncTimer()
    , m_renderTimer()
    , m_flags(DirtyText | DirtySize | (overlayVisible ? OverlayVisible : 0))
{
    DLOG_FUNC();

    parseOverlayText();
    m_defaultLoggingDevice.open(stdout, QIODevice::WriteOnly);
    m_cpuOnlineCores = sysconf(_SC_NPROCESSORS_ONLN);
    m_pageSize = sysconf(_SC_PAGESIZE);
    m_cpuTimer.start();
    m_cpuTicks = times(&m_cpuTimes);
}

QuickPlusPerformanceMetrics::~QuickPlusPerformanceMetrics()
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    if (d->m_window) {
        disconnectWindowSignals(d->m_window, this);
    }
    delete d_ptr;
}

PerformanceMetricsPrivate::~PerformanceMetricsPrivate()
{
    DLOG_FUNC();
    delete [] m_overlayTextParsed;
}

void QuickPlusPerformanceMetrics::setWindow(QQuickWindow* window)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    if (d->m_window != window) {
        if (d->m_window) {
            disconnectWindowSignals(d->m_window, this);
        }
        if (window) {
            connectWindowSignals(window, this);
        }
        d->m_window = window;
    }
}

QQuickWindow* QuickPlusPerformanceMetrics::window() const
{
    DLOG_FUNC();

    return d_func()->m_window;
}

void QuickPlusPerformanceMetrics::setOverlayText(const QString& text)
{
    DLOG_FUNC();

    d_func()->setOverlayText(text);
}

void PerformanceMetricsPrivate::setOverlayText(const QString& text)
{
    DLOG_FUNC();

    QMutexLocker locker(&m_mutex);
    if (text != m_overlayText) {
        m_overlayText = text;
        parseOverlayText();
        m_flags |= DirtyText;
    }
}

const QString& QuickPlusPerformanceMetrics::overlayText() const
{
    DLOG_FUNC();

    return d_func()->m_overlayText;
}

void QuickPlusPerformanceMetrics::setOverlayVisible(bool visible)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    if (visible) {
        d->m_flags |= PerformanceMetricsPrivate::OverlayVisible;
    } else {
        d->m_flags &= ~PerformanceMetricsPrivate::OverlayVisible;
    }
}

bool QuickPlusPerformanceMetrics::overlayVisible()
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    return d->m_flags & PerformanceMetricsPrivate::OverlayVisible ? true : false;
}

void QuickPlusPerformanceMetrics::setWindowUpdatePolicy(UpdatePolicy updatePolicy)
{
    DLOG_FUNC();

    d_func()->setWindowUpdatePolicy(updatePolicy);
}

void PerformanceMetricsPrivate::setWindowUpdatePolicy(
    QuickPlusPerformanceMetrics::UpdatePolicy updatePolicy)
{
    DLOG_FUNC();

    QMutexLocker locker(&m_mutex);
    const QuickPlusPerformanceMetrics::UpdatePolicy policy = (m_flags & ContinuousUpdate) ?
        QuickPlusPerformanceMetrics::Continuous : QuickPlusPerformanceMetrics::Live;
    if (updatePolicy != policy) {
        if (updatePolicy == QuickPlusPerformanceMetrics::Continuous) {
            if (m_window) {
                m_window->update();
            }
            m_flags |= ContinuousUpdate;
        } else {
            m_flags &= ~ContinuousUpdate;
        }
    }
}

QuickPlusPerformanceMetrics::UpdatePolicy QuickPlusPerformanceMetrics::windowUpdatePolicy()
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    return d->m_flags & PerformanceMetricsPrivate::ContinuousUpdate ? Continuous : Live;
}

void QuickPlusPerformanceMetrics::setLoggingDevice(QIODevice* loggingDevice)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    if (loggingDevice != d->m_loggingDevice) {
        if (loggingDevice && loggingDevice->isWritable()) {
            d->m_loggingDevice = loggingDevice;
        } else {
            d->m_loggingDevice = Q_NULLPTR;
        }
    }
}

QIODevice* QuickPlusPerformanceMetrics::loggingDevice() const
{
    DLOG_FUNC();

    return d_func()->m_loggingDevice;
}

void QuickPlusPerformanceMetrics::setLogging(bool logging)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    if (logging) {
        d->m_flags |= PerformanceMetricsPrivate::Logging;
    } else {
        d->m_flags &= ~PerformanceMetricsPrivate::Logging;
    }
}

bool QuickPlusPerformanceMetrics::logging()
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    return !!(d->m_flags & PerformanceMetricsPrivate::Logging);
}

void QuickPlusPerformanceMetrics::windowDestroyed(QObject*)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);

    QMutexLocker locker(&d->m_mutex);
    d->m_window = Q_NULLPTR;
}

void QuickPlusPerformanceMetrics::windowSizeChanged(int)
{
    DLOG_FUNC();
    Q_D(PerformanceMetrics);
    DASSERT(d->m_window);

    QMutexLocker locker(&d->m_mutex);
    d->m_flags |= PerformanceMetricsPrivate::DirtySize;
}

void QuickPlusPerformanceMetrics::windowSceneGraphInitialised()
{
    DLOG_FUNC();

    d_func()->windowSceneGraphInitialised();
}

void PerformanceMetricsPrivate::windowSceneGraphInitialised()
{
    DLOG_FUNC();
    DASSERT(m_window);
    DASSERT(!(m_flags & Initialised));

    QMutexLocker locker(&m_mutex);
    m_bitmapText.initialise();
    m_counters.frameCount = 0;
    const quint8 flags = Initialised | DirtyText | DirtySize;
    m_flags |= m_gpuTimer.initialise() ? (flags | GpuTimerAvailable) : flags;
}

void QuickPlusPerformanceMetrics::windowSceneGraphInvalidated()
{
    DLOG_FUNC();

    d_func()->windowSceneGraphInvalidated();
}

void PerformanceMetricsPrivate::windowSceneGraphInvalidated()
{
    DLOG_FUNC();
    DASSERT(m_window);

    QMutexLocker locker(&m_mutex);
    if (m_flags & Initialised) {
        m_bitmapText.finalise();
        if (m_flags & GpuTimerAvailable) {
            m_gpuTimer.finalise();
        }
        m_flags &= ~(Initialised | GpuTimerAvailable);
    }
}

void QuickPlusPerformanceMetrics::windowBeforeSynchronising()
{
    DLOG_FUNC();

    d_func()->windowBeforeSynchronising();
}

void PerformanceMetricsPrivate::windowBeforeSynchronising()
{
    DLOG_FUNC();
    DASSERT(m_window);

    QMutexLocker locker(&m_mutex);
    if (m_flags & Initialised) {
        m_syncTimer.start();
    }
}

void QuickPlusPerformanceMetrics::windowAfterSynchronising()
{
    DLOG_FUNC();

    d_func()->windowAfterSynchronising();
}

void PerformanceMetricsPrivate::windowAfterSynchronising()
{
    DLOG_FUNC();
    DASSERT(m_window);

    QMutexLocker locker(&m_mutex);
    if (m_flags & Initialised) {
        m_counters.syncTime = m_syncTimer.nsecsElapsed();
    }
}

void QuickPlusPerformanceMetrics::windowBeforeRendering()
{
    DLOG_FUNC();

    d_func()->windowBeforeRendering();
}

void PerformanceMetricsPrivate::windowBeforeRendering()
{
    DLOG_FUNC();
    DASSERT(m_window);

    QMutexLocker locker(&m_mutex);
    if (m_flags & Initialised) {
        m_renderTimer.start();
        if (m_flags & GpuTimerAvailable) {
            m_gpuTimer.start();
        }
    }
}

void QuickPlusPerformanceMetrics::windowAfterRendering()
{
    DLOG_FUNC();

    d_func()->windowAfterRendering();
}

void PerformanceMetricsPrivate::windowAfterRendering()
{
    DLOG_FUNC();
    DASSERT(m_window);

    m_mutex.lock();

    if (m_flags & Initialised) {
        // Update counters.
        m_counters.renderTime = m_renderTimer.nsecsElapsed();
        m_counters.gpuRenderTime = (m_flags & GpuTimerAvailable) ? m_gpuTimer.stop() : 0;
        m_counters.frameCount++;
        updateCpuUsage();
        updateMemoryUsage();

        // Update and render overlay.
        if (m_flags & OverlayVisible) {
            if (m_flags & DirtySize) {
                m_bitmapText.setViewportSize(m_window->size());
                m_flags &= ~DirtySize;
            }
            if (m_flags & DirtyText) {
                m_bitmapText.setText(m_overlayTextParsed);
                m_flags &= ~DirtyText;
            }
            updateOverlayText();
            m_bitmapText.render();
        }

        // Logging.
        // FIXME(loicm) Use a dedicated I/O thread.
        if (m_flags & Logging ) {
            QTextStream stream(m_loggingDevice ? m_loggingDevice : &m_defaultLoggingDevice);
            stream << m_counters.frameCount << ' '
                   << m_counters.syncTime << ' '
                   << m_counters.renderTime << ' '
                   << m_counters.gpuRenderTime << ' '
                   << m_counters.cpuUsage << ' '
                   << m_counters.vszMemory << ' '
                   << m_counters.rssMemory << '\n';
        }

        // Queue another update if required.
        if (m_flags & ContinuousUpdate) {
            m_window->update();
        }

        m_mutex.unlock();

    } else {
        // Get everything ready for the next frame.
        m_mutex.unlock();
        windowSceneGraphInitialised();
    }
}

void PerformanceMetricsPrivate::updateOverlayText()
{
    DLOG_FUNC();
    DASSERT(m_flags & Initialised);

    char buffer[maxCounterWidth + 1];
    for (int i = 0; i < m_overlayIndicesSize; i++) {
        const char* const format = counterInfo[m_overlayIndices[i].counterIndex].format;
        const int width = counterInfo[m_overlayIndices[i].counterIndex].width;
        DASSERT(width <= maxCounterWidth);

        switch (m_overlayIndices[i].counterIndex) {
        case CpuUsage:
            snprintf(buffer, width + 1, format, m_counters.cpuUsage);
            break;
        case VszMemory:
            snprintf(buffer, width + 1, format, m_counters.vszMemory);
            break;
        case RssMemory:
            snprintf(buffer, width + 1, format, m_counters.rssMemory);
            break;
        case FrameCount:
            snprintf(buffer, width + 1, format, m_counters.frameCount);
            break;
        case SyncTime:
            snprintf(buffer, width + 1, format, m_counters.syncTime * 0.000001f);
            break;
        case RenderTime:
            snprintf(buffer, width + 1, format, m_counters.renderTime * 0.000001f);
            break;
        case GpuRenderTime:
            if (m_flags & GpuTimerAvailable) {
                snprintf(buffer, width + 1, format, m_counters.gpuRenderTime * 0.000001f);
            } else {
                strncpy(buffer, "    N/A", width);
            }
            break;
        default:
            DNOT_REACHED();
            break;
        }

        m_bitmapText.updateText(buffer, m_overlayIndices[i].overlayTextParsedIndex, width);
    }
}

void PerformanceMetricsPrivate::parseOverlayText()
{
    DLOG_FUNC();

    const int counterInfoSize = ARRAY_SIZE(counterInfo);
    QByteArray overlayTextLatin1 = m_overlayText.toLatin1();
    const char* const overlayText = overlayTextLatin1.constData();
    const int overlayTextSize = overlayTextLatin1.size();
    int counters = 0;
    int characters = 0;

    for (int i = 0; i <= overlayTextSize; i++) {
        const char character = overlayText[i];
        if (character != '%') {
            m_overlayTextParsed[characters++] = character;
        } else if (overlayText[i+1] == '%') {
            m_overlayTextParsed[characters++] = '%';
            i++;
        } else if (counters < maxOverlayIndices) {
            for (int j = 0; j < counterInfoSize; j++) {
                if ((counterInfo[j].width < maxOverlayTextParsedSize - characters)
                    && !strncmp(&overlayText[i+1], counterInfo[j].name, counterInfo[j].nameSize)) {
                    m_overlayIndices[counters].counterIndex = j;
                    m_overlayIndices[counters].overlayTextParsedIndex = characters;
                    // Must be initialised since it might contain non-printable
                    // and break setText otherwise.
                    memset(&m_overlayTextParsed[characters], '?', counterInfo[j].width);
                    characters += counterInfo[j].width;
                    i += counterInfo[j].nameSize;
                    counters++;
                    break;
                }
            }
        }
        if (characters >= (maxOverlayTextParsedSize - 1)) {
            m_overlayTextParsed[maxOverlayTextParsedSize - 1] = '\0';
            break;
        }
    }

    m_overlayIndicesSize = counters;
}

void PerformanceMetricsPrivate::updateCpuUsage()
{
    DLOG_FUNC();

    // times() is a Linux syscall giving CPU times used by the process. The
    // granularity of the unit returned by the (some?) kernel (clock ticks)
    // prevents us from getting precise timings at a high frequency, so we have
    // to throttle to 200 ms (5 Hz).
    const qint64 throttlingFrequency = 200;
    if (m_cpuTimer.elapsed() > throttlingFrequency) {
        struct tms newCpuTimes;
        const clock_t newTicks = times(&newCpuTimes);
        const clock_t ticks = newTicks - m_cpuTicks;
        const clock_t userTime = newCpuTimes.tms_utime - m_cpuTimes.tms_utime;
        const clock_t systemTime = newCpuTimes.tms_stime - m_cpuTimes.tms_stime;
        m_counters.cpuUsage = ((userTime + systemTime) * 100) / (ticks * m_cpuOnlineCores);
        m_cpuTimer.start();
        memcpy(&m_cpuTimes, &newCpuTimes, sizeof(struct tms));
        m_cpuTicks = newTicks;
    }
}

void PerformanceMetricsPrivate::updateMemoryUsage()
{
    DLOG_FUNC();

    unsigned long vsz, rss;

    FILE* file = fopen("/proc/self/statm", "r");
    if (!file) {
        DWARN("QuickPlusPerformanceMetrics: can't open '/proc/self/statm'");
        return;
    }
    if (fscanf(file, "%lu %lu", &vsz, &rss) != 2) {
        DWARN("QuickPlusPerformanceMetrics: can't read '/proc/self/statm'");
        fclose(file);
        return;
    }
    fclose(file);

    m_counters.vszMemory = (vsz * m_pageSize) / 1024;
    m_counters.rssMemory = (rss * m_pageSize) / 1024;
}