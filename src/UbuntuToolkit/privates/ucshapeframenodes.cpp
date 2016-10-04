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

#include "ucshapeframenodes_p.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFunctions>

UCShapeFrameEdgesNode::UCShapeFrameEdgesNode()
    : QSGGeometryNode()
    , m_opaqueMaterial()
    , m_material()
    , m_geometry(attributeSet(), 16, 22, GL_UNSIGNED_SHORT)
    , m_visible(1)
    , m_blending(0)
{
    qDebug() << "creating UCShapeFrameEdgesNode";
    memcpy(m_geometry.indexData(), indices(), 22 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&m_geometry);
    setOpaqueMaterial(&m_opaqueMaterial);
    setMaterial(&m_material);
    qsgnode_set_description(this, QLatin1String("shapeframeedges"));
}

// static
const quint16* UCShapeFrameEdgesNode::indices()
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
    static const quint16 indices[] = {
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
const QSGGeometry::AttributeSet& UCShapeFrameEdgesNode::attributeSet()
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

void UCShapeFrameEdgesNode::setVisible(bool visible)
{
    qDebug() << "UCShapeFrameEdgesNode::setVisible" << visible;
    if (m_visible != visible) {
        m_visible = visible;
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeFrameEdgesNode::update(
    const QSizeF& itemSize, float radius, float thickness, float space, QRgb color)
{
    Q_UNUSED(space);
    // FIXME(loicm) Add space support.

    UCShapeFrameEdgesNode::Vertex* v =
        reinterpret_cast<UCShapeFrameEdgesNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedThickness = qMin(floorf(thickness), maxSize);
    const float outerRadius = qMin(floorf(radius), maxSize);
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

    // Update the blending state of the opaque material (in QSG terms, an opaque
    // material is the material automatically used when the opacity is 1, but
    // even if the opacity is 1 we have to handle the case where the alpha of
    // the specified color is less than 1).
    const bool blending = qAlpha(color) < 255;
    if (blending != static_cast<bool>(m_blending)) {
        m_opaqueMaterial.setFlag(QSGMaterial::Blending, blending);
        markDirty(QSGNode::DirtyMaterial);
    }
}

class UCShapeFrameCornersShader : public QSGMaterialShader
{
public:
    UCShapeFrameCornersShader();
    char const* const* attributeNames() const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE;

private:
    int m_matrixId;
    int m_opacityId;
};

UCShapeFrameCornersShader::UCShapeFrameCornersShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shaders/texture2.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shaders/frame.frag"));
}

char const* const* UCShapeFrameCornersShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoord1Attrib", "texCoord2Attrib", "colorAttrib", 0
    };
    return attributes;
}

void UCShapeFrameCornersShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    const GLint values[2] = { 0, 1 };
    program()->setUniformValueArray("texture", values, 2);
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void UCShapeFrameCornersShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    UCShapeFrameCornersMaterial* material = static_cast<UCShapeFrameCornersMaterial*>(newEffect);
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

UCShapeFrameCornersMaterial::UCShapeFrameCornersMaterial()
{
    setFlag(Blending, true);
}

QSGMaterialType* UCShapeFrameCornersMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCShapeFrameCornersMaterial::createShader() const
{
    return new UCShapeFrameCornersShader;
}

int UCShapeFrameCornersMaterial::compare(const QSGMaterial* other) const
{
    const UCShapeFrameCornersMaterial* otherFrameCornersMaterial =
        static_cast<const UCShapeFrameCornersMaterial*>(other);
    if (otherFrameCornersMaterial->outerTextureId() != m_textureId[0]) {
        return -1;
    }
    return otherFrameCornersMaterial->innerTextureId() - m_textureId[1];
}

void UCShapeFrameCornersMaterial::updateTexture(int index, UCShapeType type, quint16 radius)
{
    DASSERT(index >= 0 && index < 2);
    DASSERT(radius >= 0);
    m_textureId[index] = m_textureFactory.maskTexture(index, type, radius);
}

