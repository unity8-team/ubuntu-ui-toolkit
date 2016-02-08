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

// FIXME(loicm):
// - Floating-point values for the radius and shadow sizes are rounded for now
//   because the CPU-based shape rendering and gaussian blur don't support
//   sub-pixel rendering.
// - Should use a texture atlas to allow batching of nodes with different shadow
//   and radius sizes.
// - Should try using half-sized texture with bilinear filtering.

#include "shadow.h"
#include "basedata.h"
#include <QtCore/QMutex>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>

// FIXME(loicm) Replace that with Q_ASSERT before shipping.
#define DASSERT(x) do { if (!(x)) { qFatal("assertion failed at %d\n", __LINE__); } } while(0)

// Use QSvgRenderer instead of a custom signed distance field algorithm. Better
// quality but ~50% slower.
#define QT_SVG_RENDERER 1
#if QT_SVG_RENDERER
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtSvg/QSvgRenderer>
#endif

// Log shape rendering and gaussian blur performance timings.
#define PERF_DEBUG 0
#if PERF_DEBUG
#include <QtCore/QElapsedTimer>
#endif

#if defined(Q_CC_GNU)
#define RESTRICT __restrict__
#else
#define RESTRICT
#endif

const UCShadow::Style defaultStyle = UCShadow::Drop;
const UCShadow::Shape defaultShape = UCShadow::Squircle;
const float defaultOpacity = 1.0f;
const QRgb defaultColor = qRgba(0, 0, 0, 255);
const int defaultShadow = 25;
const int defaultRadius = 50;
const int maxShadow = 128;
const int maxRadius = 128;
const int textureStride = 32;
#if !QT_SVG_RENDERER
const float aaDistance = 1.75f;
#endif

// --- Shaders ---

class ShadowShader : public QSGMaterialShader
{
public:
    ShadowShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_channelId;
    int m_matrixId;
    int m_opacityId;
    quint8 m_style : 1;
    quint8 __padding: 7;
};

