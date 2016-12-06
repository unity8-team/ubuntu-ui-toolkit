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

// FIXME(loicm) We should use a texture atlas to store all the textures and
//     improve batching of geometry nodes. It would require a bin packing
//     strategy at run-time which is not trivial.

// FIXME(loicm) Add reprocessor define to log ref counting info.

#include "ucshapetexturefactory_p.h"

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

#define SHADOW_TEXTURE_DUMP_CHANNEL0 0  // Shadow
#define SHADOW_TEXTURE_DUMP_CHANNEL1 0  // Mask
#define SHADOW_TEXTURE_DUMP_PERF     0
#if SHADOW_TEXTURE_DUMP_PERF
#include <QtCore/QElapsedTimer>
#endif

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
    DASSERT(radius >= 0);

    // Get sizes.
    const int border = 1;
    const int width = border + radius + border;
    const int textureWidth = roundUp(width, textureRounding);
    const int textureOffset = textureWidth - border - radius;
    const int textureStride = roundUp(textureWidth, 4);  // OpenGL default unpack.
    const int textureHeight = textureWidth;
    const int textureSize = textureStride * textureHeight;
    const int textureSizeRounded = roundUp(textureSize, 16);
    const int bufferWidth = radius;
    const int bufferStride = roundUp(bufferWidth * sizeof(quint32), 16);
    const int bufferStrideU32 = bufferStride / sizeof(quint32);
    const int bufferHeight = bufferWidth;
    const int bufferSize = bufferStride * bufferHeight;
    const int totalSize = textureSizeRounded + bufferSize;

    // Allocate memory and get pointers to the buffers.
    quint8* __restrict texture = static_cast<quint8*>(alignedAlloc(totalSize, cpuCacheLineSize()));
    quint32* __restrict buffer = reinterpret_cast<quint32*>(&texture[textureSizeRounded]);

    // Render the shape with QPainter.
    if (radius > 0) {
        memset(buffer, 0, bufferSize);
        renderShape(buffer, type, radius, bufferStride);
    }

    // Fill texture. Since QImage doesn't support floating-point formats, a
    // conversion of the QPainter buffer from U32 to U8 is required (we just
    // convert one of the channel since the fill color is white).
    memset(texture, 0, textureOffset * textureStride);
    for (int i = 0; i < radius + border; i++) {
        const int dstScanlineIndex = (textureOffset + i) * textureStride;
        memset(&texture[dstScanlineIndex], 0, textureOffset);
        quint8* __restrict dstScanline = &texture[dstScanlineIndex + textureOffset];
        quint32* __restrict srcScanline = &buffer[i * bufferStrideU32];
        for (int j = 0; j < radius; j++) {
            dstScanline[j] = srcScanline[j] & 0xff;
        }
        dstScanline[radius] = 0xff;
    }
    quint8* __restrict borderScanline = &texture[(textureHeight - border) * textureStride];
    memset(borderScanline, 0, textureOffset);
    memset(&borderScanline[textureOffset], 0xff, radius + border);

    return texture;
}

template <int N>
quint32 UCShapeTextureFactory<N>::maskTexture(int index, UCShapeType type, quint16 radius)
{
    DASSERT(index >= 0 && index < N);
    DASSERT((radius & 0xf000) == 0);
    DASSERT(QOpenGLContext::currentContext() == m_context);

    // Texture layout
    // ┌────────────────┐
    // │                │
    // │   ┌────────────┤
    // │   │ ┌────────┐ │
    // │   │ │        │ │
    // │   │ │        │ │
    // │   │ │        │ │
    // │   │ └────────┘ │
    // └───┴────────────┘
    //     >─<        >─< Border (the geometry nodes use border clamping)
    //       >────────<   Radius
    //     >────────────< Width
    // >───<              Texture offset
    // >────────────────< Texture width

    const quint32 currentKey = m_keys[index];
    const quint32 newKey = makeMaskTextureKey(type, radius);
    bool needsUpdate, isNewTexture;
    quint32 textureId = acquireTexture(index, currentKey, newKey, &needsUpdate, &isNewTexture);
    if (!needsUpdate) {
        return textureId;
    }

    // Bind the texture, initialise states and allocate space.
    const int textureWidth = maskTextureSize(radius);
    QOpenGLFunctions* funcs = m_context->functions();
    funcs->glBindTexture(GL_TEXTURE_2D, textureId);
    if (isNewTexture) {
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, textureWidth, textureWidth, 0,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    } else if (maskTextureSizeFromKey(currentKey) != textureWidth) {
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, textureWidth, textureWidth, 0,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    }

    // Render and upload texture data.
    quint8* texture = renderMaskTexture(type, radius);
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureWidth, GL_LUMINANCE,
                           GL_UNSIGNED_BYTE, texture);
    free(texture);

    return textureId;
}

