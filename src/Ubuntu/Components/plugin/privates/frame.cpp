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

// FIXME(loicm) Should maybe use one big texture with all the radius
//     packed. That would allow to batch more items together and simplify the
//     code.

#include "frame.h"
#include <QtCore/QMutex>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtSvg/QSvgRenderer>

const UCFrame::Shape defaultShape = UCFrame::Squircle;
const QRgb defaultColor = qRgba(255, 255, 255, 255);
const int defaultThickness = 10;

// --- Shader ---

class FrameCornerShader : public QSGMaterialShader
{
public:
    FrameCornerShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_matrixId;
    int m_opacityId;
};

FrameCornerShader::FrameCornerShader()
{
    setShaderSourceFile(
        QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/frame.vert"));
    setShaderSourceFile(
        QOpenGLShader::Fragment, QStringLiteral(":/uc/privates/shaders/frame.frag"));
}

char const* const* FrameCornerShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "outerCoordAttrib", "innerCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

void FrameCornerShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    const GLint values[2] = { 0, 1 };
    program()->setUniformValueArray("texture", values, 2);
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void FrameCornerShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    UCFrameCornerMaterial* material = static_cast<UCFrameCornerMaterial*>(newEffect);
    funcs->glActiveTexture(GL_TEXTURE1);
    funcs->glBindTexture(GL_TEXTURE_2D, material->innerTextureId());
    funcs->glActiveTexture(GL_TEXTURE0);
    funcs->glBindTexture(GL_TEXTURE_2D, material->outerTextureId());

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Material ---

static QHash<QOpenGLContext*, UCFrameCornerMaterial::TextureHash*> contextHash;
static QMutex contextHashMutex;

UCFrameCornerMaterial::UCFrameCornerMaterial()
{
    setFlag(Blending, true);

    // Get the texture hash for the current context. Note that this hash is
    // stored at creation and is never updated since we assume that QtQuick
    // bounds a graphics context to a material for its entire lifetime.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    contextHashMutex.lock();
    m_textureHash = const_cast<TextureHash*>(contextHash.value(context));

    // Create it in case it's the first corner material created in that context.
    if (!m_textureHash) {
        m_textureHash = new TextureHash();
        contextHash.insert(context, m_textureHash);
        QObject::connect(context, &QOpenGLContext::aboutToBeDestroyed, [context] {
            // Remove and delete the texture hash associated to a context when
            // the context is about to be destroyed.
            DASSERT(QOpenGLContext::currentContext() == context);
            contextHashMutex.lock();
            TextureHash* textureHash = contextHash.take(context);
            contextHashMutex.unlock();
#if !defined(QT_NO_DEBUG)
            // Even though the remaining textures would be deleted when the
            // context is destroyed, we delete all of these in debug builds for
            // the sake of correctness (it also makes OpenGL debugging tools
            // happy). Note that in a common execution, it's unlikely that any
            // corner materials remain in there since they would have been
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

UCFrameCornerMaterial::~UCFrameCornerMaterial()
{
    // Unref the texture data associated to the current material and clean up if
    // not used anymore.
    int textureCount = 0;
    quint32 textureIds[2];
    for (int i = 0; i < 2; i++) {
        TextureHash::iterator it = m_textureHash->find(m_key[i]);
        if (it != m_textureHash->end()) {
            if (it.value().unref() == 0) {
                textureIds[textureCount++] = it.value().id();
                m_textureHash->erase(it);
            }
        }
    }
    if (textureCount > 0) {
        QOpenGLContext::currentContext()->functions()->glDeleteTextures(textureCount, textureIds);
    };
}

QSGMaterialType* UCFrameCornerMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCFrameCornerMaterial::createShader() const
{
    return new FrameCornerShader;
}

int UCFrameCornerMaterial::compare(const QSGMaterial* other) const
{
    const UCFrameCornerMaterial* otherFrameCornerMaterial =
        static_cast<const UCFrameCornerMaterial*>(other);
    if (otherFrameCornerMaterial->outerTextureId() != m_textureId[0]) {
        return -1;
    }
    return otherFrameCornerMaterial->innerTextureId() - m_textureId[1];
}

static quint8* renderShape(int radius, UCFrame::Shape shape)
{
    DASSERT(radius >= 0);

    const int border = 1;  // 1 pixel border around the edges for clamping reasons.
    const int width = getStride(radius + 2 * border, 1, textureStride);
    const int stride = getStride(width, 1, 4);  // OpenGL unpack pixel storage is 4 by default.
    const int height = width;
    const int finalBufferSize = stride * height * sizeof(quint8);
    const int painterBufferSize = radius * radius * sizeof(quint32);
    quint8* RESTRICT bufferU8 = (quint8*) malloc(finalBufferSize + painterBufferSize);
    quint32* RESTRICT painterBufferU32 = (quint32*) &bufferU8[finalBufferSize];

    // Render the shape with QPainter.
    memset(painterBufferU32, 0, painterBufferSize);
    QImage image(reinterpret_cast<quint8*>(painterBufferU32), radius, radius, radius * 4,
                 QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    if (shape == UCFrame::Squircle) {
        // QSvgRenderer being reentrant, we use a static instance with local
        // data.
        // FIXME(loicm) Use the same QSvgRenderer for all the items.
        static QSvgRenderer svg(QByteArray(squircleSvg), 0);
        svg.render(&painter);
    } else {
        painter.setBrush(Qt::white);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawEllipse(QRectF(-0.5, -0.5, radius * 2.0 + 0.5, radius * 2.0 + 0.5));
    }

    // Initialise the top and left borders containing the 1 pixel border and
    // texture stride of the final buffer.
    const int offset = width - radius - border;
    memset(bufferU8, 0x00, stride * offset);
    for (int i = 0; i < radius + border; i++) {
        memset(&bufferU8[(i + offset) * stride], 0x00, offset);
        bufferU8[(i + offset) * stride + width - border] = 0xff;
    }
    memset(&bufferU8[(height - border) * stride + offset], 0xff, radius + border);

    // Since QImage doesn't support floating-point formats, a conversion from
    // U32 to U8 is required once rendered (we just convert one of the channel
    // since the fill color is white).
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < radius; j++) {
            bufferU8[(i + offset) * stride + j + offset] = painterBufferU32[i * radius + j] & 0xff;
        }
    }

    return bufferU8;
}

void UCFrameCornerMaterial::updateTexture(
    int index, UCFrame::Shape shape, int radius, UCFrame::Shape newShape, int newRadius)
{
    DASSERT(newRadius > 0);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    bool isNewTexture = false;

    // Handle the texture cache (a texture is shared by all the shadow materials
    // of the same shadow and radius sizes). No locking is done since we assume
    // the QtQuick renderer calls preprocess() from the same thread for nodes
    // sharing the same graphics context.
    m_key[index] = makeTextureHashKey(newShape, newRadius);
    TextureHash::iterator it = m_textureHash->find(makeTextureHashKey(shape, radius));
    TextureHash::iterator newIt = m_textureHash->find(m_key[index]);
    if (it != m_textureHash->end() && it.value().unref() == 0) {
        quint32 textureId = it.value().id();
        m_textureHash->erase(it);
        if (newIt == m_textureHash->end()) {
            m_textureHash->insert(m_key[index], Texture(textureId));
            DASSERT(m_textureId[index] == textureId);
        } else {
            funcs->glDeleteTextures(1, &textureId);
            m_textureId[index] = newIt.value().ref();
            return;
        }
    } else {
        if (newIt == m_textureHash->end()) {
            funcs->glGenTextures(1, &m_textureId[index]);
            m_textureHash->insert(m_key[index], Texture(m_textureId[index]));
            isNewTexture = true;
        } else {
            m_textureId[index] = newIt.value().ref();
            return;
        }
    }

    quint8* buffer = renderShape(newRadius, newShape);

    // Upload texture. The texture size is a multiple of textureStride to allow
    // GPUs to speed up uploads and optimise storage.
    funcs->glBindTexture(GL_TEXTURE_2D, m_textureId[index]);
    const int border = 1;
    const int newSize = newRadius + 2 * border;
    const int newSizeRounded = getStride(newSize, 1, textureStride);
    if (isNewTexture) {
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, newSizeRounded, newSizeRounded, 0,
                            GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    } else {
        const int size = radius + 2 * border;
        const int sizeRounded = getStride(size, 1, textureStride);
        if (sizeRounded != newSizeRounded) {
            funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, newSizeRounded, newSizeRounded, 0,
                                GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
        }
    }
    funcs->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, newSizeRounded, newSizeRounded, GL_LUMINANCE,
                           GL_UNSIGNED_BYTE, buffer);

    free(buffer);
}

// --- Node ---

UCFrameCornerNode::UCFrameCornerNode(UCFrame::Shape shape, bool visible)
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), 20, 26, GL_UNSIGNED_SHORT)
    , m_radius{0, 0}
    , m_newRadius{0, 0}
    , m_shape(shape)
    , m_newShape(shape)
    , m_visible(visible)
{
    setFlag(QSGNode::UsePreprocess);
    memcpy(m_geometry.indexData(), indices(), 26 * sizeof(unsigned short));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
    qsgnode_set_description(this, QLatin1String("shapeframecorner"));
}

// static
const unsigned short* UCFrameCornerNode::indices()
{
    // The geometry is made of 20 vertices indexed with a triangle strip mode.
    //     0 -1       2- 3
    //     |   4     5   |
    //     6  /       \  7
    //       8         9
    //
    //       10       11
    //    12  \       /  13
    //     |   14   15   |
    //    16 -17     18- 19
    static const unsigned short indices[] = {
        0, 6, 1, 8, 4,
        4, 5,  // Degenerate triangle.
        5, 9, 2, 7, 3,
        3, 16,  // Degenerate triangle.
        16, 12, 17, 10, 14,
        14, 15,  // Degenerate triangle.
        15, 11, 18, 13, 19
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCFrameCornerNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        4, sizeof(Vertex), attributes
    };
    return attributeSet;
}

void UCFrameCornerNode::preprocess()
{
    const bool hasNewshape = m_newShape != m_shape;
    for (int i = 0; i < 2; i++) {
        if (hasNewshape || m_newRadius[i] != m_radius[i]) {
            m_material.updateTexture(i, static_cast<UCFrame::Shape>(m_shape), m_radius[i],
                                     static_cast<UCFrame::Shape>(m_newShape), m_newRadius[i]);
            m_radius[i] = m_newRadius[i];
        }
    }
    if (hasNewshape) {
        m_shape = m_newShape;
    }
}

void UCFrameCornerNode::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        markDirty(DirtySubtreeBlocked);
    }
}