ShadowShader::ShadowShader()
    : m_style(defaultStyle)
{
    setShaderSourceFile(
        QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/shadow.vert"));
    setShaderSourceFile(
        QOpenGLShader::Fragment, QStringLiteral(":/uc/privates/shaders/shadow.frag"));
}

char const* const* ShadowShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

void ShadowShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue(m_channelId, m_style == UCShadow::Drop ? 0 : 3);
    program()->setUniformValue("texture", 0);
    m_channelId = program()->uniformLocation("channel");
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void ShadowShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    UCShadowMaterial* material = static_cast<UCShadowMaterial*>(newEffect);

    funcs->glBindTexture(GL_TEXTURE_2D, material->textureId());

    const UCShadow::Style style = material->style();
    if (m_style != style) {
        program()->setUniformValue(m_channelId, style == UCShadow::Drop ? 0 : 3);
        m_style = style;
    }
    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Material ---

static QHash<QOpenGLContext*, UCShadowMaterial::TextureHash*> contextHash;
static QMutex contextHashMutex;

UCShadowMaterial::UCShadowMaterial(UCShadow::Style style)
    : m_textureId(0)
    , m_key(0)
    , m_style(style)
{
    setFlag(Blending, true);

    // Get the texture hash for the current context. Note that this hash is
    // stored at creation and is never updated since we assume that QtQuick
    // bounds a graphics context to a material for its entire lifetime.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    contextHashMutex.lock();
    m_textureHash = const_cast<TextureHash*>(contextHash.value(context));

    // Create it in case it's the first shadow material created in that context.
    if (!m_textureHash) {
        m_textureHash = new TextureHash();
        contextHash.insert(context, m_textureHash);
        QObject::connect(context, &QOpenGLContext::aboutToBeDestroyed, [context] {
            // Remove and delete the texture hash associated to a context when
            // the context is about to be destroyed.
            contextHashMutex.lock();
            TextureHash* textureHash = contextHash.take(context);
            contextHashMutex.unlock();
#if !defined(QT_NO_DEBUG)
            // Even though the remaining textures would be deleted when the
            // context is destroyed, we delete all of these in debug builds for
            // the sake of correctness (it also makes OpenGL debugging tools
            // happy). Note that in a common execution, it's unlikely that any
            // shadow materials remain in there since they would have been
            // deleted before that signal is emitted.
            const int count = textureHash->count();
            if (count > 0) {
                int i = 0;
                quint32* textures = new quint32[count];
                QHash<quint32, Texture>::const_iterator it = textureHash->constBegin();
                while (it != textureHash->constEnd()) {
                    textures[i++] = it.value().id();
                    it++;
                }
                context->functions()->glDeleteTextures(count, textures);
                delete[] textures;
            }
#endif
            delete textureHash;
        });
    }

    contextHashMutex.unlock();
}

UCShadowMaterial::~UCShadowMaterial()
{
    // Unref the texture data associated to the current material and clean up if
    // not used anymore.
    TextureHash::iterator it = m_textureHash->find(m_key);
    if (it != m_textureHash->end()) {
        if (it.value().unref() == 0) {
            quint32 textureId = it.value().id();
            QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
            funcs->glDeleteTextures(1, &textureId);
            m_textureHash->erase(it);
        }
    }
}

QSGMaterialType* UCShadowMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCShadowMaterial::createShader() const
{
    return new ShadowShader;
}

int UCShadowMaterial::compare(const QSGMaterial* other) const
{
    const UCShadowMaterial* otherShadow = static_cast<const UCShadowMaterial*>(other);
    if (otherShadow->textureId() == m_textureId) {
        return otherShadow->style() - m_style;
    } else {
        return 1;
    }
}

// Get the stride of a buffer of the given width and bytes per pixel for a
// specific alignment.
static int getStride(int width, int bytesPerPixel, int alignment)
{
    DASSERT(width > 0);
    DASSERT(!(bytesPerPixel & (bytesPerPixel - 1)));  // Power-of-two
    DASSERT(!(alignment & (alignment - 1)));  // Power-of-two

    return ((width * bytesPerPixel + alignment - 1) & ~(alignment - 1)) / bytesPerPixel;
}

static quint16* renderShape(int shadow, int radius, UCShadow::Shape shape)
{
    DASSERT(shadow > 0);
    DASSERT(radius >= 0);

    const int shadowPlusRadius = shadow + radius;
    const int textureWidth = shadowPlusRadius + shadow;
    const int gutter = shadow;
    const int textureWidthPlusGutters = 2 * gutter + textureWidth;
    const int bufferSize = 2 * textureWidth * textureWidthPlusGutters * sizeof(float);
    quint16* RESTRICT dataU16 = (quint16*) malloc(bufferSize);
    float* RESTRICT dataF32 = (float*) dataU16;

    // FIXME(loicm) Try filling unset bytes instead.
    memset(dataU16, 0, bufferSize);

#if PERF_DEBUG
    QElapsedTimer timer;
    printf("texture rendering:\n  shape... ");
    timer.start();
#endif

    if (radius > 0) {
#if QT_SVG_RENDERER
        // Render the shape with QPainter. Since QImage doesn't support
        // floating-point formats, a conversion from U32 to F32 is required once
        // rendered (we just convert one of the channel since the fill color is
        // white).
        QImage image((quint8*)&dataF32[textureWidthPlusGutters * shadow + gutter + shadow], radius,
                     radius, textureWidthPlusGutters * 4, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        if (shape == UCShadow::Squircle) {
            // QSvgRenderer being reentrant, we use a static instance with local
            // data.
            static QSvgRenderer svg(QByteArray(squircleSvg), 0);
            svg.render(&painter);
        } else {
            painter.setBrush(Qt::white);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawEllipse(QRectF(-0.5, -0.5, radius * 2.0 + 0.5, radius * 2.0 + 0.5));
        }
        for (int i = shadow; i < shadowPlusRadius; i++) {
            for (int j = shadow; j < shadowPlusRadius; j++) {
                const int index = i * textureWidthPlusGutters + gutter + j;
                dataF32[index] = (((quint32*)dataF32)[index] & 0xff) / 255.0f;
            }
        }
#else
        // Render a squircled corner using a custom signed distance field
        // renderer. The distance field rendering agorithm with an anti-aliasing
        // distance >1 pixel leads to top/right and bottom/left pixels not
        // perfectly ending at 1. That creates a gap in the shading with the
        // neighbouring pixels (added right after). The current solution is to
        // compensate by adding a linearly interpolated bias to the offset based
        // on the radius.
        // FIXME(loicm) This is not satisfying quality-wise, we should maybe try
        // with a non-linear interpolation.
        const float biasFrom = 0.2f;
        const float biasTo = 0.0775f;
        const float offset = squircleOffset + (((biasTo - biasFrom) / 128.0f) * radius) + biasFrom;
        const float gradient = (squircleSdfWidth - offset) / radius;
        const float normaliseFactor = 1.0f / (gradient * aaDistance);
        float x = 0.5f, y = 0.5f;
        for (int i = shadow; i < shadowPlusRadius; i++, x = 0.5f, y += 1.0f) {
            for (int j = shadow; j < shadowPlusRadius; j++, x += 1.0f) {
                // Bilinear filtering.
                const float u = x * gradient + offset - 0.5f;
                const float v = y * gradient + offset - 0.5f;
                const int x0 = static_cast<int>(u);
                const int y0 = static_cast<int>(v);
                // FIXME(loicm) Avoid clamps by adding a bottom/right border.
                const int x1 = qMin(x0 + 1, squircleSdfWidth - 1);
                const int y1 = qMin(y0 + 1, squircleSdfWidth - 1);
                const float wu0 = u - x0;
                const float wv0 = v - y0;
                const float wu1 = 1.0f - wu0;
                const float wv1 = 1.0f - wv0;
                const float sample =
                    (squircleSdf[x0][y0] * wu1 + squircleSdf[x1][y0] * wu0) * wv1 +
                    (squircleSdf[x0][y1] * wu1 + squircleSdf[x1][y1] * wu0) * wv0;
                // Smooth step (cubic Hermite interpolation) and store.
                const float t = qBound(0.0f, sample * normaliseFactor + 0.5f, 1.0f);
                dataF32[i * textureWidthPlusGutters + j + gutter] = t * t * (3.0f - (2.0f * t));
            }
        }
#endif
    }

#if PERF_DEBUG
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  quads... ");
    timer.start();
#endif

    // Fill bottom-right side of the corner.
    for (int i = shadow; i < shadowPlusRadius; i++) {
        for (int j = shadowPlusRadius; j < textureWidth + gutter; j++) {
            dataF32[i * textureWidthPlusGutters + gutter + j] = 1.0f;
        }
    }
    for (int i = shadowPlusRadius; i < textureWidth; i++) {
        for (int j = shadow; j < textureWidth + gutter; j++) {
            dataF32[i * textureWidthPlusGutters + gutter + j] = 1.0f;
        }
    }

#if PERF_DEBUG
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  hblur... ");
    timer.start();
#endif

    // Gaussian blur horizontal pass.
    const int gaussianIndex = shadow - 1;
    const float sumFactor = 1.0f / gaussianSums[gaussianIndex];
    const float* RESTRICT gaussianKernel = &gaussianKernels[gaussianOffsets[gaussianIndex]];
    float* RESTRICT tempF32 = &dataF32[textureWidthPlusGutters * textureWidth];
    for (int i = 0; i < textureWidth; i++) {
        const int index = textureWidthPlusGutters * i + gutter;
        const int offset = gutter + i;
        for (int j = 0; j < textureWidth; j++) {
            float sum = 0.0f;
            float* RESTRICT src = &dataF32[index + j];
            for (int k = -shadow; k <= shadow; k++) {
                sum += src[k] * gaussianKernel[k];
            }
            tempF32[textureWidthPlusGutters * j + offset] = sum * sumFactor;
        }
    }

#if PERF_DEBUG
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  gutters. ");
    timer.start();
#endif

    // Fill the bottom gutter of the temporary buffer by repeating last row.
    for (int i = 0; i < textureWidth; i++) {
        const float edge = tempF32[textureWidthPlusGutters * i + gutter + textureWidth - 1];
        float* RESTRICT dst = &tempF32[textureWidthPlusGutters * i + gutter + textureWidth];
        for (int j = 0; j < gutter; j++) {
            dst[j] = edge;
        }
    }

#if PERF_DEBUG
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
    printf("  vblur... ");
    timer.start();
#endif

    // Gaussian blur vertical pass, shape masking (simply reuse the shape
    // already rendered), floating-point quantization to 8 bits and
    // store. Ensures the returned 16-bit buffer has a 4 bytes stride alignment
    // to fit the default OpenGL unpack alignment.
    const int stride = getStride(textureWidth, sizeof(quint16), 4);
    for (int i = 0; i < textureWidth; i++) {
        const int index = textureWidthPlusGutters * i + gutter;
        quint16* RESTRICT dst = &dataU16[stride * i];
        for (int j = 0; j < textureWidth; j++) {
            float sum = 0.0f;
            float* RESTRICT src = &tempF32[index + j];
            for (int k = -shadow; k <= shadow; k++) {
                sum += src[k] * gaussianKernel[k];
            }
            const float shadow = sum * sumFactor;
            const float shape = dataF32[i * textureWidthPlusGutters + j + gutter];
            const float inner = shape * (1.0f - shadow);
            const float drop = shadow * (1.0f - shape);
            const quint16 innerU16 = (quint16) (inner * 255.0f + 0.5f);
            const quint16 dropU16 = (quint16) (drop * 255.0f + 0.5f);
            dst[j] = innerU16 << 8 | dropU16;
        }
    }

#if PERF_DEBUG
    printf("%6.2f ms\n", timer.nsecsElapsed() * 0.000001f);
#endif

    return dataU16;
}

void UCShadowMaterial::updateTexture(
    UCShadow::Shape shape, int shadow, int radius,
    UCShadow::Shape newShape, int newShadow, int newRadius)
{
    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    bool isNewTexture = false;

    // Handle the texture cache (a texture is shared by all the shadow materials
    // of the same shadow and radius sizes). No locking is done since we assume
    // the QtQuick renderer calls preprocess() from the same thread for nodes
    // sharing the same graphics context.
    m_key = makeTextureHashKey(newShape, newShadow, newRadius);
    TextureHash::iterator it = m_textureHash->find(makeTextureHashKey(shape, shadow, radius));
    TextureHash::iterator newIt = m_textureHash->find(m_key);
    if (it != m_textureHash->end() && it.value().unref() == 0) {
        quint32 textureId = it.value().id();
        m_textureHash->erase(it);
        if (newIt == m_textureHash->end()) {
            m_textureHash->insert(m_key, Texture(textureId));
            DASSERT(m_textureId == textureId);
        } else {
            funcs->glDeleteTextures(1, &textureId);
            m_textureId = newIt.value().ref();
            return;
        }
    } else {
        if (newIt == m_textureHash->end()) {
            funcs->glGenTextures(1, &m_textureId);
            m_textureHash->insert(m_key, Texture(m_textureId));
            isNewTexture = true;
        } else {
            m_textureId = newIt.value().ref();
            return;
        }
    }

    quint16* buffer = renderShape(newShadow, newRadius, newShape);

    // Upload texture. The texture size is a multiple of textureStride for
    // efficiency reasons to allow GPUs to speed up uploads and optimise
    // storage.
    funcs->glBindTexture(GL_TEXTURE_2D, m_textureId);
    const int newSize = 2 * newShadow + newRadius;
    const int newSizeRounded = getStride(newSize, 1, textureStride);
    if (isNewTexture) {
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, newSizeRounded, newSizeRounded, 0,
                            GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    } else {
        const int size = 2 * shadow + radius;
        const int sizeRounded = getStride(size, 1, textureStride);
        if (sizeRounded != newSizeRounded) {
            funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, newSizeRounded,
                                newSizeRounded, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
        }
    }
    const int offset = newSizeRounded - newSize;
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, offset, offset, newSize, newSize, GL_LUMINANCE_ALPHA,
                           GL_UNSIGNED_BYTE, buffer);

    free(buffer);
}

// --- Node ---

UCShadowNode::UCShadowNode(UCShadow::Style style, UCShadow::Shape shape)
    : QSGGeometryNode()
    , m_material(style)
    , m_geometry(attributeSet(), 20, 34, GL_UNSIGNED_SHORT)
    , m_shadow(0)
    , m_newShadow(0)
    , m_radius(0)
    , m_newRadius(0)
    , m_style(style)
    , m_shape(shape)
    , m_newShape(shape)
{
    setFlag(QSGNode::UsePreprocess);
    memcpy(m_geometry.indexData(), indices(), 34 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shadow"));
}

// static
const quint16* UCShadowNode::indices()
{
    // The geometry is made of 20 vertices indexed with a triangle strip mode.
    //     0 ------ 1 ------ 2
    //     |   3 -- 4 -- 5   |
    //     |  /           \  |
    //     | 6             7 |
    //     8 9            10 11
    //     | 12           13 |
    //     |  \           /  |
    //     |   14 - 15 -16   |
    //    17 ------ 18 ----- 19
    static const quint16 indices[] = {
        9, 8, 6, 0, 3, 1, 4,
        4, 4,  // Degenerate triangle.
        4, 1, 5, 2, 7, 11, 10,
        10, 10,  // Degenerate triangle.
        10, 11, 13, 19, 16, 18, 15,
        15, 15,  // Degenerate triangle.
        15, 18, 14, 17, 12, 8, 9
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCShadowNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        3, sizeof(Vertex), attributes
    };
    return attributeSet;
}

void UCShadowNode::preprocess()
{
    if (m_newShadow != m_shadow || m_newRadius != m_radius || m_newShape != m_shape) {
        m_material.updateTexture(
            static_cast<UCShadow::Shape>(m_shape), m_shadow, m_radius,
            static_cast<UCShadow::Shape>(m_newShape), m_newShadow, m_newRadius);
        m_shadow = m_newShadow;
        m_radius = m_newRadius;
        m_shape = m_newShape;
    }
}

void UCShadowNode::setStyle(UCShadow::Style style)
{
    m_material.setStyle(style);
    m_style = style;
}

// Pack a color in a premultiplied ABGR32 value.
static quint32 packColor(QRgb color)
{
    const quint32 a = qAlpha(color);
    const quint32 b = ((qBlue(color) * a) + 0xff) >> 8;
    const quint32 g = ((qGreen(color) * a) + 0xff) >> 8;
    const quint32 r = ((qRed(color) * a) + 0xff) >> 8;
    return (a << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
}

// FIXME(loicm) Clean up.
void UCShadowNode::updateGeometry(
    const QSizeF& itemSize, float shadow, float radius, QRgb color)
{
    UCShadowNode::Vertex* v = reinterpret_cast<UCShadowNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedShadow = qMin(shadow, maxSize);
    const float clampedRadius = qMin(radius, maxSize);
    const float textureSize = 2.0f * clampedShadow + clampedRadius;
    const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
    const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
    const float textureFactor = (1.0f - textureOffset) / textureSize;
    const quint32 packedColor = packColor(color);

    if (m_style == UCShadow::Drop) {
        // 1st row.
        v[0].x = -clampedShadow;
        v[0].y = -clampedShadow;
        v[0].s = textureOffset;
        v[0].t = textureOffset;
        v[0].color = packedColor;
        v[1].x = w * 0.5f;
        v[1].y = -clampedShadow;
        v[1].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[1].t = textureOffset;
        v[1].color = packedColor;
        v[2].x = w + clampedShadow;
        v[2].y = -clampedShadow;
        v[2].s = textureOffset;
        v[2].t = textureOffset;
        v[2].color = packedColor;

        // 2nd row.
        v[3].x = clampedRadius;
        v[3].y = 0.0f;
        v[3].s = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[3].t = clampedShadow * textureFactor + textureOffset;
        v[3].color = packedColor;
        v[4].x = w * 0.5f;
        v[4].y = 0.0f;
        v[4].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[4].t = clampedShadow * textureFactor + textureOffset;
        v[4].color = packedColor;
        v[5].x = w - clampedRadius;
        v[5].y = 0.0f;
        v[5].s = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[5].t = clampedShadow * textureFactor + textureOffset;
        v[5].color = packedColor;

        // 3rd row.
        v[6].x = 0.0f;
        v[6].y = clampedRadius;
        v[6].s = clampedShadow * textureFactor + textureOffset;
        v[6].t = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[6].color = packedColor;
        v[7].x = w;
        v[7].y = clampedRadius;
        v[7].s = clampedShadow * textureFactor + textureOffset;
        v[7].t = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[7].color = packedColor;

        // 4th row.
        v[8].x = -clampedShadow;
        v[8].y = h * 0.5f;
        v[8].s = textureOffset;
        v[8].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[8].color = packedColor;
        v[9].x = 0.0f;
        v[9].y = h * 0.5f;
        v[9].s = clampedShadow * textureFactor + textureOffset;
        v[9].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[9].color = packedColor;
        v[10].x = w;
        v[10].y = h * 0.5f;
        v[10].s = clampedShadow * textureFactor + textureOffset;
        v[10].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[10].color = packedColor;
        v[11].x = w + clampedShadow;
        v[11].y = h * 0.5f;
        v[11].s = textureOffset;
        v[11].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[11].color = packedColor;

        // 5th row.
        v[12].x = 0.0f;
        v[12].y = h - clampedRadius;
        v[12].s = clampedShadow * textureFactor + textureOffset;
        v[12].t = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[12].color = packedColor;
        v[13].x = w;
        v[13].y = h - clampedRadius;
        v[13].s = clampedShadow * textureFactor + textureOffset;
        v[13].t = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[13].color = packedColor;

        // 6th row.
        v[14].x = clampedRadius;
        v[14].y = h;
        v[14].s = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[14].t = clampedShadow * textureFactor + textureOffset;
        v[14].color = packedColor;
        v[15].x = w * 0.5f;
        v[15].y = h;
        v[15].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[15].t = clampedShadow * textureFactor + textureOffset;
        v[15].color = packedColor;
        v[16].x = w - clampedRadius;
        v[16].y = h;
        v[16].s = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[16].t = clampedShadow * textureFactor + textureOffset;
        v[16].color = packedColor;

        // 7th row.
        v[17].x = -clampedShadow;
        v[17].y = h + clampedShadow;
        v[17].s = textureOffset;
        v[17].t = textureOffset;
        v[17].color = packedColor;
        v[18].x = w * 0.5f;
        v[18].y = h + clampedShadow;
        v[18].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[18].t = textureOffset;
        v[18].color = packedColor;
        v[19].x = w + clampedShadow;
        v[19].y = h + clampedShadow;
        v[19].s = textureOffset;
        v[19].t = textureOffset;
        v[19].color = packedColor;

    } else {
        // 1st row.
        v[0].x = 0.0f;
        v[0].y = 0.0f;
        v[0].s = (clampedShadow) * textureFactor + textureOffset;
        v[0].t = (clampedShadow) * textureFactor + textureOffset;
        v[0].color = packedColor;
        v[1].x = w * 0.5f;
        v[1].y = 0.0f;
        v[1].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[1].t = (clampedShadow) * textureFactor + textureOffset;
        v[1].color = packedColor;
        v[2].x = w;
        v[2].y = 0.0f;
        v[2].s = (clampedShadow) * textureFactor + textureOffset;
        v[2].t = (clampedShadow) * textureFactor + textureOffset;
        v[2].color = packedColor;

        // 2nd row.
        v[3].x = qMax(clampedShadow, clampedRadius);
        v[3].y = clampedShadow;
        v[3].s = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[3].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[3].color = packedColor;
        v[4].x = w * 0.5f;
        v[4].y = clampedShadow;
        v[4].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[4].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[4].color = packedColor;
        v[5].x = w - qMax(clampedShadow, clampedRadius);
        v[5].y = clampedShadow;
        v[5].s = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[5].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[5].color = packedColor;

        // 3rd row.
        v[6].x = clampedShadow;
        v[6].y = qMax(clampedShadow, clampedRadius);
        v[6].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[6].t = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[6].color = packedColor;
        v[7].x = w - clampedShadow;
        v[7].y = qMax(clampedShadow, clampedRadius);
        v[7].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[7].t = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[7].color = packedColor;

        // 4th row.
        v[8].x = 0.0f;
        v[8].y = h * 0.5f;
        v[8].s = (clampedShadow) * textureFactor + textureOffset;
        v[8].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[8].color = packedColor;
        v[9].x = clampedShadow;
        v[9].y = h * 0.5f;
        v[9].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[9].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[9].color = packedColor;
        v[10].x = w - clampedShadow;
        v[10].y = h * 0.5f;
        v[10].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[10].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[10].color = packedColor;
        v[11].x = w;
        v[11].y = h * 0.5f;
        v[11].s = (clampedShadow) * textureFactor + textureOffset;
        v[11].t = (h * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[11].color = packedColor;

        // 5th row.
        v[12].x = clampedShadow;
        v[12].y = h - qMax(clampedShadow, clampedRadius);
        v[12].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[12].t = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[12].color = packedColor;
        v[13].x = w - clampedShadow;
        v[13].y = h - qMax(clampedShadow, clampedRadius);
        v[13].s = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[13].t = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[13].color = packedColor;

        // 6th row.
        v[14].x = qMax(clampedShadow, clampedRadius);
        v[14].y = h - clampedShadow;
        v[14].s = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[14].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[14].color = packedColor;
        v[15].x = w * 0.5f;
        v[15].y = h - clampedShadow;
        v[15].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[15].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[15].color = packedColor;
        v[16].x = w - qMax(clampedShadow, clampedRadius);
        v[16].y = h - clampedShadow;
        v[16].s = (clampedShadow + qMax(clampedShadow, clampedRadius)) * textureFactor + textureOffset;
        v[16].t = (2.0f * clampedShadow) * textureFactor + textureOffset;
        v[16].color = packedColor;

        // 7th row.
        v[17].x = 0.0f;
        v[17].y = h;
        v[17].s = (clampedShadow) * textureFactor + textureOffset;
        v[17].t = (clampedShadow) * textureFactor + textureOffset;
        v[17].color = packedColor;
        v[18].x = w * 0.5f;
        v[18].y = h;
        v[18].s = (w * 0.5f + clampedShadow) * textureFactor + textureOffset;
        v[18].t = (clampedShadow) * textureFactor + textureOffset;
        v[18].color = packedColor;
        v[19].x = w;
        v[19].y = h;
        v[19].s = (clampedShadow) * textureFactor + textureOffset;
        v[19].t = (clampedShadow) * textureFactor + textureOffset;
        v[19].color = packedColor;
    }

    markDirty(QSGNode::DirtyGeometry);

    if (m_shadow != static_cast<quint8>(clampedShadow)) {
        m_newShadow = static_cast<quint8>(clampedShadow);
    }
    if (m_radius != static_cast<quint8>(clampedRadius)) {
        m_newRadius = static_cast<quint8>(clampedRadius);
    }
}

// --- Item ---

UCShadow::UCShadow(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_size(defaultShadow)
    , m_radius(defaultRadius)
    , m_style(defaultStyle)
    , m_shape(defaultShape)
    , m_flags(0)
{
    setFlag(ItemHasContents);
    setOpacity(defaultOpacity);
}

void UCShadow::setStyle(Style style)
{
    const quint8 newStyle = style;
    if (m_style != newStyle) {
        m_style = newStyle;
        m_flags |= DirtyStyle;
        update();
        Q_EMIT styleChanged();
    }
}

void UCShadow::setShape(Shape shape)
{
    const quint8 newShape = shape;
    if (m_shape != newShape) {
        m_shape = newShape;
        m_flags |= DirtyShape;
        update();
        Q_EMIT shapeChanged();
    }
}

void UCShadow::setSize(qreal size)
{
    const quint8 clampedSize = static_cast<quint8>(qBound(0, qRound(size), maxShadow));
    if (m_size != clampedSize) {
        m_size = clampedSize;
        update();
        Q_EMIT sizeChanged();
    }
}

void UCShadow::setRadius(qreal radius)
{
    const quint8 clampedRadius = static_cast<quint8>(qBound(0, qRound(radius), maxRadius));
    if (m_radius != clampedRadius) {
        m_radius = clampedRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

void UCShadow::setColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_color != rgbColor) {
        m_color = rgbColor;
        update();
        Q_EMIT colorChanged();
    }
}

QSGNode* UCShadow::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty() || m_size <= 0.0f) {
        delete oldNode;
        return NULL;
    }

    UCShadowNode* node;

    if (oldNode) {
        node = static_cast<UCShadowNode*>(oldNode);
        if (m_flags & DirtyStyle) {
            node->setStyle(static_cast<Style>(m_style));
        }
        if (m_flags & DirtyShape) {
            node->setShape(static_cast<Shape>(m_shape));
        }
        m_flags = 0;
    } else {
        node = new UCShadowNode(static_cast<Style>(m_style), static_cast<Shape>(m_shape));
    }

    node->updateGeometry(
        itemSize, static_cast<float>(m_size), static_cast<float>(m_radius), m_color);

    return node;
}
