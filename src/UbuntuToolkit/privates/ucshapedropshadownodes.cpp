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

// TODO(loicm): Try using half-sized textures to speed up CPU-based shadow
//     rendering, switching to bilinear texture sampling at runtime. Evaluate
//     the trade-off between texture creation (faster generation allows higher
//     max radius and shadow sizes) and rendering speed.

// TODO(loicm): Outer shadow rendering should provide a clippedShape (or
//     knockedOutShape) mode that knocks out the pixels of the shape area. This
//     is often what we need and that would allow to optimise rendering by
//     removing transparent pixels (that are still rasterised, shaded and
//     blended by the GPU...) with a dedicated mesh.

// TODO(loicm): Add support for negative distances.

#include "ucshapedropshadownodes_p.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFunctions>

class UCShapeDropShadowShader : public QSGMaterialShader
{
public:
    UCShapeDropShadowShader();
    char const* const* attributeNames() const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE;

private:
    int m_matrixId;
    int m_opacityId;
};

UCShapeDropShadowShader::UCShapeDropShadowShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shaders/texture.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shaders/colorcoverage.frag"));
}

char const* const* UCShapeDropShadowShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

void UCShapeDropShadowShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("texture", 0);
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void UCShapeDropShadowShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    UCShapeDropShadowMaterial* material = static_cast<UCShapeDropShadowMaterial*>(newEffect);
    funcs->glBindTexture(GL_TEXTURE_2D, material->textureId());

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

UCShapeDropShadowMaterial::UCShapeDropShadowMaterial()
{
    setFlag(Blending, true);
}

QSGMaterialType* UCShapeDropShadowMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCShapeDropShadowMaterial::createShader() const
{
    return new UCShapeDropShadowShader;
}

int UCShapeDropShadowMaterial::compare(const QSGMaterial* other) const
{
    const UCShapeDropShadowMaterial* otherShadow =
        static_cast<const UCShapeDropShadowMaterial*>(other);
    return otherShadow->textureId() - m_textureId;
}

void UCShapeDropShadowMaterial::updateTexture(UCShapeType type, quint16 radius, quint16 shadow)
{
    m_textureId = m_textureFactory.shadowTexture(0, type, radius, shadow);
}