UCShapeFrameCornersNode::UCShapeFrameCornersNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), 20, 26, GL_UNSIGNED_SHORT)
    , m_radius{0, 0}
    , m_newRadius{0, 0}
    , m_type(0)
    , m_newType(0)
    , m_visible(1)
{
    qDebug() << "creating UCShapeFrameCornersNode";
    setFlag(QSGNode::UsePreprocess);
    memcpy(m_geometry.indexData(), indices(), 26 * sizeof(quint16));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
    qsgnode_set_description(this, QLatin1String("shapeframecorners"));
}

// static
const quint16* UCShapeFrameCornersNode::indices()
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
    static const quint16 indices[] = {
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
const QSGGeometry::AttributeSet& UCShapeFrameCornersNode::attributeSet()
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

void UCShapeFrameCornersNode::preprocess()
{
    const bool hasNewType = m_newType != m_type;
    for (int i = 0; i < 2; i++) {
        if (hasNewType || m_newRadius[i] != m_radius[i]) {
            m_material.updateTexture(i, static_cast<UCShapeType>(m_newType), m_newRadius[i]);
            m_radius[i] = m_newRadius[i];
        }
    }
    if (hasNewType) {
        m_type = m_newType;
    }
}

void UCShapeFrameCornersNode::setVisible(bool visible)
{
    qDebug() << "UCShapeFrameCornersNode::setVisible" << visible;
    if (m_visible != visible) {
        m_visible = visible;
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeFrameCornersNode::update(
    const QSizeF& itemSize, UCShapeType type, float radius, float thickness, float space,
    QRgb color)
{
    Q_UNUSED(space);
    // FIXME(loicm) Add space support.

    // FIXME(loicm) Temp hack to hide the broken rendering of corners which
    //     slightly appears when the thickness is less than 1.
    if (thickness < 1.0f) {
        radius = 0.0f;
    }

    UCShapeFrameCornersNode::Vertex* v =
        reinterpret_cast<UCShapeFrameCornersNode::Vertex*>(m_geometry.vertexData());
    const float devicePixelRatio = qGuiApp->devicePixelRatio();
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedThickness = qMin(floorf(thickness), maxSize);
    const float deviceThickness = clampedThickness * devicePixelRatio;
    const float border = 1.0f;
    const float outerRadius = qMin(floorf(radius), maxSize);
    const float deviceOuterRadius = outerRadius * devicePixelRatio;
    // FIXME(loicm) Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float outerRadiusRounded =
        getStride(static_cast<int>(deviceOuterRadius + 2 * border), 1, textureStride);
    const float innerRadius = floorf(outerRadius * ((maxSize - clampedThickness) / maxSize));
    const float deviceInnerRadius = innerRadius * devicePixelRatio;
    const float innerRadiusRounded =
        getStride(static_cast<int>(deviceInnerRadius + 2 * border), 1, textureStride);

    const float outerRadiusOffset = outerRadiusRounded - deviceOuterRadius - border;
    const float outerTextureFactor = 1.0f / outerRadiusRounded;
    const float outerS0 = outerTextureFactor * outerRadiusOffset;
    const float outerS1 = outerTextureFactor * (outerRadiusOffset + deviceOuterRadius);
    const float outerS2 = outerTextureFactor * (outerRadiusOffset + deviceThickness);
    const float outerS3 =
        outerTextureFactor * (outerRadiusOffset + deviceThickness + deviceInnerRadius);

    const float innerRadiusOffset = innerRadiusRounded - deviceInnerRadius - border;
    const float innerTextureFactor = 1.0f / innerRadiusRounded;
    const float innerS0 = innerTextureFactor * innerRadiusOffset;
    const float innerS1 = innerTextureFactor * (innerRadiusOffset + deviceInnerRadius);
    const float innerS2 = innerTextureFactor * (innerRadiusOffset - deviceThickness);
    const float innerS3 =
        innerTextureFactor * (innerRadiusOffset - deviceThickness + deviceOuterRadius);

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
    if (m_radius[0] != static_cast<quint8>(deviceOuterRadius)) {
        m_newRadius[0] = static_cast<quint8>(deviceOuterRadius);
    }
    if (m_radius[1] != static_cast<quint8>(deviceInnerRadius)) {
        m_newRadius[1] = static_cast<quint8>(deviceInnerRadius);
    }
    if (m_type != static_cast<quint8>(type)) {
        m_newType = static_cast<quint8>(type);
    }
}
