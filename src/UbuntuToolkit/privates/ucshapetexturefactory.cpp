/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#include "ucshapetexturefactory_p.h"

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

#define SHADOW_TEXTURE_DUMP_CHANNEL0 0
#define SHADOW_TEXTURE_DUMP_CHANNEL1 0
#define SHADOW_TEXTURE_DUMP_PERF     0
#if SHADOW_TEXTURE_DUMP_PERF
#include <QtCore/QElapsedTimer>
#endif

// FIXME(loicm) Add a define to log ref counting info.

// We use explicit template instantiation for UCShapeTextureFactory so that we
// can separate the implementation from the header. The drawback is that we have
// to keep it in sync with the shape items.
template class UCShapeTextureFactory<1>;  // Used by UCCornerMaterial (fill.[h,cpp]).
template class UCShapeTextureFactory<2>;  // Used by UCFrameCornerMaterial (frame.[h,cpp]).

// Reference counted wrapper for key hashed textures.
class KeyHash
{
public:
    KeyHash() : m_data(new QHash<quint32, UCShapeTexture>()), m_refCount(0) {}

    QHash<quint32, UCShapeTexture>* ref() {
        DASSERT(m_refCount < UINT_MAX);
        m_refCount++;
        return m_data;
    }
    quint32 unref() {
        DASSERT(m_refCount > 0);
        if (Q_UNLIKELY(--m_refCount == 0)) { DASSERT(m_data->empty()); delete m_data; }
        return m_refCount;
    }
#if !defined(QT_NO_DEBUG)
    QHash<quint32, UCShapeTexture>* data() const { return m_data; }
#endif

private:
    QHash<quint32, UCShapeTexture>* m_data;
    quint32 m_refCount;
};

static QHash<QOpenGLContext*, KeyHash> contextHash;
static QMutex contextHashMutex;

template <int N>
UCShapeTextureFactory<N>::UCShapeTextureFactory()
    : m_context(QOpenGLContext::currentContext())
{
    DASSERT(m_context);

    // Ref the texture hash associated with the current context.
    contextHashMutex.lock();
    m_keyHash = contextHash[m_context].ref();
    contextHashMutex.unlock();

    for (int i = 0; i < N; i++) {
        m_keys[i] = invalidKey;
    }
}

template <int N>
UCShapeTextureFactory<N>::~UCShapeTextureFactory()
{
    DASSERT(QOpenGLContext::currentContext() == m_context);

    // Unref current textures.
    int textureCount = 0;
    quint32 textureIds[N];
    for (int i = 0; i < N; i++) {
        auto it = m_keyHash->find(m_keys[i]);
        if (it != m_keyHash->end()) {
            if (it.value().unref() == 0) {
                textureIds[textureCount++] = it.value().id();
                m_keyHash->erase(it);
            }
        }
    }
    if (textureCount > 0) {
        m_context->functions()->glDeleteTextures(textureCount, textureIds);
    };

    // Unref the texture hash associated with the current context.
    contextHashMutex.lock();
    auto it = contextHash.find(m_context);
    DASSERT(it.value().data() == m_keyHash);
    if (it.value().unref() == 0) {
        contextHash.erase(it);
    }
    contextHashMutex.unlock();
}

template <int N>
quint32 UCShapeTextureFactory<N>::acquireTexture(
    int index, quint32 currentKey, quint32 newKey, bool* needsUpdate, bool* isNewTexture)
{
    DASSERT(index >= 0 && index < N);
    DASSERT(needsUpdate);
    DASSERT(isNewTexture);

    m_keys[index] = newKey;
    auto newIt = m_keyHash->find(newKey);
    auto currentIt = m_keyHash->find(currentKey);

    if (currentIt != m_keyHash->end() && currentIt.value().unref() == 0) {
        quint32 textureId = currentIt.value().id();
        m_keyHash->erase(currentIt);
        if (newIt == m_keyHash->end()) {
            m_keyHash->insert(newKey, UCShapeTexture(textureId));
            *needsUpdate = true;
            *isNewTexture = false;
            return textureId;
        } else {
            m_context->functions()->glDeleteTextures(1, &textureId);
            *needsUpdate = false;
            *isNewTexture = false;
            return newIt.value().ref();
        }
    } else {
        if (newIt == m_keyHash->end()) {
            quint32 textureId;
            m_context->functions()->glGenTextures(1, &textureId);
            m_keyHash->insert(newKey, UCShapeTexture(textureId));
            *needsUpdate = true;
            *isNewTexture = true;
            return textureId;
        } else {
            *needsUpdate = false;
            *isNewTexture = false;
            return newIt.value().ref();
        }
    }
}

