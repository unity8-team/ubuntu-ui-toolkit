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
#include "utils.h"
#include <QtGui/QOpenGLFunctions>

const UCShadow::Style defaultStyle = UCShadow::Outer;
const UCShadow::Shape defaultShape = UCShadow::Squircle;
const QRgb defaultColor = qRgba(0, 0, 0, 255);
const int defaultShadow = 25;
const int maxShadow = 128;

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
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shape/shaders/texture.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shape/shaders/shadow.frag"));
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
    program()->setUniformValue(m_channelId, m_style == UCShadow::Outer ? 0 : 3);
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
        program()->setUniformValue(m_channelId, style == UCShadow::Outer ? 0 : 3);
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

UCShadowMaterial::UCShadowMaterial(UCShadow::Style style)
    : m_style(style)
{
    setFlag(Blending, true);
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
            static_cast<UCShadow::Shape>(m_newShape), m_newRadius, m_newShadow);
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
    // FIXME(loicm) The diagonal at rounded integers pos prevents rasterising pixels on a side.
    const float clampedRadius = qMin(radius, maxSize);
    const float textureSize = 2.0f * clampedShadow + clampedRadius;
    const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
    const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
    const float textureFactor = (1.0f - textureOffset) / textureSize;
    const quint32 packedColor = packColor(color);

    if (m_style == UCShadow::Outer) {
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

    // Update data for the preprocess() call.
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