void UCFrameCornerNode::updateGeometry(
    const QSizeF& itemSize, float thickness, float radius, QRgb color)
{
    UCFrameCornerNode::Vertex* v =
        reinterpret_cast<UCFrameCornerNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedThickness = qMin(thickness, maxSize);
    const float border = 1.0f;
    const float outerRadius = qMin(radius, maxSize);
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float innerRadius = floorf(outerRadius * ((maxSize - clampedThickness) / maxSize));
    const float outerRadiusRounded =
        getStride(static_cast<int>(outerRadius + 2 * border), 1, textureStride);
    const float innerRadiusRounded =
        getStride(static_cast<int>(innerRadius + 2 * border), 1, textureStride);

    const float outerRadiusOffset = outerRadiusRounded - outerRadius - border;
    const float outerTextureFactor = 1.0f / outerRadiusRounded;
    const float outerS0 = outerTextureFactor * outerRadiusOffset;
    const float outerS1 = outerTextureFactor * (outerRadiusOffset + outerRadius);
    const float outerS2 = outerTextureFactor * (outerRadiusOffset + clampedThickness);
    const float outerS3 = outerTextureFactor * (outerRadiusOffset + clampedThickness + innerRadius);

    const float innerRadiusOffset =
        innerRadius > 0.0f ? innerRadiusRounded - innerRadius - border : -FLT_MAX;
    const float innerTextureFactor = 1.0f / innerRadiusRounded;
    const float innerS0 = innerTextureFactor * innerRadiusOffset;
    const float innerS1 = innerTextureFactor * (innerRadiusOffset + innerRadius);
    const float innerS2 = innerTextureFactor * (innerRadiusOffset - clampedThickness);
    const float innerS3 = innerTextureFactor * (innerRadiusOffset - clampedThickness + outerRadius);

    const quint32 packedColor = packColor(color);

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].outerS = outerS0;
    v[0].outerT = outerS0;
    v[0].innerS = innerS2;
    v[0].innerT = innerS2;
    v[0].color = packedColor;
    v[1].x = outerRadius;
    v[1].y = 0.0f;
    v[1].outerS = outerS1;
    v[1].outerT = outerS0;
    v[1].innerS = innerS3;
    v[1].innerT = innerS2;
    v[1].color = packedColor;
    v[2].x = w - outerRadius;
    v[2].y = 0.0f;
    v[2].outerS = outerS1;
    v[2].outerT = outerS0;
    v[2].innerS = innerS3;
    v[2].innerT = innerS2;
    v[2].color = packedColor;
    v[3].x = w;
    v[3].y = 0.0f;
    v[3].outerS = outerS0;
    v[3].outerT = outerS0;
    v[3].innerS = innerS2;
    v[3].innerT = innerS2;
    v[3].color = packedColor;

    v[4].x = clampedThickness + innerRadius;
    v[4].y = clampedThickness;
    v[4].outerS = outerS3;
    v[4].outerT = outerS2;
    v[4].innerS = innerS1;
    v[4].innerT = innerS0;
    v[4].color = packedColor;
    v[5].x = w - (clampedThickness + innerRadius);
    v[5].y = clampedThickness;
    v[5].outerS = outerS3;
    v[5].outerT = outerS2;
    v[5].innerS = innerS1;
    v[5].innerT = innerS0;
    v[5].color = packedColor;

    v[6].x = 0.0f;
    v[6].y = outerRadius;
    v[6].outerS = outerS0;
    v[6].outerT = outerS1;
    v[6].innerS = innerS2;
    v[6].innerT = innerS3;
    v[6].color = packedColor;
    v[7].x = w;
    v[7].y = outerRadius;
    v[7].outerS = outerS0;
    v[7].outerT = outerS1;
    v[7].innerS = innerS2;
    v[7].innerT = innerS3;
    v[7].color = packedColor;

    v[8].x = clampedThickness;
    v[8].y = clampedThickness + innerRadius;
    v[8].outerS = outerS2;
    v[8].outerT = outerS3;
    v[8].innerS = innerS0;
    v[8].innerT = innerS1;
    v[8].color = packedColor;
    v[9].x = w - clampedThickness;
    v[9].y = clampedThickness + innerRadius;
    v[9].outerS = outerS2;
    v[9].outerT = outerS3;
    v[9].innerS = innerS0;
    v[9].innerT = innerS1;
    v[9].color = packedColor;

    v[10].x = clampedThickness;
    v[10].y = h - (clampedThickness + innerRadius);
    v[10].outerS = outerS2;
    v[10].outerT = outerS3;
    v[10].innerS = innerS0;
    v[10].innerT = innerS1;
    v[10].color = packedColor;
    v[11].x = w - clampedThickness;
    v[11].y = h - (clampedThickness + innerRadius);
    v[11].outerS = outerS2;
    v[11].outerT = outerS3;
    v[11].innerS = innerS0;
    v[11].innerT = innerS1;
    v[11].color = packedColor;

    v[12].x = 0.0f;
    v[12].y = h - outerRadius;
    v[12].outerS = outerS0;
    v[12].outerT = outerS1;
    v[12].innerS = innerS2;
    v[12].innerT = innerS3;
    v[12].color = packedColor;
    v[13].x = w;
    v[13].y = h - outerRadius;
    v[13].outerS = outerS0;
    v[13].outerT = outerS1;
    v[13].innerS = innerS2;
    v[13].innerT = innerS3;
    v[13].color = packedColor;

    v[14].x = clampedThickness + innerRadius;
    v[14].y = h - clampedThickness;
    v[14].outerS = outerS3;
    v[14].outerT = outerS2;
    v[14].innerS = innerS1;
    v[14].innerT = innerS0;
    v[14].color = packedColor;
    v[15].x = w - (clampedThickness + innerRadius);
    v[15].y = h - clampedThickness;
    v[15].outerS = outerS3;
    v[15].outerT = outerS2;
    v[15].innerS = innerS1;
    v[15].innerT = innerS0;
    v[15].color = packedColor;

    v[16].x = 0.0f;
    v[16].y = h;
    v[16].outerS = outerS0;
    v[16].outerT = outerS0;
    v[16].innerS = innerS2;
    v[16].innerT = innerS2;
    v[16].color = packedColor;
    v[17].x = outerRadius;
    v[17].y = h;
    v[17].outerS = outerS1;
    v[17].outerT = outerS0;
    v[17].innerS = innerS3;
    v[17].innerT = innerS2;
    v[17].color = packedColor;
    v[18].x = w - outerRadius;
    v[18].y = h;
    v[18].outerS = outerS1;
    v[18].outerT = outerS0;
    v[18].innerS = innerS3;
    v[18].innerT = innerS2;
    v[18].color = packedColor;
    v[19].x = w;
    v[19].y = h;
    v[19].outerS = outerS0;
    v[19].outerT = outerS0;
    v[19].innerS = innerS2;
    v[19].innerT = innerS2;
    v[19].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);

    // Update data for the preprocess() call.
    if (m_radius[0] != static_cast<quint8>(outerRadius)) {
        m_newRadius[0] = static_cast<quint8>(outerRadius);
    }
    const quint8 nonNullInnerRadius = qMax(1, static_cast<int>(innerRadius));
    if (m_radius[1] != nonNullInnerRadius) {
        m_newRadius[1] = nonNullInnerRadius;
    }
}