// static
template <int N>
void UCShapeTextureFactory<N>::renderShape(void* buffer, UCShapeType type, int radius, int stride)
{
    DASSERT(buffer);
    DASSERT(radius > 0);
    DASSERT(stride > 0);
    DASSERT((stride & 0x3) == 0);  // Ensure 32-bit alignment required by QImage.

    QImage image(static_cast<quint8*>(buffer), radius, radius, stride,
                 QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);

    if (type == UCShapeType::Squircle) {
        // QSvgRenderer being reentrant, we use a static instance with local data.
        static QSvgRenderer svg(QByteArray(squircleSvg), 0);
        svg.render(&painter);
    } else {
        painter.setBrush(Qt::white);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // Offsetting by 0.5 provides the best looking anti-aliasing.
        painter.drawEllipse(QRectF(-0.5, -0.5, radius * 2.0 + 0.5, radius * 2.0 + 0.5));
    }
}

// static
template <int N>
quint8* UCShapeTextureFactory<N>::renderMaskTexture(UCShapeType type, int radius)
{
    //  >────────────────< Texture size
    //      >─<        >─< Borders (1 pixel each)
    //  ┌────────────────┐
    //  │ Texture offset │
    //  │   ┌────────────┤
    //  │   │ ┌────────┐ │
    //  │   │ │        │ │
    //  │   │ │ Radius │ │
    //  │   │ │        │ │
    //  │   │ └────────┘ │
    //  └───┴────────────┘

    DASSERT(radius >= 0);

    const int border = 1;  // 1 pixel border around the edges for clamping reasons.
    // FIXME(loicm) Looks creepy (and the 2nd getStride is most likely useless
    //     since textureStride is 32).
    const int width = getStride(radius + 2 * border, 1, textureStride);
    const int stride = getStride(width, 1, 4);  // OpenGL unpack pixel storage is 4 by default.
    const int height = width;
    const int finalBufferSize = stride * height * sizeof(quint8);
    const int painterBufferSize = radius * radius * sizeof(quint32);
    quint8* __restrict bufferU8 = reinterpret_cast<quint8*>(
        alignedAlloc(32, finalBufferSize + painterBufferSize));
    quint32* __restrict painterBufferU32 = reinterpret_cast<quint32*>(&bufferU8[finalBufferSize]);

    if (radius > 0) {
        // Render the shape with QPainter.
        memset(painterBufferU32, 0, painterBufferSize);
        renderShape(painterBufferU32, type, radius, radius * 4);
    }

    const int offset = width - radius - border;
    // Fill top texture offset and border with 0x00.
    memset(bufferU8, 0x00, stride * offset);
    for (int i = 0; i < radius + border; i++) {
        // Fill left texture offset with 0x00.
        memset(&bufferU8[(i + offset) * stride], 0x00, offset);
        // Fill texture mask. Since QImage doesn't support floating-point
        // formats, a conversion from U32 to U8 is required (we just convert one
        // of the channel since the fill color is white).
        for (int j = 0; j < radius; j++) {
            bufferU8[(i + offset) * stride + j + offset] = painterBufferU32[i * radius + j] & 0xff;
        }
        // Set right border to 0xff.
        bufferU8[(i + offset) * stride + width - border] = 0xff;
    }
    // Set left texture offset of the last row with 0x00.
    memset(&bufferU8[(height - border) * stride], 0x00, offset);
    // Set bottom border to 0xff.
    memset(&bufferU8[(height - border) * stride + offset], 0xff, radius + border);

    return bufferU8;
}

