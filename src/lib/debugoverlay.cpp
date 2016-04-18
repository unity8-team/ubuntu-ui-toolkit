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

#include "debugoverlay.h"
#include "bitmaptextfont_p.h"
#include <unistd.h>

#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif

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

const int defaultFontSize = 12;

BitmapText::BitmapText()
    : m_functions(nullptr)
    , m_vertexBuffer(nullptr)
    , m_textToVertexBuffer(nullptr)
    , m_viewportSize(1, 1)
    , m_position(0.0f, 0.0f)
    , m_transform()
    , m_textLength(0)
    , m_characterCount(0)
    , m_flags(DirtyTransform)
{
    // Set current font based on requested font size.
    // FIXME(loicm) Define font size based on the grid unit.
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
    delete [] m_vertexBuffer;
    delete [] m_textToVertexBuffer;
}

static GLuint createProgram(QOpenGLFunctions* functions, const char* vertexShaderSource,
                            const char* fragmentShaderSource, GLuint* vertexShaderObject,
                            GLuint* fragmentShaderObject)
{
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;

    vertexShader = functions->glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = functions->glCreateShader(GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragmentShader == 0) {
        qWarning() << "BitmapText: glCreateShader() failed (OpenGL error: " << glGetError() << ")";
        return 0;
    }

    functions->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    functions->glCompileShader(vertexShader);
    functions->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[2048];
        functions->glGetShaderInfoLog(vertexShader, 2048, nullptr, infoLog);
        qWarning() << "BitmapText: vertex shader compilation failed:\n" << infoLog;
        return 0;
    }

    functions->glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    functions->glCompileShader(fragmentShader);
    functions->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[2048];
        functions->glGetShaderInfoLog(fragmentShader, 2048, nullptr, infoLog);
        qWarning() << "BitmapText: fragment shader compilation failed:\n" << infoLog;
        return 0;
    }

    program = functions->glCreateProgram();
    functions->glAttachShader(program, vertexShader);
    functions->glAttachShader(program, fragmentShader);
    functions->glLinkProgram(program);
    functions->glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char infoLog[2048];
        functions->glGetProgramInfoLog(program, 2048, nullptr, infoLog);
        qWarning() << "BitmapText: shader linking failed:\n" << infoLog;
        return 0;
    }

    *vertexShaderObject = vertexShader;
    *fragmentShaderObject = fragmentShader;

    return program;
}

bool BitmapText::initialise()
{
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

    m_functions = nullptr;
    m_flags &= ~Initialised;
}