UCFrameNode::UCFrameNode()
    : QSGGeometryNode()
    , m_geometry(attributeSet(), 16, 22, GL_UNSIGNED_SHORT)
{
    memcpy(m_geometry.indexData(), indices(), 22 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setOpaqueMaterial(&m_opaqueMaterial);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shapeframe"));
}

// static
const unsigned short* UCFrameNode::indices()
{
    // The geometry is made of 16 vertices indexed with a triangle strip mode.
    //        0 ----- 1
    //         2 --- 3
    //     4             5
    //     | 6         7 |
    //     | |         | |
    //     | 8         9 |
    //    10             11
    //        12 --- 13
    //       14 ----- 15
    static const unsigned short indices[] = {
        0, 2, 1, 3,
        3, 4,  // Degenerate triangle.
        4, 10, 6, 8,
        8, 7,  // Degenerate triangle.
        7, 9, 5, 11,
        11, 12,  // Degenerate triangle.
        12, 14, 13, 15
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCFrameNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        2, sizeof(Vertex), attributes
    };
    return attributeSet;
}

void UCFrameNode::updateGeometry(const QSizeF& itemSize, float thickness, float radius, QRgb color)
{
    UCFrameNode::Vertex* v =
        reinterpret_cast<UCFrameNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedThickness = qMin(thickness, maxSize);
    const float outerRadius = qMin(radius, maxSize);
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float innerRadius = floorf(outerRadius * ((maxSize - clampedThickness) / maxSize));
    const quint32 packedColor = packColor(color);

    v[0].x = outerRadius;
    v[0].y = 0.0f;
    v[0].color = packedColor;
    v[1].x = w - outerRadius;
    v[1].y = 0.0f;
    v[1].color = packedColor;
    v[2].x = clampedThickness + innerRadius;
    v[2].y = clampedThickness;
    v[2].color = packedColor;
    v[3].x = w - clampedThickness - innerRadius;
    v[3].y = clampedThickness;
    v[3].color = packedColor;
    v[4].x = 0.0f;
    v[4].y = outerRadius;
    v[4].color = packedColor;
    v[5].x = w;
    v[5].y = outerRadius;
    v[5].color = packedColor;
    v[6].x = clampedThickness;
    v[6].y = clampedThickness + innerRadius;
    v[6].color = packedColor;
    v[7].x = w - clampedThickness;
    v[7].y = clampedThickness + innerRadius;
    v[7].color = packedColor;
    v[8].x = clampedThickness;
    v[8].y = h - clampedThickness - innerRadius;
    v[8].color = packedColor;
    v[9].x = w - clampedThickness;
    v[9].y = h - clampedThickness - innerRadius;
    v[9].color = packedColor;
    v[10].x = 0.0f;
    v[10].y = h - outerRadius;
    v[10].color = packedColor;
    v[11].x = w;
    v[11].y = h - outerRadius;
    v[11].color = packedColor;
    v[12].x = clampedThickness + innerRadius;
    v[12].y = h - clampedThickness;
    v[12].color = packedColor;
    v[13].x = w - clampedThickness - innerRadius;
    v[13].y = h - clampedThickness;
    v[13].color = packedColor;
    v[14].x = outerRadius;
    v[14].y = h;
    v[14].color = packedColor;
    v[15].x = w - outerRadius;
    v[15].y = h;
    v[15].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);
}