// FIXME(loicm) We should maybe use a texture atlas storing all the radii to
//     improve batching.
template <int N>
quint32 UCShapeTextureFactory<N>::maskTexture(int index, UCShapeType type, quint16 radius)
{
    DASSERT(index >= 0 && index < N);
    DASSERT((radius & 0xf000) == 0);
    DASSERT(QOpenGLContext::currentContext() == m_context);

    const quint32 currentKey = m_keys[index];
    const quint32 newKey = makeMaskTextureKey(type, radius);
    bool needsUpdate, isNewTexture;
    quint32 textureId = acquireTexture(index, currentKey, newKey, &needsUpdate, &isNewTexture);
    if (!needsUpdate) {
        return textureId;
    }

    // Bind the texture, initialise states and allocate space. The texture size
    // is a multiple of textureStride to allow GPUs to speed up uploads and
    // optimise storage.
    const int textureSize = maskTextureSize(radius);
    QOpenGLFunctions* funcs = m_context->functions();
    funcs->glBindTexture(GL_TEXTURE_2D, textureId);
    if (isNewTexture) {
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, textureSize, textureSize, 0,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    } else if (maskTextureSizeFromKey(currentKey) != textureSize) {
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, textureSize, textureSize, 0,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    }

    // Render and upload texture data.
    quint8* buffer = renderMaskTexture(type, radius);
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureSize, textureSize, GL_LUMINANCE,
                           GL_UNSIGNED_BYTE, buffer);
    free(buffer);

    return textureId;
}