// static
template <int N>
quint16* UCShapeTextureFactory<N>::renderShadowTexture(UCShapeType type, int radius, int shadow)
{
    DASSERT(radius >= 0);
    DASSERT(shadow > 0);

    // A Gaussian blur is applied on the shape to create the shadow. It is
    // rendered using the Extended Box Filtering [1] algorithm that allows a
    // fast and vectorisable implementation on the CPU, along with subpixel
    // support. The generated blur is an approximation that tends to converge
    // towards a Gaussian as we increase the number of box filtering passes. We
    // expose a constant variable (passes) to tweak the rendering quality at
    // build time, it corresponds to the variable d in the paper. With passes=4,
    // as explained by Fabian Giesen [2], the piecewise cubic function we get is
    // already within 3% (absolute) error of the Gaussian. We use passes=3 here
    // since it looks pretty good and is faster.
    //
    // [1] http://www.mia.uni-saarland.de/Publications/gwosdek-ssvm11.pdf
    // [2] https://fgiesen.wordpress.com/2012/08/01/fast-blurs-2

    // FIXME(loicm) Potential optimisation:
    //     - take advantage of the symmetry to compute half of the values
    //     - take advantage of the shadow part being filled with 0 at left/top
    //       and kernel radius being lower that the shadow to start passes with
    //       an offset (same could be applied at the end)
    //     - use 8.8 fixed-point instead of 32-bit floating-point
    //     - SIMD vectorisation

    const int passes = 3;
    Q_STATIC_ASSERT(passes >= 2);  // The implementation relies on that.
    Q_STATIC_ASSERT(passes <= 5);  // Would be useless and overkill.

    // FIXME(loicm) That seems to reduce a blur gradient "jump" at shadow=3.
    // if (shadow <= 3) {
    //     passes = 2;
    // }

    // Buffer sizes.
    const int width = shadow + radius + shadow;
    const int border = 1;
    const int textureWidth = roundUp(border + width + border, textureRounding);
    const int textureOffset = textureWidth - width - border;
    const int textureStride = roundUp(textureWidth * sizeof(quint16), 4);  // OpenGL default unpack.
    const int textureStrideU16 = textureStride / sizeof(quint16);
    const int textureHeight = textureWidth;
    const int textureSize = textureStride * textureHeight;
    const int textureSizeRounded = roundUp(textureSize, cpuCacheLineSize());
    const float filterRadius = (static_cast<float>(shadow) / passes);
    const float filterLength = 2.0f * filterRadius + 1.0f;
    const float filterScale = 1.0f / filterLength;
    const int filterRadiusUp = static_cast<int>(ceilf(filterRadius));
    const int filterLengthUp = 2 * filterRadiusUp + 1;
    const int scratchlineWidth = 1 + filterRadiusUp + width + filterRadiusUp;
    const int scratchlineStride = roundUp(scratchlineWidth * sizeof(float), cpuCacheLineSize());
    const int bufferWidth = width + filterRadiusUp;
    const int bufferStride = roundUp(bufferWidth * sizeof(float), 4);
    const int bufferStrideF32 = bufferStride / sizeof(float);
    const int bufferHeight = width;
    const int buffer2Height = bufferWidth;
    const int bufferSize = bufferStride * bufferHeight;
    const int bufferSizeRounded = roundUp(bufferSize, cpuCacheLineSize());
    const int buffer2Size = bufferStride * buffer2Height;
    const int buffer2SizeRounded = roundUp(buffer2Size, cpuCacheLineSize());
    const int totalSize =
        textureSizeRounded + scratchlineStride + 2 * bufferSizeRounded + buffer2SizeRounded;

    // Allocate memory and get pointers to the buffers.
    quint8* __restrict data = static_cast<quint8*>(alignedAlloc(totalSize, cpuCacheLineSize()));
    int index = 0;
    quint16* __restrict texture = reinterpret_cast<quint16*>(&data[index]);
    index += textureSizeRounded;
    float* __restrict scratchline = &reinterpret_cast<float*>(&data[index])[1 + filterRadiusUp];
    index += scratchlineStride;
    float* __restrict buffer1 = reinterpret_cast<float*>(&data[index]);
    index += bufferSizeRounded;
    float* __restrict buffer2 = reinterpret_cast<float*>(&data[index]);
    index += buffer2SizeRounded;
    float* __restrict buffer3 = reinterpret_cast<float*>(&data[index]);

    // Initialise scratchline, buffer1 and buffer2. Only the first bottom shadow
    // scanline of buffer1 is initialised because it is duplicated at the
    // horizontal convolution step.
    for (int i = -(1 + filterRadiusUp); i < 0; i++) {
        scratchline[i] = 0.0f;  // Always clamp to 0 on the left side.
    }
    for (int i = 0; i < shadow; i++) {
        float* __restrict scanline2 = &buffer2[i * bufferStrideF32];
        for (int j = 0; j < bufferWidth; j++) {
            scanline2[j] = 0.0f;
        }
    }
    for (int i = shadow; i < shadow + radius; i++) {
        float* __restrict scanline1 = &buffer1[i * bufferStrideF32];
        for (int j = 0; j < shadow + radius; j++) {
            scanline1[j] = 0.0f;
        }
        for (int j = shadow + radius; j < bufferWidth; j++) {
            scanline1[j] = 1.0f;
        }
    }{
        float* __restrict scanline1 = &buffer1[(shadow + radius) * bufferStrideF32];
        for (int j = 0; j < shadow; j++) {
            scanline1[j] = 0.0f;
        }
        for (int j = shadow; j < bufferWidth; j++) {
            scanline1[j] = 1.0f;
        }
    }

    // Render the shape in buffer1. We first render with QPainter as ARGB32 and
    // then convert the 8-bit unsigned alpha channel to 32-bit floating-point.
    if (radius > 0) {
        renderShape(&buffer1[shadow * bufferStrideF32 + shadow], type, radius, bufferStride);
        const float normaliseScale = 1.0f / 255.0f;
        for (int i = shadow; i < shadow + radius; i++) {
            float* __restrict scanline = &buffer1[i * bufferStrideF32];
            for (int j = shadow; j < shadow + radius; j++) {
                scanline[j] = (reinterpret_cast<quint32*>(scanline)[j] & 0xff) * normaliseScale;
            }
        }
    }

#if SHADOW_TEXTURE_DUMP_PERF
    QElapsedTimer timer;
    timer.start();
#endif

    // Get fractional part of the kernel radius for lerps.
    float filterFrac = filterRadius - floorf(filterRadius);
    if (filterFrac == 0.0f) {
        filterFrac = 1.0f;
    }

    // Horizontal convolution passes with first pass with reads from buffer1 and
    // writes to buffer2 and subsequent passes iterating on buffer2 in-place. We
    // optimise by not filtering the top shadow (it's filled with 0) and by
    // filtering only the first bottom shadow scanline (they are equivalent) and
    // then copying it to remaining bottom scanlines of buffer2 after the last
    // pass. Note that buffer2 has a higher height so that the first vertical
    // convolution pass can do transposed reads from buffer2 with correct taps
    // on padded scanlines.
    float* __restrict src[passes];
    src[0] = buffer1;
    for (int i = 1; i < passes; i++) {
        src[i] = buffer2;
    }
    for (int i = shadow; i < shadow + radius + 1; i++) {
        for (int pass = 0; pass < passes; pass++) {
            // Init running sum.
            float runningSum = 0.0f;
            for (int j = 0; j < filterRadiusUp; j++) {
                const float value = src[pass][(i * bufferStrideF32) + j];
                scratchline[j] = value;
                runningSum += value;
            }
            // Convolve scanline.
            float* __restrict srcScanline = &src[pass][i * bufferStrideF32];
            float* __restrict dstScanline = &buffer2[i * bufferStrideF32];
            for (int j = 0, k = filterRadiusUp; j < width; j++, k++) {
                const float old1 = scratchline[k - filterLengthUp];
                const float old2 = scratchline[k - filterLengthUp + 1];
                const float new1 = srcScanline[k - 1];
                const float new2 = srcScanline[k];
                runningSum += lerp(filterFrac, new1, new2) - lerp(filterFrac, old2, old1);
                scratchline[k] = new2;
                dstScanline[j] = runningSum * filterScale;
            }
            // We clamp to edge on the right side at next pass, so copy the last
            // scanline value to the padding bytes.
            const float edge = dstScanline[width - 1];
            dstScanline = &dstScanline[width];
            for (int j = 0; j < filterRadiusUp; j++) {
                dstScanline[j] = edge;
            }
        }
    }{
        float* __restrict scanline = &buffer2[(shadow + radius) * bufferStrideF32];
        for (int i = 1; i < buffer2Height - shadow - radius; i++) {
            memcpy(&scanline[i * bufferStrideF32], scanline, width * sizeof(float));
        }
    }

    // First vertical convolution pass with transposed reads from buffer2 and
    // writes to buffer3.
    for (int i = 0; i < bufferHeight; i++) {
        // Init running sum.
        float runningSum = 0.0f;
        for (int j = 0; j < filterRadiusUp; j++) {
            const float value = buffer2[(j * bufferStrideF32) + i];
            scratchline[j] = value;
            runningSum += value;
        }
        // Convolve vertical line.
        float* __restrict dstScanline = &buffer3[i * bufferStrideF32];
        for (int j = 0, k = filterRadiusUp; j < width; j++, k++) {
            const float old1 = scratchline[k - filterLengthUp];
            const float old2 = scratchline[k - filterLengthUp + 1];
            // FIXME(loicm) Is "scratchline[k - 1];" faster (caching)?
            const float new1 = buffer2[((k - 1) * bufferStrideF32) + i];
            const float new2 = buffer2[(k * bufferStrideF32) + i];
            runningSum += lerp(filterFrac, new1, new2) - lerp(filterFrac, old2, old1);
            scratchline[k] = new2;
            dstScanline[j] = runningSum * filterScale;
        }
        // We clamp to edge on the right side at next pass, so copy the last
        // scanline value to the padding bytes.
        const float edge = dstScanline[width - 1];
        dstScanline = &dstScanline[width];
        for (int j = 0; j < filterRadiusUp; j++) {
            dstScanline[j] = edge;
        }
    }

    // Vertical convolution passes iterating on buffer3 in-place.
    for (int pass = 1; pass < passes - 1; pass++) {
        for (int i = 0; i < bufferHeight; i++) {
            // Init running sum.
            float runningSum = 0.0f;
            for (int j = 0; j < filterRadiusUp; j++) {
                const float value = buffer3[(i * bufferStrideF32) + j];
                scratchline[j] = value;
                runningSum += value;
            }
            // Convolve scanline.
            float* __restrict scanline = &buffer3[i * bufferStrideF32];
            for (int j = 0, k = filterRadiusUp; j < width; j++, k++) {
                const float old1 = scratchline[k - filterLengthUp];
                const float old2 = scratchline[k - filterLengthUp + 1];
                const float new1 = scanline[k - 1];
                const float new2 = scanline[k];
                runningSum += lerp(filterFrac, new1, new2) - lerp(filterFrac, old2, old1);
                scratchline[k] = new2;
                scanline[j] = runningSum * filterScale;
            }
            // We clamp to edge on the right side at next pass, so copy the last
            // scanline value to the padding bytes.
            const float edge = scanline[width - 1];
            scanline = &scanline[width];
            for (int j = 0; j < filterRadiusUp; j++) {
                scanline[j] = edge;
            }
        }
    }

    // Last vertical convolution pass with reads on buffer3 and final writes to
    // the texture. The upper/left texture offsets are filled with 0s. The
    // texture borders are filled with 0s on the upper/left and with 1s on the
    // bottom/right so that texture edge clamping works as expected by the
    // geometry nodes.
    memset(texture, 0, textureOffset * textureStride);
    for (int i = 0; i < bufferHeight; i++) {
        const int scanlineIndex = i + textureOffset;
        memset(&texture[scanlineIndex * textureStrideU16], 0, textureOffset * sizeof(quint16));
        // Init running sum.
        float runningSum = 0.0f;
        for (int j = 0; j < filterRadiusUp; j++) {
            const float value = buffer3[(i * bufferStrideF32) + j];
            scratchline[j] = value;
            runningSum += value;
        }
        // Convolve buffer3 scanline and write shadow and shape values to
        // texture with floating-point to 8-bit quantisation.
        float* __restrict srcScanline1 = &buffer1[i * bufferStrideF32];
        float* __restrict srcScanline3 = &buffer3[i * bufferStrideF32];
        quint16* __restrict dstScanline =
            &texture[(scanlineIndex * textureStrideU16) + textureOffset];
        for (int j = 0, k = filterRadiusUp; j < width; j++, k++) {
            const float old1 = scratchline[k - filterLengthUp];
            const float old2 = scratchline[k - filterLengthUp + 1];
            const float new1 = srcScanline3[k - 1];
            const float new2 = srcScanline3[k];
            runningSum += lerp(filterFrac, new1, new2) - lerp(filterFrac, old2, old1);
            scratchline[k] = new2;
            const float shadowF32 = runningSum * filterScale;
            const float shapeF32 = srcScanline1[j];
            const quint16 shadowU16 = static_cast<quint16>(qMin(1.0f, shadowF32) * 255.0f + 0.5f);
            const quint16 shapeU16 = static_cast<quint16>(shapeF32 * 255.0f + 0.5f);
            dstScanline[j] = (shapeU16 << 8) | shadowU16;
        }
        dstScanline[width] = dstScanline[width - 1];
    }
    quint16* __restrict borderScanline = &texture[(textureWidth - border) * textureStrideU16];
    memset(borderScanline, 0, textureOffset * sizeof(quint16));
    memcpy(&borderScanline[textureOffset], &borderScanline[textureOffset - textureStrideU16],
           width * sizeof(quint16));
    borderScanline[textureWidth - 1] = 0xffff;

#if SHADOW_TEXTURE_DUMP_PERF
    LOG("shadow tex gen: %6.2f ms", timer.nsecsElapsed() * 0.000001f);
#endif
#if SHADOW_TEXTURE_DUMP_CHANNEL0
    for (int i = 0; i < textureWidth; i++) {
        for (int j = 0; j < textureWidth; j++) {
            fprintf(stdout, "%02x", texture[i * textureStrideU16 + j] & 0xff);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
#endif
#if SHADOW_TEXTURE_DUMP_CHANNEL1
    for (int i = 0; i < textureWidth; i++) {
        for (int j = 0; j < textureWidth; j++) {
            fprintf(stdout, "%02x", texture[i * textureStrideU16 + j] >> 8);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
#endif

    return texture;
}

// static
template <int N>
quint16* UCShapeTextureFactory<N>::renderShadowTextureNoShadow(UCShapeType type, int radius)
{
    DASSERT(radius >= 0);

    // This is basically a 16-bit texture version of renderMaskTexture() with
    // the mask duplicated on the 2 bytes.

    // Get sizes.
    const int border = 1;
    const int width = border + radius + border;
    const int textureWidth = roundUp(width, textureRounding);
    const int textureOffset = textureWidth - border - radius;
    const int textureStride = roundUp(textureWidth * sizeof(quint16), 4);  // OpenGL default unpack.
    const int textureStrideU16 = textureStride / sizeof(quint16);
    const int textureHeight = textureWidth;
    const int textureSize = textureStride * textureHeight;
    const int textureSizeRounded = roundUp(textureSize, 16);
    const int bufferWidth = radius;
    const int bufferStride = roundUp(bufferWidth * sizeof(quint32), 16);
    const int bufferStrideU32 = bufferStride / sizeof(quint32);
    const int bufferHeight = bufferWidth;
    const int bufferSize = bufferStride * bufferHeight;
    const int totalSize = textureSizeRounded + bufferSize;

    // Allocate memory and get pointers to the buffers.
    quint8* __restrict data = static_cast<quint8*>(alignedAlloc(totalSize, cpuCacheLineSize()));
    quint16* __restrict texture = reinterpret_cast<quint16*>(data);
    quint32* __restrict buffer = reinterpret_cast<quint32*>(&data[textureSizeRounded]);

    // Render the shape with QPainter.
    if (radius > 0) {
        memset(buffer, 0, bufferSize);
        renderShape(buffer, type, radius, bufferStride);
    }

    // Fill texture. Since QImage doesn't support floating-point formats, a
    // conversion of the QPainter buffer from U32 to U8 is required (we just
    // convert one of the channel since the fill color is white).
    memset(texture, 0, textureOffset * textureStride);
    for (int i = 0; i < radius + border; i++) {
        const int dstScanlineIndex = (textureOffset + i) * textureStrideU16;
        memset(&texture[dstScanlineIndex], 0, textureOffset * sizeof(quint16));
        quint16* __restrict dstScanline = &texture[dstScanlineIndex + textureOffset];
        quint32* __restrict srcScanline = &buffer[i * bufferStrideU32];
        for (int j = 0; j < radius; j++) {
            const quint16 mask = srcScanline[j] & 0xff;
            dstScanline[j] = (mask << 8) | mask;
        }
        dstScanline[radius] = 0xffff;
    }
    quint16* __restrict borderScanline = &texture[(textureHeight - border) * textureStrideU16];
    memset(borderScanline, 0, textureOffset * sizeof(quint16));
    memset(&borderScanline[textureOffset], 0xff, (radius + border) * sizeof(quint16));

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

    // Texture layout
    // ┌────────────────┐
    // │                │
    // │   ┌────────────┤
    // │   │ ┌────────┐ │
    // │   │ │ ┌────┐ │ │
    // │   │ │ │    │ │ │
    // │   │ │ └────┘ │ │
    // │   │ └────────┘ │
    // └───┴────────────┘
    //     >─<        >─< Border (the geometry nodes use border clamping)
    //       >─<    >─<   Shadow
    //         >────<     Radius
    // >───<              Texture offset
    // >────────────────< Texture width

    const quint32 currentKey = m_keys[index];
    const quint32 newKey = makeShadowTextureKey(type, radius, shadow);
    bool needsUpdate, isNewTexture;
    quint32 textureId = acquireTexture(index, currentKey, newKey, &needsUpdate, &isNewTexture);
    if (!needsUpdate) {
        return textureId;
    }

    // Bind the texture, initialise states and allocate space.
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
    quint16* texture = shadow > 0 ?
        renderShadowTexture(type, radius, shadow) : renderShadowTextureNoShadow(type, radius);
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureWidth, GL_LUMINANCE_ALPHA,
                           GL_UNSIGNED_BYTE, texture);

    free(texture);
    return textureId;
}