UCShapeDropShadowNode::UCShapeDropShadowNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), 9, 14, GL_UNSIGNED_SHORT)
    , m_size(0)
    , m_newSize(0)
    , m_radius(0)
    , m_newRadius(0)
    , m_type(0)
    , m_newType(0)
    , m_visible(0)
{
    DLOG("creating UCShapeDropShadowNode");
    setFlag(QSGNode::UsePreprocess);
    memcpy(m_geometry.indexData(), indices(), 14 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shapedropshadow"));
}

// static
const quint16* UCShapeDropShadowNode::indices()
{
    // The geometry is made of 9 vertices indexed with a triangle strip mode.
    //     0 --- 1 --- 2
    //     |  /  |  /  |
    //     3 --- 4 --- 5
    //     |  /  |  /  |
    //     6 --- 7 --- 8
    static const quint16 indices[] = {
        0, 3, 1, 4, 2, 5,
        5, 3,  // Degenerate triangle.
        3, 6, 4, 7, 5, 8
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCShapeDropShadowNode::attributeSet()
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

void UCShapeDropShadowNode::preprocess()
{
    if (m_newSize != m_size || m_newRadius != m_radius || m_newType != m_type) {
        m_material.updateTexture(static_cast<UCShapeType>(m_newType), m_newRadius, m_newSize);
        m_size = m_newSize;
        m_radius = m_newRadius;
        m_type = m_newType;
    }
}

void UCShapeDropShadowNode::setVisible(bool visible)
{
    DLOG("UCShapeDropShadowNode::setVisible %d", visible);
    if (m_visible != visible) {
        m_visible = visible;
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeDropShadowNode::update(
    const QSizeF& itemSize, UCShapeType type, float radius, float size, float angle, float distance,
    QRgb color)
{
    const float devicePixelRatio = qGuiApp->devicePixelRatio();
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedSize = qMin(floorf(size), maxSize);
    // FIXME(loicm) The diagonal at rounded integers pos prevents rasterising
    //     pixels on a side.
    const float clampedRadius = qMin(floorf(radius), maxSize);
    const float textureSize = (2.0f * clampedSize + clampedRadius) * devicePixelRatio;
    const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
    const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
    const float textureFactor = ((1.0f - textureOffset) * devicePixelRatio) / textureSize;
    const quint32 packedColor = packColor(color);

    // Get the offsets. Adding 180° to cast the shadow according to the virtual
    // light position and using the opposite angle to rotate counter clockwise.
    float s, c;
    sincosf((180.0f - angle) * (M_PI / 180.0f), &s, &c);

    UCShapeDropShadowNode::Vertex* v =
        reinterpret_cast<UCShapeDropShadowNode::Vertex*>(m_geometry.vertexData());
    const float midW = w * 0.5f;
    const float midH = h * 0.5f;
    const float textureMidWShadow = (midW + clampedSize) * textureFactor + textureOffset;
    const float textureMidHShadow = (midH + clampedSize) * textureFactor + textureOffset;
    const float offsetX = c * distance;
    const float offsetY = s * distance;

    // 1st row.
    v[0].x = -clampedSize + offsetX;
    v[0].y = -clampedSize + offsetY;
    v[0].s = textureOffset;
    v[0].t = textureOffset;
    v[0].color = packedColor;
    v[1].x = midW + offsetX;
    v[1].y = -clampedSize + offsetY;
    v[1].s = textureMidWShadow;
    v[1].t = textureOffset;
    v[1].color = packedColor;
    v[2].x = w + clampedSize + offsetX;
    v[2].y = -clampedSize + offsetY;
    v[2].s = textureOffset;
    v[2].t = textureOffset;
    v[2].color = packedColor;
    // 2nd row.
    v[3].x = -clampedSize + offsetX;
    v[3].y = midH + offsetY;
    v[3].s = textureOffset;
    v[3].t = textureMidHShadow;
    v[3].color = packedColor;
    v[4].x = midW + offsetX;
    v[4].y = midH + offsetY;
    v[4].s = textureMidWShadow;
    v[4].t = textureMidHShadow;
    v[4].color = packedColor;
    v[5].x = w + clampedSize + offsetX;
    v[5].y = midH + offsetY;
    v[5].s = textureOffset;
    v[5].t = textureMidHShadow;
    v[5].color = packedColor;
    // 3rd row.
    v[6].x = -clampedSize + offsetX;
    v[6].y = h + clampedSize + offsetY;
    v[6].s = textureOffset;
    v[6].t = textureOffset;
    v[6].color = packedColor;
    v[7].x = midW + offsetX;
    v[7].y = h + clampedSize + offsetY;
    v[7].s = textureMidWShadow;
    v[7].t = textureOffset;
    v[7].color = packedColor;
    v[8].x = w + clampedSize + offsetX;
    v[8].y = h + clampedSize + offsetY;
    v[8].s = textureOffset;
    v[8].t = textureOffset;
    v[8].color = packedColor;
    markDirty(QSGNode::DirtyGeometry);

    // Update data for the preprocess() call.
    const quint8 deviceSize = static_cast<quint16>(clampedSize * devicePixelRatio);
    if (m_size != deviceSize) {
        m_newSize = deviceSize;
    }
    const quint8 deviceRadius = static_cast<quint16>(clampedRadius * devicePixelRatio);
    if (m_radius != deviceRadius) {
        m_newRadius = deviceRadius;
    }
    if (m_type != static_cast<quint8>(type)) {
        m_newType = static_cast<quint8>(type);
    }
}
