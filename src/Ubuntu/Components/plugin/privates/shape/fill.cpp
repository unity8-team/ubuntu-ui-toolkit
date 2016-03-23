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

#include "fill.h"
#include <QtGui/QOpenGLFunctions>

const UCFill::Shape defaultShape = UCFill::Squircle;
const QRgb defaultColor = qRgba(255, 255, 255, 255);

// --- Shaders ---

class CornerShader : public QSGMaterialShader
{
public:
    CornerShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_matrixId;
    int m_opacityId;
};

CornerShader::CornerShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shape/shaders/texture.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shape/shaders/luminance.frag"));
}

char const* const* CornerShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

void CornerShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("texture", 0);
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void CornerShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    UCCornerMaterial* material = static_cast<UCCornerMaterial*>(newEffect);

    funcs->glBindTexture(GL_TEXTURE_2D, material->textureId());

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Material ---

UCCornerMaterial::UCCornerMaterial()
{
    setFlag(Blending, true);
}

QSGMaterialType* UCCornerMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCCornerMaterial::createShader() const
{
    return new CornerShader;
}

int UCCornerMaterial::compare(const QSGMaterial* other) const
{
    return static_cast<const UCCornerMaterial*>(other)->textureId() - m_textureId;
}

// --- Node ---

UCCornerNode::UCCornerNode(UCFill::Shape shape, bool visible)
    : QSGGeometryNode()
    , m_geometry(attributeSet(), 12, 12, GL_UNSIGNED_SHORT)
    , m_radius(0)
    , m_newRadius(0)
    , m_shape(shape)
    , m_newShape(shape)
    , m_visible(visible)
{
    setFlag(QSGNode::UsePreprocess);
    memcpy(m_geometry.indexData(), indices(), 12 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLES);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shapecorner"));
}

// static
const quint16* UCCornerNode::indices()
{
    // The geometry is made of 12 vertices indexed with a triangles mode.
    //     0 - 1    2 - 3
    //     | /        \ |
    //     4            5
    //
    //     6            7
    //     | \        / |
    //     8 - 9   10 - 11
    static const quint16 indices[] = { 1, 0, 4, 6, 8, 9, 10, 11, 7, 5, 3, 2 };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCCornerNode::attributeSet()
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

void UCCornerNode::preprocess()
{
    if (m_newRadius != m_radius || m_newShape != m_shape) {
        m_material.updateTexture(static_cast<UCFill::Shape>(m_newShape), m_newRadius);
        m_radius = m_newRadius;
        m_shape = m_newShape;
    }
}

void UCCornerNode::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        markDirty(DirtySubtreeBlocked);
    }
}

void UCCornerNode::updateGeometry(const QSizeF& itemSize, float radius, QRgb color)
{
    UCCornerNode::Vertex* v = reinterpret_cast<UCCornerNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    // FIXME(loicm) The diagonal at rounded integers pos prevents rasterising pixels on a side.
    const float clampedRadius = qMin(radius, maxSize);
    const float border = 1.0f;
    const float textureSize = clampedRadius + 2 * border;
    const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
    const float textureStart = (textureSizeRounded - textureSize + border) / textureSizeRounded;
    const float textureEnd = (textureSizeRounded - border) / textureSizeRounded;
    const quint32 packedColor = packColor(color);

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].s = textureStart;
    v[0].t = textureStart;
    v[0].color = packedColor;
    v[1].x = clampedRadius;
    v[1].y = 0.0f;
    v[1].s = textureEnd;
    v[1].t = textureStart;
    v[1].color = packedColor;
    v[2].x = w - clampedRadius;
    v[2].y = 0.0f;
    v[2].s = textureEnd;
    v[2].t = textureStart;
    v[2].color = packedColor;
    v[3].x = w;
    v[3].y = 0.0f;
    v[3].s = textureStart;
    v[3].t = textureStart;
    v[3].color = packedColor;

    v[4].x = 0.0f;
    v[4].y = clampedRadius;
    v[4].s = textureStart;
    v[4].t = textureEnd;
    v[4].color = packedColor;
    v[5].x = w;
    v[5].y = clampedRadius;
    v[5].s = textureStart;
    v[5].t = textureEnd;
    v[5].color = packedColor;

    v[6].x = 0.0f;
    v[6].y = h - clampedRadius;
    v[6].s = textureStart;
    v[6].t = textureEnd;
    v[6].color = packedColor;
    v[7].x = w;
    v[7].y = h - clampedRadius;
    v[7].s = textureStart;
    v[7].t = textureEnd;
    v[7].color = packedColor;

    v[8].x = 0.0f;
    v[8].y = h;
    v[8].s = textureStart;
    v[8].t = textureStart;
    v[8].color = packedColor;
    v[9].x = clampedRadius;
    v[9].y = h;
    v[9].s = textureEnd;
    v[9].t = textureStart;
    v[9].color = packedColor;
    v[10].x = w - clampedRadius;
    v[10].y = h;
    v[10].s = textureEnd;
    v[10].t = textureStart;
    v[10].color = packedColor;
    v[11].x = w;
    v[11].y = h;
    v[11].s = textureStart;
    v[11].t = textureStart;
    v[11].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);

    // Update data for the preprocess() call.
    if (m_radius != static_cast<quint8>(clampedRadius)) {
        m_newRadius = static_cast<quint8>(clampedRadius);
    }
}