void BitmapText::setText(const char* text)
{
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
        m_vertexBuffer = nullptr;
        m_textToVertexBuffer = nullptr;
        m_textLength = 0;
        m_characterCount = 0;
        m_flags &= ~NotEmpty;
        return;
    }

    // Fill the index buffer. The GL_TRIANGLES primitive mode requires 3 indices
    // per triangle, so 6 per character.
    GLushort* indices = new GLushort [6 * characterCount];
    for (int i = 0; i < maxCharacters; i++) {
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
    m_textToVertexBuffer = new char [textLength];
    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalised = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalised = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;
    float x = 0.0f;
    float y = 0.0f;
    int characterCount = 0;
    for (int i = 0; i < textLength; i++) {
        char character = text[i];
        if (character >= 32 && character <= 126) {  // Printable characters.
            const int index = characterCount * 4;
            // The atlas stores 2 lines per font size, second line starts at
            // character 80.
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
    Q_ASSERT(text);
    Q_ASSERT(index >= 0 && index <= m_textLength);
    Q_ASSERT(index + length <= m_textLength);

    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalised = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalised = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;

    for (int i = index, j = 0; i < length; i++, j++) {
        int vertexBufferIndex = m_textToVertexBuffer[j];
        const char character = text[j];
        if (vertexBufferIndex != -1 && character >= 32 && character <= 126) {
            // The atlas stores 2 lines per font size, second line starts at
            // character 80.
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
    if (viewportSize != m_viewportSize) {
        m_viewportSize = viewportSize;
        m_flags |= DirtyTransform;
    }
}

void BitmapText::setPosition(const QPointF& position)
{
    if (position != m_position) {
        m_position = position;
        m_flags |= DirtyTransform;
    }
}

void BitmapText::render()
{
    // Update transformation vector. It stores a scale (in (x, y)) and translate
    // (in (z, w)) transform used to put vertices in the right space ((-1, 1),
    // (-1, 1)), at the right position.
    if (m_flags & DirtyTransform) {
        m_transform = QVector4D(
            (2.0f * g_bitmapTextFont.font[m_currentFont].width) / m_viewportSize.width(),
            -(2.0f * g_bitmapTextFont.font[m_currentFont].height) / m_viewportSize.height(),
            ((2.0f * m_position.x()) / m_viewportSize.width()) - 1.0f,
            ((2.0f * -m_position.y()) / m_viewportSize.height()) + 1.0f);
        m_flags &= ~DirtyTransform;
    }

    if ((m_flags & (Initialised | NotEmpty)) == (Initialised | NotEmpty)) {
        m_functions->glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<char*>(m_vertexBuffer));
        m_functions->glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            reinterpret_cast<char*>(m_vertexBuffer) + (2 * sizeof(float)));
        m_functions->glEnableVertexAttribArray(0);
        m_functions->glEnableVertexAttribArray(1);
        m_functions->glUseProgram(m_program);
        m_functions->glUniform4fv(
            m_programTransform, 1, reinterpret_cast<const float*>(&m_transform));
        m_functions->glBindTexture(GL_TEXTURE_2D, m_texture);
        m_functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        m_functions->glDisable(GL_DEPTH_TEST);  // QtQuick renderers restore that at each draw call.
        m_functions->glDrawElements(GL_TRIANGLES, 6 * m_characterCount, GL_UNSIGNED_SHORT, 0);
    } else {
        qWarning() << "BitmapText: render() called while not initialised";
    }
}

bool GPUTimer::initialise()
{
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
        return true;

    } else {
        m_type = NotInitialised;
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
        return true;

    } else {
        m_type = NotInitialised;
        return false;
    }
#endif
}

void GPUTimer::finalise()
{
#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        if (m_beforeSync != EGL_NO_SYNC_KHR) {
            m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), m_beforeSync);
        }
        m_type = NotInitialised;

    // NVFence.
    } else if (m_type == NVFence) {
        m_fenceNV.deleteFencesNV(2, m_fence);
        m_type = NotInitialised;
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_timerQuery.deleteQueries(2, m_timer);
        m_type = NotInitialised;

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_timerQuery.deleteQueries(1, m_timer);
        m_type = NotInitialised;
    }
#endif
    else {
        qWarning() << "GPUTimer: finalise() called while not initialised.";
    }
}

void GPUTimer::start()
{
    if (m_started) {
        qWarning() << "GPUTimer: start() called while not stopped.";
        return;
    }

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
    else {
        qWarning() << "GPUTimer: start() called while not initialised.";
    }
}

qint64 GPUTimer::stop()
{
    if (!m_started) {
        qWarning() << "GPUTimer: stop() called while not started.";
        return 0;
    }

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        m_started = false;
        QElapsedTimer timer;
        EGLDisplay dpy = eglGetCurrentDisplay();
        EGLSyncKHR afterSync = m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
        EGLint beforeSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(dpy, m_beforeSync, 0, EGL_FOREVER_KHR);
        qint64 beforeTime = timer.nsecsElapsed();
        EGLint afterSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
        qint64 afterTime = timer.nsecsElapsed();
        m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
        m_fenceSyncKHR.destroySyncKHR(dpy, m_beforeSync);
        m_beforeSync = EGL_NO_SYNC_KHR;
        if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
            && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
            return afterTime - beforeTime;
        } else {
            return -1;
        }

    // NVFence.
    } else if (m_type == NVFence) {
        m_started = false;
        QElapsedTimer timer;
        m_fenceNV.setFenceNV(m_fence[1], GL_ALL_COMPLETED_NV);
        m_fenceNV.finishFenceNV(m_fence[0]);
        qint64 beforeTime = timer.nsecsElapsed();
        m_fenceNV.finishFenceNV(m_fence[1]);
        qint64 afterTime = timer.nsecsElapsed();
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
        return static_cast<qint64>(time);
    }