// --- Item ---

UCFrame::UCFrame(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_thickness(defaultThickness)
    , m_radius(defaultRadius)
    , m_shape(defaultShape)
    , m_flags(DirtyShape | DirtyCornerVisibility)
{
    setFlag(ItemHasContents);
}

void UCFrame::setShape(Shape shape)
{
    const quint8 newShape = shape;
    if (m_shape != newShape) {
        m_shape = newShape;
        m_flags |= DirtyShape;
        update();
        Q_EMIT shapeChanged();
    }
}

void UCFrame::setThickness(qreal thickness)
{
    const quint16 clampedThickness = static_cast<quint8>(qBound(0, qRound(thickness), 0xffff));
    if (m_thickness != clampedThickness) {
        m_thickness = clampedThickness;
        update();
        Q_EMIT thicknessChanged();
    }
}

void UCFrame::setRadius(qreal radius)
{
    const quint8 clampedRadius = static_cast<quint8>(qBound(0, qRound(radius), maxRadius));
    if (m_radius != clampedRadius) {
        if (clampedRadius == 0 || m_radius == 0) {
            m_flags |= DirtyCornerVisibility;
        }
        m_radius = clampedRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

void UCFrame::setColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_color != rgbColor) {
        m_color = rgbColor;
        update();
        Q_EMIT colorChanged();
    }
}

QSGNode* UCFrame::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty() || m_thickness == 0) {
        delete oldNode;
        return NULL;
    }

    UCFrameNode* frameNode;
    UCFrameCornerNode* frameCornerNode;

    if (oldNode) {
        frameNode = static_cast<UCFrameNode*>(oldNode->firstChild());
        frameCornerNode = static_cast<UCFrameCornerNode*>(oldNode->lastChild());
        if (m_flags & DirtyShape) {
            frameCornerNode->setShape(static_cast<Shape>(m_shape));
        }
        if (m_flags & DirtyCornerVisibility) {
            frameCornerNode->setVisible(m_radius > 0);
        }
    } else {
        oldNode = new QSGNode;
        frameNode = new UCFrameNode;
        frameCornerNode = new UCFrameCornerNode(static_cast<Shape>(m_shape), m_radius > 0);
        oldNode->appendChildNode(frameNode);
        oldNode->appendChildNode(frameCornerNode);
    }

    frameNode->updateGeometry(
        itemSize, static_cast<float>(m_thickness), static_cast<float>(m_radius), m_color);
    if (m_radius > 0) {
        frameCornerNode->updateGeometry(
            itemSize, static_cast<float>(m_thickness), static_cast<float>(m_radius), m_color);
    }

    m_flags = 0;

    return oldNode;
}