UCColorNode::UCColorNode()
    : QSGGeometryNode()
    , m_geometry(attributeSet(), 8, 8, GL_UNSIGNED_SHORT)
{
    memcpy(m_geometry.indexData(), indices(), 8 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setOpaqueMaterial(&m_opaqueMaterial);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shapecolor"));
}

// static
const quint16* UCColorNode::indices()
{
    // The geometry is made of 8 vertices indexed with a triangle strip mode.
    //       0 - 1
    //      /     \     Ignore ending backslash in comment (-Werror=comment).
    //     2       3
    //     |       |
    //     4       5
    //      \     /
    //       6 - 7
    static const quint16 indices[] = { 2, 4, 0, 6, 1, 7, 3, 5 };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCColorNode::attributeSet()
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

void UCColorNode::updateGeometry(const QSizeF& itemSize, float radius, QRgb color)
{
    UCColorNode::Vertex* v = reinterpret_cast<UCColorNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // Rounded down since Shadow doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedRadius = qMin(radius, maxSize);
    const quint32 packedColor = packColor(color);

    v[0].x = clampedRadius;
    v[0].y = 0.0f;
    v[0].color = packedColor;
    v[1].x = w - clampedRadius;
    v[1].y = 0.0f;
    v[1].color = packedColor;
    v[2].x = 0.0f;
    v[2].y = clampedRadius;
    v[2].color = packedColor;
    v[3].x = w;
    v[3].y = clampedRadius;
    v[3].color = packedColor;
    v[4].x = 0.0f;
    v[4].y = h - clampedRadius;
    v[4].color = packedColor;
    v[5].x = w;
    v[5].y = h - clampedRadius;
    v[5].color = packedColor;
    v[6].x = clampedRadius;
    v[6].y = h;
    v[6].color = packedColor;
    v[7].x = w - clampedRadius;
    v[7].y = h;
    v[7].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);
}

// --- Item ---

UCFill::UCFill(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_radius(defaultRadius)
    , m_shape(defaultShape)
    , m_flags(DirtyShape | DirtyCornerVisibility)
{
    setFlag(ItemHasContents);
}

void UCFill::setShape(Shape shape)
{
    const quint8 newShape = shape;
    if (m_shape != newShape) {
        m_shape = newShape;
        m_flags |= DirtyShape;
        update();
        Q_EMIT shapeChanged();
    }
}

void UCFill::setRadius(qreal radius)
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

void UCFill::setColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_color != rgbColor) {
        m_color = rgbColor;
        update();
        Q_EMIT colorChanged();
    }
}

QSGNode* UCFill::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty()) {
        delete oldNode;
        return NULL;
    }

    UCColorNode* colorNode;
    UCCornerNode* cornerNode;

    if (oldNode) {
        colorNode = static_cast<UCColorNode*>(oldNode->firstChild());
        cornerNode = static_cast<UCCornerNode*>(oldNode->lastChild());
        if (m_flags & DirtyShape) {
            cornerNode->setShape(static_cast<Shape>(m_shape));
        }
        if (m_flags & DirtyCornerVisibility) {
            cornerNode->setVisible(m_radius > 0);
        }
    } else {
        oldNode = new QSGNode;
        colorNode = new UCColorNode;
        cornerNode = new UCCornerNode(static_cast<Shape>(m_shape), m_radius > 0);
        oldNode->appendChildNode(colorNode);
        oldNode->appendChildNode(cornerNode);
    }

    colorNode->updateGeometry(itemSize, static_cast<float>(m_radius), m_color);
    if (m_radius > 0) {
        cornerNode->updateGeometry(itemSize, static_cast<float>(m_radius), m_color);
    }

    m_flags = 0;

    return oldNode;
}