// static
template <int N>
quint16* UCShapeTextureFactory<N>::renderShadowTexture(UCShapeType type, int radius, int shadow)
{
    //  >-<              >-<  Gutters
    //    >-<          >-<    Shadows
    //      >----------<      Radius
    //  ┌─┬─────────────┬─┐
    //  │ │ ┌─────────┐ │ │
    //  │ │ │         │ │ │
    //  │ │ │         │ │ │  <-- Buffer 1
    //  │ │ │         │ │ │
    //  │ │ │         │ │ │
    //  │ │ └─────────┘ │ │
    //  ├─┼─────────────┼─┤

    DASSERT(shadow >= 0);
    DASSERT(radius >= 0);

    // Get sizes, allocate data and get pointers to the buffers.
    const int gutter = shadow;
    const int border = 1;
    const int width = shadow + radius + shadow;
    const int widthGutters = gutter + width + gutter;
    const int widthBorders = border + width + border;
    const int textureWidth = getStride(widthBorders, 1, textureStride);
    const int textureSize = textureWidth * textureWidth * sizeof(quint16);
    const int bufferSize = width * widthGutters * sizeof(float);
    quint8* __restrict data = static_cast<quint8*>(alignedAlloc(32, textureSize + 2 * bufferSize));
    quint16* __restrict texture = reinterpret_cast<quint16*>(data);
    float* __restrict buffer1 = reinterpret_cast<float*>(&data[textureSize]);
    float* __restrict buffer2 = reinterpret_cast<float*>(&data[textureSize + bufferSize]);

#if SHADOW_TEXTURE_DUMP_PERF
    QElapsedTimer timer;
    printf("texture rendering:\n  fill... ");
    timer.start();
#endif

    // Initialise buffer1.
    // ┌─────────────────┐
    // │ 0 0 0 0 0 0 ┌───┤
    // │ 0 0 0 0 0 0 │ 1 │
    // │ 0 0 0 0 0 0 │ 1 │
    // │ 0 0 0 0 0 0 │ 1 │
    // │ 0 0 0 0 0 0 │ 1 │
    // │ 0 ┌─────────┘ 1 │
    // └───┴─────────────┘
    for (int i = 0; i < shadow; i++) {
        for (int j = 0; j < widthGutters; j++) {
            buffer1[i * widthGutters + j] = 0.0f;
        }
    }
    for (int i = shadow; i < shadow + radius; i++) {
        for (int j = 0; j < gutter + shadow + radius; j++) {
            buffer1[i * widthGutters + j] = 0.0f;
        }
        for (int j = gutter + shadow + radius; j < widthGutters; j++) {
            buffer1[i * widthGutters + j] = 1.0f;
        }
    }
    for (int i = shadow + radius; i < width; i++) {
        for (int j = 0; j < gutter + shadow; j++) {
            buffer1[i * widthGutters + j] = 0.0f;
        }
        for (int j = gutter + shadow; j < widthGutters; j++) {
            buffer1[i * widthGutters + j] = 1.0f;
        }
    }

#if SHADOW_TEXTURE_DUMP_PERF
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  shape... ");
    timer.start();
#endif

    // ┌─────────────────┐
    // │   ┌─────────┐   │
    // │   │ 0 0 0 1 │   │
    // │   │ 0 1 1 1 │   │
    // │   │ 0 1 1 1 │   │
    // │   │ 1 1 1 1 │   │
    // │   └─────────┘   │
    // └─────────────────┘
    // Render the shape in buffer1. We first render with QPainter as ARGB32,
    // then we convert to float.
    if (radius > 0) {
        quint32* __restrict u32 = reinterpret_cast<quint32*>(buffer1);
        renderShape(&u32[widthGutters * shadow + gutter + shadow], type, radius, widthGutters * 4);
        for (int i = shadow; i < shadow + radius; i++) {
            for (int j = gutter + shadow; j < gutter + shadow + radius; j++) {
                const int index = i * widthGutters + j;
                buffer1[index] = (u32[index] & 0xff) / 255.0f;
            }
        }
    }

#if SHADOW_TEXTURE_DUMP_PERF
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  hblur... ");
    timer.start();
#endif

    // ┌─┬─────────────┬─┐             ┌─┬─────────────┬─┐
    // │ │ x x x x x x │ │             │ │ x x x x x x │ │
    // │ │ x x A B C D │E│  buffer1    │ │ x x x x x x │ │
    // │ │ x x x x x x │ │             │ │ x x x x x x │ │
    // │ │ x x x x x x │ │    buffer2  │ │ x x x x x x │ │
    // │ │ x x x x x x │ │             │ │ x F x x x x │ │
    // │ │ x x x x x x │ │             │ │ x x x x x x │ │  F = gaussian(A,B,C,D,E)
    // └─┴─────────────┴─┘             └─┴─────────────┴─┘
    // Gaussian blur horizontal pass on buffer1 with transposed writes to
    // buffer2. The transposition allows to compute the vertical pass with
    // values on the same cache line.
    const int gaussianIndex = qMax(0, shadow - 1);  // FIXME(loicm)
    const float sumFactor = 1.0f / gaussianSums[gaussianIndex];
    const float* __restrict gaussianKernel = &gaussianKernels[gaussianOffsets[gaussianIndex]];
    for (int i = 0; i < width; i++) {
        const int index = widthGutters * i + gutter;
        const int offset = gutter + i;
        for (int j = 0; j < width; j++) {
            float sum = 0.0f;
            const float* __restrict source = &buffer1[index + j];
            for (int k = -shadow; k <= shadow; k++) {
                sum += source[k] * gaussianKernel[k];
            }
            buffer2[widthGutters * j + offset] = sum * sumFactor;
        }
    }

#if SHADOW_TEXTURE_DUMP_PERF
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  gutters. ");
    timer.start();
#endif

    // ┌─┬─────────────┬─┐
    // │0│ x x x x x A │A│
    // │0│ x x x x x B │B│
    // │0│ x x x x x C │C│
    // │0│ x x x x x D │D│
    // │0│ x x x x x E │E│
    // │0│ x x x x x F │F│
    // └─┴─────────────┴─┘
    // Fill gutters of buffer2.
    for (int i = 0; i < width; i++) {
        int index = widthGutters * i;
        float* __restrict line = &buffer2[index];
        for (int j = 0; j < gutter; j++) {
            line[j] = 0.0f;
        }
        index += gutter + width;
        line = &buffer2[index];
        const float edge = buffer2[index - 1];
        for (int j = 0; j < gutter; j++) {
            line[j] = edge;
        }
    }

#if SHADOW_TEXTURE_DUMP_PERF
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  vblur... ");
    timer.start();
#endif

    // Gaussian blur vertical pass, shape masking (simply reuse the shape
    // already rendered), floating-point quantization to 8 bits and
    // store. Ensures the returned 16-bit buffer has a 4 bytes stride alignment
    // to fit the default OpenGL unpack alignment.
    const int textureStride = getStride(textureWidth, sizeof(quint16), 4);
    const int textureOffset = textureWidth - widthBorders;
    const int textureOffsetBorder = textureOffset + border;
    memset(texture, 0, textureOffsetBorder * textureStride * sizeof(quint16));
    for (int i = 0; i < width; i++) {
        const int textureIndex = (textureOffsetBorder + i) * textureStride;
        memset(&texture[textureIndex], 0, textureOffsetBorder * sizeof(quint16));
        quint16* __restrict line = &texture[textureIndex + textureOffsetBorder];
        const int bufferIndex = widthGutters * i + gutter;
        for (int j = 0; j < width; j++) {
            float* __restrict source = &buffer2[bufferIndex + j];
            float sum = 0.0f;
            for (int k = -shadow; k <= shadow; k++) {
                sum += source[k] * gaussianKernel[k];
            }
            const float shadow = sum * sumFactor;
            const float shape = buffer1[bufferIndex + j];
            const quint16 shadowU16 = static_cast<quint16>(shadow * 255.0f + 0.5f);
            const quint16 shapeU16 = static_cast<quint16>(shape * 255.0f + 0.5f);
            line[j] = shapeU16 << 8 | shadowU16;
        }
        line[width] = line[width - 1];
    }
    const int lastLineIndex = (textureWidth - border) * textureStride;
    const int penultimateLineIndex = (textureWidth - 2 * border) * textureStride;
    memset(&texture[lastLineIndex], 0, textureOffsetBorder * sizeof(quint16));
    memcpy(&texture[lastLineIndex + textureOffsetBorder],
           &texture[penultimateLineIndex + textureOffsetBorder], width * sizeof(quint16));
    texture[lastLineIndex + textureOffsetBorder + width] = 0xffff;

#if SHADOW_TEXTURE_DUMP_PERF
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
#endif

    // Texture dump routines.
#if SHADOW_TEXTURE_DUMP_CHANNEL0
    for (int i = 0; i < textureStride; i++) {
        for (int j = 0; j < textureWidth; j++) {
            fprintf(stdout, "%02x", texture[i * textureStride + j] & 0xff);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
#endif
#if SHADOW_TEXTURE_DUMP_CHANNEL1
    for (int i = 0; i < textureStride; i++) {
        for (int j = 0; j < textureWidth; j++) {
            fprintf(stdout, "%02x", (texture[i * textureStride + j] >> 8) & 0xff);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
#endif

    return texture;
}

template <int N>
quint32 UCShapeTextureFactory<N>::shadowTexture(
    int index, UCShapeType type, quint16 radius, quint16 shadow)
{
    DASSERT(index >= 0 && index < N);
    DASSERT((radius & 0xf000) == 0);
    DASSERT((shadow & 0xf000) == 0);
    DASSERT(QOpenGLContext::currentContext() == m_context);

    const quint32 currentKey = m_keys[index];
    const quint32 newKey = makeShadowTextureKey(type, radius, shadow);
    bool needsUpdate, isNewTexture;
    quint32 textureId = acquireTexture(index, currentKey, newKey, &needsUpdate, &isNewTexture);
    if (!needsUpdate) {
        return textureId;
    }

    // Bind the texture, initialise states and allocate space. The texture size
    // is a multiple of textureStride to allow GPUs to speed up uploads and
    // optimise storage.
    const int textureWidth = shadowTextureSize(radius, shadow);
    QOpenGLFunctions* funcs = m_context->functions();
    funcs->glBindTexture(GL_TEXTURE_2D, textureId);
    if (isNewTexture) {
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, textureWidth, textureWidth, 0,
                            GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    } else if (shadowTextureSizeFromKey(currentKey) != textureWidth) {
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, textureWidth, textureWidth, 0,
                            GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    }

    // Render and upload texture data.
    quint16* texture = renderShadowTexture(type, radius, shadow);
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureWidth, GL_LUMINANCE_ALPHA,
                           GL_UNSIGNED_BYTE, texture);
    free(texture);

    return textureId;
}