#endif

    else {
        qWarning() << "GPUTimer: stop() called while not initialised.";
        return 0;
    }
}

// static const char* defaultText =
//     "CPUs: %cpuUsage %% / Mem(Vsz): %vszMemory Mb / Mem(Rss): %rssMemory Mb\n"
//     "\n"
//     "Frame ..... %frameCount\n"
//     "Sync(Cpu) . %syncTime ms\n"
//     "Render(Cpu) %renderTime ms\n"
//     "Render(Gpu) %gpuRenderTime ms";

// static const struct { char* name; char* format; int width; } counterTypes = {
//     { "cpuUsage",      "%3d",   3 },
//     { "vszMemory",     "%4d",   4 },
//     { "rssMemory",     "%4d",   4 },
//     { "frameCount",    "%7d",   7 },
//     { "syncTime",      "%7.2f", 7 },
//     { "renderTime",    "%7.2f", 7 },
//     { "gpuRenderTime", "%7.2f", 7 }
// };

// const int maxTextSize = 2048;
// const int maxCounterCount = 16;


// // static void buildText()
// // {
// //     // 
// //     const void (*updateFuncs)() = {
        
// //     };
// //     for (int i = 0; i < FormatTypeCount; i++) {
// //         if (data.updateFlags & (1 << i)) {
            
// //         }
// //     }

// //     // 
// //     for (int i = 0, j = 0; i < formatCount; i++) {
// //         j += snprintf(
// //             &data.outputString[j], data.formats[i].size, &data.string[data.formats[i].index],
// //             );
// //     }
// // }

// // int main(int argc, char* argv[])
// // {
// //     Q_UNUSED(argc);
// //     Q_UNUSED(argv);

// //     QFile file("format.txt");
// //     if (!file.open(QFile::ReadOnly)) {
// //         qWarning() << "can't open file";
// //         return 1;
// //     }
// //     QTextStream stream(&file);
// //     stream.setCodec("ISO 8859-1");

// //     QString inputData = stream.readAll();
// //     const int inputDataSize = inputData.size();
// //     char textBuffer = new char[inputDataSize];
// //     int textIndex = 0;
// //     QStringList textList;
// //     bool isFormat = false;
// //     bool isFormatInvalid = true;

// //     for (int i = 0; i < inputDataSize; i++) {
// //         const char character = inputData[i].toLatin1();
// //         if (character != '%') {
// //             textBuffer[textIndex++] = character;
// //         } else {
// //             readFormat();
// //         }
// //     }

// //     delete [] currentTextBuffer;

// //     // printf("#%7.2f#\n", 3.1415);

// //     return 0;
// // }

//     //     if (!isFormatInvalid) {
//     //         if (!isFormat) {
//     //             if (character == '%') {
//     //                 isFormat = true;
//     //             } else {
//     //                 textBuffer[textIndex++] = character;
//     //             }
//     //         } else {
//     //             if (character == '%') {
//     //                 isFormatInvalid = true;
//     //             }
//     //         }

//     //     } else {
//     //         if (character == '}') {
//     //             formatIsInvalid = false;
//     //         }
//     //     }
//     // }

// DebugOverlay::DebugOverlay(QQuickWindow* window)
//     : QObject(window)
//     , m_text(nullptr)
//     , m_counters(nullptr)
//     , m_textSize(0)
//     , m_countersSize(0)
//     , m_updateFlags(0)
//     , m_bitmapText()
//     , m_gpuTimer()
//     , m_cpuTimer()
//     , m_cpuUsage(0)
//     , m_vszMemory(0)
//     , m_rssMemory(0)
//     , m_frameCount(0)
//     , m_syncTimer()
//     , m_renderTimer()
//     , m_syncTime(0.0f)
//     , m_renderTime(0.0f)
//     , m_gpuRenderTime(0.0f)
// {
//     qDebug() << "DebugOverlay::DebugOverlay";
//     Q_ASSERT(window);

//     // 
    

//     connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(windowDestroyed(QObject*)));
//     connect(window, SIGNAL(widthChanged(int)), this, SLOT(windowSizeChanged(int)));
//     connect(window, SIGNAL(heightChanged(int)), this, SLOT(windowSizeChanged(int)));
//     connect(window, SIGNAL(sceneGraphInitialized()), this, SLOT(windowSceneGraphInitialised()),
//             Qt::DirectConnection);
//     connect(window, SIGNAL(sceneGraphInvalidated()), this, SLOT(windowSceneGraphInvalidated()),
//             Qt::DirectConnection);
//     connect(window, SIGNAL(beforeSynchronizing()), this, SLOT(windowBeforeSynchronising()));
//     connect(window, SIGNAL(afterSynchronizing()), this, SLOT(windowAfterSynchronising()));
//     connect(window, SIGNAL(beforeRendering()), this, SLOT(windowBeforeRendering()),
//             Qt::DirectConnection);
//     connect(window, SIGNAL(afterRendering()), this, SLOT(windowAfterRendering()),
//             Qt::DirectConnection);

//     m_cpuTimer.start();
//     m_cpuTimingFactor = 100.0f / sysconf(_SC_NPROCESSORS_ONLN);
//     m_cpuTicks = times(&m_cpuTimes);
// }

// DebugOverlay::~DebugOverlay()
// {
//     qDebug() << "DebugOverlay::~DebugOverlay";
// }

// void DebugOverlay::windowDestroyed(QObject*)
// {
//     qDebug() << "DebugOverlay::windowDestroyed";
// }

// void DebugOverlay::windowSizeChanged(int)
// {
//     m_bitmapText.setViewportSize(size());
// }

// void DebugOverlay::windowSceneGraphInitialised()
// {
//     m_bitmapText.initialise();
//     m_gpuTimer.initialise();
// }

// void DebugOverlay::windowSceneGraphInvalidated()
// {
//     m_bitmapText.finalise();
//     m_gpuTimer.finalise();
// }

// void DebugOverlay::windowBeforeSynchronizing()
// {
//     m_syncTimer.start();
// }

// void DebugOverlay::windowAfterSynchronizing()
// {
//     m_syncTime = static_cast<float>(m_syncTimer.elapsed());
// }

// void DebugOverlay::windowBeforeRendering()
// {
//     m_renderTimer.start();
//     m_gpuTimer.start();
// }

// void DebugOverlay::windowAfterRendering()
// {
//     m_renderTime = static_cast<float>(m_renderTimer.restart());
//     m_gpuRenderTime = m_gpuTimer.stop() * 0.000001f;

//     // m_text.clear();
//     // m_textStream << "Frame " << ++frame << "\n";
//     // m_textStream << "Render: " << qSetFieldWidth(6) << time << qSetFieldWidth(0) << " ms";
//     m_bitmapText.updateText();
//     m_bitmapText.render();
// }

// bool DebugOverlay::parseText(const char* text)
// {
    
// }

// // FIXME(loicm) Get higher definition timings for each CPU core.
// void DebugOverlay::updateCpuUsage()
// {
//     // times() is a Linux syscall giving CPU times used by the process. The
//     // granularity of the unit returned by the kernel (clock ticks) prevents us
//     // from getting precise timings at a high frequency, so we have to throttle
//     // to 200 ms (5 Hz).
//     const qint64 throttlingFrequency = 200;
//     if (m_cpuTimer.elapsed() > throttlingFrequency) {
//         struct tms newTimes;
//         clock_t newTicks = times(&newTimes);
//         m_cpuUsage =
//             ((newTimes.tms_utime + newTimes.tms_stime - m_times.tms_utime - m_times.tms_stime)
//              / static_cast<float>(newTicks - m_cpuTicks)) * m_cpuTimingFactor;
//         m_cpuTimer.start();
//         memcpy(&m_cpuTimes, &newTimes, sizeof(struct tms));
//         m_cpuTicks = newTicks;
//     }
// }

// void DebugOverlay::updateMemory()
// {
    
// }

// void DebugOverlay::updateFrameCount()
// {
//     m_frameCount++;
// }
