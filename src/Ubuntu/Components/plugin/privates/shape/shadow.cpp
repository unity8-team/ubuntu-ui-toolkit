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

#include "shadow.h"
#include "utils.h"
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QGuiApplication>

const UCShadow::Style defaultStyle = UCShadow::Outer;
const UCShadow::Shape defaultShape = UCShadow::Squircle;
const QRgb defaultColor = qRgba(0, 0, 0, 255);
const int defaultShadow = 25;
const int maxShadow = 128;
const float maxDistance = 255.0f;

// --- Shaders ---

class ShadowShader : public QSGMaterialShader
{
public:
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_matrixId;
    int m_opacityId;
};

void ShadowShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("texture", 0);
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

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

class OuterShadowShader : public ShadowShader
{
public:
    OuterShadowShader();
    virtual char const* const* attributeNames() const;
};

OuterShadowShader::OuterShadowShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shape/shaders/texture.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shape/shaders/luminance.frag"));
}

char const* const* OuterShadowShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

class InnerShadowShader : public ShadowShader
{
public:
    InnerShadowShader();
    virtual char const* const* attributeNames() const;
};

InnerShadowShader::InnerShadowShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex,
                        QStringLiteral(":/uc/privates/shape/shaders/texture2.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment,
                        QStringLiteral(":/uc/privates/shape/shaders/innershadow.frag"));
}

char const* const* InnerShadowShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "texCoord1Attrib", "texCoord2Attrib", "colorAttrib", 0
    };
    return attributes;
}

// --- Materials ---

UCShadowMaterial::UCShadowMaterial()
{
    setFlag(Blending, true);
}

int UCShadowMaterial::compare(const QSGMaterial* other) const
{
    const UCShadowMaterial* otherShadow = static_cast<const UCShadowMaterial*>(other);
    return otherShadow->textureId() - m_textureId;
}

void UCShadowMaterial::updateTexture(UCShadow::Shape shape, int radius, int shadow)
{
    m_textureId = m_textureFactory.shadowTexture(
        0, static_cast<Texture::Shape>(shape), radius, shadow);
}

QSGMaterialType* UCOuterShadowMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCOuterShadowMaterial::createShader() const
{
    return new OuterShadowShader;
}

QSGMaterialType* UCInnerShadowMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCInnerShadowMaterial::createShader() const
{
    return new InnerShadowShader;
}

// --- Node ---

UCShadowNode::UCShadowNode(UCShadow::Style style, UCShadow::Shape shape)
    : QSGGeometryNode()
    , m_material(Q_NULLPTR)
    , m_geometry(Q_NULLPTR)
    , m_shadow(0)
    , m_newShadow(0)
    , m_radius(0)
    , m_newRadius(0)
    , m_style(style)
    , m_shape(shape)
    , m_newShape(shape)
{
    setFlag(QSGNode::UsePreprocess);

    m_material = style == UCShadow::Outer ?
        static_cast<UCShadowMaterial*>(new UCOuterShadowMaterial) :
        static_cast<UCShadowMaterial*>(new UCInnerShadowMaterial);
    setMaterial(m_material);

    m_geometry = new QSGGeometry(
        attributeSet(style), vertexCount(style), indexCount(style), GL_UNSIGNED_SHORT);
    m_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry->setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry->setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    memcpy(m_geometry->indexData(), indices(style), indexCount(style) * sizeof(quint16));
    setGeometry(m_geometry);

    qsgnode_set_description(this, QLatin1String("shapeshadow"));
}

// static
const quint16* UCShadowNode::indices(UCShadow::Style style)
{
    // The geometry is made of 9 vertices indexed with a triangle strip mode.
    //     0 --- 1 --- 2
    //     |  /  |  /  |
    //     3 --- 4 --- 5
    //     |  /  |  /  |
    //     6 --- 7 --- 8
    static const quint16 outerIndices[] = {
        0, 3, 1, 4, 2, 5,
        5, 3,  // Degenerate triangle.
        3, 6, 4, 7, 5, 8
    };
    // The geometry is made of 36 vertices indexed with a triangle strip mode.
    //     0 --------- 1 8 --------- 9
    //     |  4 --- 5  | |  12 - 13  |
    //     |  |     |  | |  |     |  |
    //     |  6 --- 7  | |  14 - 15  |
    //     2 --------- 3 10 ------- 11
    //     16 ------- 17 24 ------- 25
    //     |  20 - 21  | |  28 - 29  |
    //     |  |     |  | |  |     |  |
    //     |  22 - 23  | |  30 - 31  |
    //     18 ------- 19 26 ------- 27
    static const quint16 innerIndices[] = {
        0, 4, 1, 5, 3, 7, 2, 6, 0, 4,
        4, 8,  // Degenerate triangle.
        8, 12, 9, 13, 11, 15, 10, 14, 8, 12,
        12, 16,  // Degenerate triangle.
        16, 20, 17, 21, 19, 23, 18, 22, 16, 20,
        20, 24,  // Degenerate triangle.
        24, 28, 25, 29, 27, 31, 26, 30, 24, 28
    };

    STATIC_ASSERT(UCShadow::Outer == 0 && UCShadow::Inner == 1);
    const quint16* indices[2] = { outerIndices, innerIndices };
    return indices[static_cast<int>(style)];
}

// static
const QSGGeometry::AttributeSet& UCShadowNode::attributeSet(UCShadow::Style style)
{
    static const QSGGeometry::Attribute outerAttributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::Attribute innerAttributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE)
    };

    STATIC_ASSERT(UCShadow::Outer == 0 && UCShadow::Inner == 1);
    static const QSGGeometry::AttributeSet attributeSet[2] = {
        { 3, sizeof(OuterVertex), outerAttributes },
        { 4, sizeof(InnerVertex), innerAttributes }
    };
    return attributeSet[static_cast<int>(style)];
}

void UCShadowNode::preprocess()
{
    if (m_newShadow != m_shadow || m_newRadius != m_radius || m_newShape != m_shape) {
        m_material->updateTexture(
            static_cast<UCShadow::Shape>(m_newShape), m_newRadius, m_newShadow);
        m_shadow = m_newShadow;
        m_radius = m_newRadius;
        m_shape = m_newShape;
    }
}

void UCShadowNode::setStyle(UCShadow::Style style)
{
    if (style == m_style) {
        return;
    }
    m_style = style;

    // Since textures are reference counted, the current material is deleted
    // after updating the texture of the new one so that we can reuse the
    // previous texture if it is allocated.
    UCShadowMaterial* material = style == UCShadow::Outer ?
        static_cast<UCShadowMaterial*>(new UCOuterShadowMaterial) :
        static_cast<UCShadowMaterial*>(new UCInnerShadowMaterial);
    material->updateTexture(static_cast<UCShadow::Shape>(m_shape), m_radius, m_shadow);
    delete m_material;
    m_material = material;

    delete m_geometry;
    m_geometry = new QSGGeometry(
        attributeSet(style), vertexCount(style), indexCount(style), GL_UNSIGNED_SHORT);
    m_geometry->setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry->setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry->setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    memcpy(m_geometry->indexData(), indices(style), indexCount(style) * sizeof(quint16));

    setMaterial(m_material);
    setGeometry(m_geometry);
}

void UCShadowNode::updateGeometry(
    const QSizeF& itemSize, float shadow, float radius, float angle, float distance, QRgb color)
{
    const float devicePixelRatio = qGuiApp->devicePixelRatio();
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    // Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedShadow = qMin(shadow, maxSize);
    // FIXME(loicm) The diagonal at rounded integers pos prevents rasterising
    //     pixels on a side.
    const float clampedRadius = qMin(radius, maxSize);
    const float textureSize = (2.0f * clampedShadow + clampedRadius) * devicePixelRatio;
    const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
    const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
    const float textureFactor = ((1.0f - textureOffset) * devicePixelRatio) / textureSize;
    const quint32 packedColor = packColor(color);

    // Get the offsets. Adding 180° to cast the shadow according to the virtual
    // light position and using the opposite angle to rotate counter clockwise.
    float s, c;
    sincosf((180.0f - angle) * (M_PI / 180.0f), &s, &c);

    if (m_style == UCShadow::Outer) {
        UCShadowNode::OuterVertex* v =
            reinterpret_cast<UCShadowNode::OuterVertex*>(m_geometry->vertexData());
        const float midW = w * 0.5f;
        const float midH = h * 0.5f;
        const float textureMidWShadow = (midW + clampedShadow) * textureFactor + textureOffset;
        const float textureMidHShadow = (midH + clampedShadow) * textureFactor + textureOffset;
        const float offsetX = c * distance;
        const float offsetY = s * distance;

        // 1st row.
        v[0].x = -clampedShadow + offsetX;
        v[0].y = -clampedShadow + offsetY;
        v[0].s = textureOffset;
        v[0].t = textureOffset;
        v[0].color = packedColor;
        v[1].x = midW + offsetX;
        v[1].y = -clampedShadow + offsetY;
        v[1].s = textureMidWShadow;
        v[1].t = textureOffset;
        v[1].color = packedColor;
        v[2].x = w + clampedShadow + offsetX;
        v[2].y = -clampedShadow + offsetY;
        v[2].s = textureOffset;
        v[2].t = textureOffset;
        v[2].color = packedColor;
        // 2nd row.
        v[3].x = -clampedShadow + offsetX;
        v[3].y = midH + offsetY;
        v[3].s = textureOffset;
        v[3].t = textureMidHShadow;
        v[3].color = packedColor;
        v[4].x = midW + offsetX;
        v[4].y = midH + offsetY;
        v[4].s = textureMidWShadow;
        v[4].t = textureMidHShadow;
        v[4].color = packedColor;
        v[5].x = w + clampedShadow + offsetX;
        v[5].y = midH + offsetY;
        v[5].s = textureOffset;
        v[5].t = textureMidHShadow;
        v[5].color = packedColor;
        // 3rd row.
        v[6].x = -clampedShadow + offsetX;
        v[6].y = h + clampedShadow + offsetY;
        v[6].s = textureOffset;
        v[6].t = textureOffset;
        v[6].color = packedColor;
        v[7].x = midW + offsetX;
        v[7].y = h + clampedShadow + offsetY;
        v[7].s = textureMidWShadow;
        v[7].t = textureOffset;
        v[7].color = packedColor;
        v[8].x = w + clampedShadow + offsetX;
        v[8].y = h + clampedShadow + offsetY;
        v[8].s = textureOffset;
        v[8].t = textureOffset;
        v[8].color = packedColor;

    } else {
        UCShadowNode::InnerVertex* v =
            reinterpret_cast<UCShadowNode::InnerVertex*>(m_geometry->vertexData());

        const float maxOffsetX = qMax(0.0f, w - 2.0f * (clampedRadius + clampedShadow));
        const float maxOffsetY = qMax(0.0f, h - 2.0f * (clampedRadius + clampedShadow));
        const float offsetX = qBound(-maxOffsetX, c * distance, maxOffsetX);
        const float offsetY = qBound(-maxOffsetY, s * distance, maxOffsetY);
        const float midWOffset = (offsetX > 0.0f ? 1.0f : -1.0f) * maxOffsetX - 2.0f * offsetX;
        const float midHOffset = (offsetY > 0.0f ? 1.0f : -1.0f) * maxOffsetY - 2.0f * offsetY;
        const float midW = 0.5f * (w - midWOffset);
        const float midH = 0.5f * (h - midHOffset);

        //const float textureMidWShadow = (midW + clampedShadow) * textureFactor + textureOffset;
        //const float textureMidHShadow = (midH + clampedShadow) * textureFactor + textureOffset;

        const float shadowOffsetX = clampedShadow + offsetX;
        const float shadowOffsetY = clampedShadow + offsetY;
        //const float midWShadowOffsetX = midW + shadowOffsetX;
        //const float midHShadowOffsetY = midH + shadowOffsetY;
        //const float textureMidWShadowOffsetX = midWShadowOffsetX * textureFactor + textureOffset;
        //const float textureMidHShadowOffsetY = midHShadowOffsetY * textureFactor + textureOffset;

        const float shadowMinusOffsetX = clampedShadow - offsetX;
        const float shadowMinusOffsetY = clampedShadow - offsetY;
        //const float midWShadowMinusOffsetX = midW + shadowMinusOffsetX;
        //const float midHShadowMinusOffsetY = midH + shadowMinusOffsetY;
        //const float textureMidWShadowMinusOffsetX = midWShadowMinusOffsetX * textureFactor + textureOffset;
        //const float textureMidHShadowMinusOffsetY = midHShadowMinusOffsetY * textureFactor + textureOffset;

        // Position in the corner where we can put the inner vertex without
        // cropping shape pixels. That allows to avoid using the radius directly
        // and render too much transparent pixels. We consider the shape is a
        // circle (it's harder to get that value for the SVG defined curve of
        // the squircle shape), hence the 1/sqrt(2) (= cos(Pi/4)).
        const float shapeCoverage =
            m_newShape == UCShadow::Squircle ? squircleCoverage : 1.0f - M_SQRT1_2;
        const float cornerLimit =
            qMin(ceilf(clampedRadius * shapeCoverage + clampedShadow), maxSize);

        float x1, x2, y1, y2;

        // Top-left quad.
        v[0].x = 0.0f;
        v[0].y = 0.0f;
        v[0].shadowS = shadowMinusOffsetX * textureFactor + textureOffset;
        v[0].shadowT = shadowMinusOffsetY * textureFactor + textureOffset;
        v[0].shapeS = clampedShadow * textureFactor + textureOffset;
        v[0].shapeT = clampedShadow * textureFactor + textureOffset;
        v[0].color = packedColor;
        v[1].x = midW;
        v[1].y = 0.0f;
        v[1].shadowS = (midW + clampedShadow - offsetX) * textureFactor + textureOffset;
        v[1].shadowT = shadowMinusOffsetY * textureFactor + textureOffset;
        v[1].shapeS = (midW + clampedShadow) * textureFactor + textureOffset;
        v[1].shapeT = clampedShadow * textureFactor + textureOffset;
        v[1].color = packedColor;
        v[2].x = 0.0f;
        v[2].y = midH;
        v[2].shadowS = shadowMinusOffsetX * textureFactor + textureOffset;
        v[2].shadowT = (midH + clampedShadow - offsetY) * textureFactor + textureOffset;
        v[2].shapeS = clampedShadow * textureFactor + textureOffset;
        v[2].shapeT = (midH + clampedShadow) * textureFactor + textureOffset;
        v[2].color = packedColor;
        v[3].x = midW;
        v[3].y = midH;
        v[3].shadowS = (midW + clampedShadow - offsetX) * textureFactor + textureOffset;
        v[3].shadowT = (midH + clampedShadow - offsetY) * textureFactor + textureOffset;
        v[3].shapeS = (midW + clampedShadow) * textureFactor + textureOffset;
        v[3].shapeT = (midH + clampedShadow) * textureFactor + textureOffset;
        v[3].color = packedColor;
        x1 = qBound(cornerLimit, /*shadowOffsetX*/cornerLimit + offsetX, midW);
        y1 = qBound(cornerLimit, /*shadowOffsetY*/cornerLimit + offsetY, midH);
        x2 = qBound(cornerLimit, w - /*clampedShadow*/cornerLimit + offsetX, midW);
        y2 = qBound(cornerLimit, h - /*clampedShadow*/cornerLimit + offsetY, midH);
        v[4].x = x1;
        v[4].y = y1;
        v[4].shadowS = (x1 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[4].shadowT = (y1 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[4].shapeS = (x1 + clampedShadow) * textureFactor + textureOffset;
        v[4].shapeT = (y1 + clampedShadow) * textureFactor + textureOffset;
        v[4].color = packedColor;
        v[5].x = x2;
        v[5].y = y1;
        v[5].shadowS = (x2 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[5].shadowT = (y1 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[5].shapeS = (x2 + clampedShadow) * textureFactor + textureOffset;
        v[5].shapeT = (y1 + clampedShadow) * textureFactor + textureOffset;
        v[5].color = packedColor;
        v[6].x = x1;
        v[6].y = y2;
        v[6].shadowS = (x1 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[6].shadowT = (y2 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[6].shapeS = (x1 + clampedShadow) * textureFactor + textureOffset;
        v[6].shapeT = (y2 + clampedShadow) * textureFactor + textureOffset;
        v[6].color = packedColor;
        v[7].x = x2;
        v[7].y = y2;
        v[7].shadowS = (x2 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[7].shadowT = (y2 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[7].shapeS = (x2 + clampedShadow) * textureFactor + textureOffset;
        v[7].shapeT = (y2 + clampedShadow) * textureFactor + textureOffset;
        v[7].color = packedColor;

        // Top-right quad.
        v[8].x = midW;
        v[8].y = 0.0f;
        v[8].shadowS = (midW + midWOffset + clampedShadow + offsetX) * textureFactor + textureOffset;
        v[8].shadowT = shadowMinusOffsetY * textureFactor + textureOffset;
        v[8].shapeS = (midW + midWOffset + clampedShadow) * textureFactor + textureOffset;
        v[8].shapeT = clampedShadow * textureFactor + textureOffset;
        v[8].color = packedColor;
        v[9].x = w;
        v[9].y = 0.0f;
        v[9].shadowS = shadowOffsetX * textureFactor + textureOffset;
        v[9].shadowT = shadowMinusOffsetY * textureFactor + textureOffset;
        v[9].shapeS = clampedShadow * textureFactor + textureOffset;
        v[9].shapeT = clampedShadow * textureFactor + textureOffset;
        v[9].color = packedColor;
        v[10].x = midW;
        v[10].y = midH;
        v[10].shadowS = (midW + midWOffset + clampedShadow + offsetX) * textureFactor + textureOffset;
        v[10].shadowT = (midH + clampedShadow - offsetY) * textureFactor + textureOffset;
        v[10].shapeS = (midW + midWOffset + clampedShadow) * textureFactor + textureOffset;
        v[10].shapeT = (midH + clampedShadow) * textureFactor + textureOffset;
        v[10].color = packedColor;
        v[11].x = w;
        v[11].y = midH;
        v[11].shadowS = shadowOffsetX * textureFactor + textureOffset;
        v[11].shadowT = (midH + clampedShadow - offsetY) * textureFactor + textureOffset;
        v[11].shapeS = clampedShadow * textureFactor + textureOffset;
        v[11].shapeT = (midH + clampedShadow) * textureFactor + textureOffset;
        v[11].color = packedColor;
        x1 = qBound(midW, /*shadowOffsetX*/cornerLimit + offsetX, w - cornerLimit);
        y1 = qBound(cornerLimit, /*shadowOffsetY*/cornerLimit + offsetY, midH);
        x2 = qBound(midW, w - /*clampedShadow*/cornerLimit + offsetX, w - cornerLimit);
        y2 = qBound(cornerLimit, h - /*clampedShadow*/cornerLimit + offsetY, midH);
        v[12].x = x1;
        v[12].y = y1;
        v[12].shadowS = (w - x1 + shadowOffsetX) * textureFactor + textureOffset;
        v[12].shadowT = (y1 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[12].shapeS = (w - x1 + clampedShadow) * textureFactor + textureOffset;
        v[12].shapeT = (y1 + clampedShadow) * textureFactor + textureOffset;
        v[12].color = packedColor;
        v[13].x = x2;
        v[13].y = y1;
        v[13].shadowS = (w - x2 + shadowOffsetX) * textureFactor + textureOffset;
        v[13].shadowT = (y1 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[13].shapeS = (w - x2 + clampedShadow) * textureFactor + textureOffset;
        v[13].shapeT = (y1 + clampedShadow) * textureFactor + textureOffset;
        v[13].color = packedColor;
        v[14].x = x1;
        v[14].y = y2;
        v[14].shadowS = (w - x1 + shadowOffsetX) * textureFactor + textureOffset;
        v[14].shadowT = (y2 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[14].shapeS = (w - x1 + clampedShadow) * textureFactor + textureOffset;
        v[14].shapeT = (y2 + clampedShadow) * textureFactor + textureOffset;
        v[14].color = packedColor;
        v[15].x = x2;
        v[15].y = y2;
        v[15].shadowS = (w - x2 + shadowOffsetX) * textureFactor + textureOffset;
        v[15].shadowT = (y2 + shadowMinusOffsetY) * textureFactor + textureOffset;
        v[15].shapeS = (w - x2 + clampedShadow) * textureFactor + textureOffset;
        v[15].shapeT = (y2 + clampedShadow) * textureFactor + textureOffset;
        v[15].color = packedColor;

        // Bottom-left quad.
        v[16].x = 0.0f;
        v[16].y = midH;
        v[16].shadowS = shadowMinusOffsetX * textureFactor + textureOffset;
        v[16].shadowT = (midH + midHOffset + clampedShadow + offsetY) * textureFactor + textureOffset;
        v[16].shapeS = clampedShadow * textureFactor + textureOffset;
        v[16].shapeT = (midH + midHOffset + clampedShadow) * textureFactor + textureOffset;
        v[16].color = packedColor;
        v[17].x = midW;
        v[17].y = midH;
        v[17].shadowS = (midW + clampedShadow - offsetX) * textureFactor + textureOffset;
        v[17].shadowT = (midH + midHOffset + clampedShadow + offsetY) * textureFactor + textureOffset;
        v[17].shapeS = (midW + clampedShadow) * textureFactor + textureOffset;
        v[17].shapeT = (midH + midHOffset + clampedShadow) * textureFactor + textureOffset;
        v[17].color = packedColor;
        v[18].x = 0.0f;
        v[18].y = h;
        v[18].shadowS = shadowMinusOffsetX * textureFactor + textureOffset;
        v[18].shadowT = shadowOffsetY * textureFactor + textureOffset;
        v[18].shapeS = clampedShadow * textureFactor + textureOffset;
        v[18].shapeT = clampedShadow * textureFactor + textureOffset;
        v[18].color = packedColor;
        v[19].x = midW;
        v[19].y = h;
        v[19].shadowS = (midW + clampedShadow - offsetX) * textureFactor + textureOffset;
        v[19].shadowT = shadowOffsetY * textureFactor + textureOffset;
        v[19].shapeS = (midW + clampedShadow) * textureFactor + textureOffset;
        v[19].shapeT = clampedShadow * textureFactor + textureOffset;
        v[19].color = packedColor;
        x1 = qBound(cornerLimit, /*shadowOffsetX*/cornerLimit + offsetX, midW);
        y1 = qBound(midH, /*shadowOffsetY*/cornerLimit + offsetY, h - cornerLimit);
        x2 = qBound(cornerLimit, w - /*clampedShadow*/cornerLimit + offsetX, midW);
        y2 = qBound(midH, h - /*clampedShadow*/cornerLimit + offsetY, h - cornerLimit);
        v[20].x = x1;
        v[20].y = y1;
        v[20].shadowS = (x1 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[20].shadowT = (h - y1 + shadowOffsetY) * textureFactor + textureOffset;
        v[20].shapeS = (x1 + clampedShadow) * textureFactor + textureOffset;
        v[20].shapeT = (h - y1 + clampedShadow) * textureFactor + textureOffset;
        v[20].color = packedColor;
        v[21].x = x2;
        v[21].y = y1;
        v[21].shadowS = (x2 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[21].shadowT = (h - y1 + shadowOffsetY) * textureFactor + textureOffset;
        v[21].shapeS = (x2 + clampedShadow) * textureFactor + textureOffset;
        v[21].shapeT = (h - y1 + clampedShadow) * textureFactor + textureOffset;
        v[21].color = packedColor;
        v[22].x = x1;
        v[22].y = y2;
        v[22].shadowS = (x1 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[22].shadowT = (h - y2 + shadowOffsetY) * textureFactor + textureOffset;
        v[22].shapeS = (x1 + clampedShadow) * textureFactor + textureOffset;
        v[22].shapeT = (h - y2 + clampedShadow) * textureFactor + textureOffset;
        v[22].color = packedColor;
        v[23].x = x2;
        v[23].y = y2;
        v[23].shadowS = (x2 + shadowMinusOffsetX) * textureFactor + textureOffset;
        v[23].shadowT = (h - y2 + shadowOffsetY) * textureFactor + textureOffset;
        v[23].shapeS = (x2 + clampedShadow) * textureFactor + textureOffset;
        v[23].shapeT = (h - y2 + clampedShadow) * textureFactor + textureOffset;
        v[23].color = packedColor;

        // Bottom-right quad.
        v[24].x = midW;
        v[24].y = midH;
        v[24].shadowS = (midW + midWOffset + clampedShadow + offsetX) * textureFactor + textureOffset;
        v[24].shadowT = (midH + midHOffset + clampedShadow + offsetY) * textureFactor + textureOffset;
        v[24].shapeS = (midW + midWOffset + clampedShadow) * textureFactor + textureOffset;
        v[24].shapeT = (midH + midHOffset + clampedShadow) * textureFactor + textureOffset;
        v[24].color = packedColor;
        v[25].x = w;
        v[25].y = midH;
        v[25].shadowS = shadowOffsetX * textureFactor + textureOffset;
        v[25].shadowT = (midH + midHOffset + clampedShadow + offsetY) * textureFactor + textureOffset;
        v[25].shapeS = clampedShadow * textureFactor + textureOffset;
        v[25].shapeT = (midH + midHOffset + clampedShadow) * textureFactor + textureOffset;
        v[25].color = packedColor;
        v[26].x = midW;
        v[26].y = h;
        v[26].shadowS = (midW + midWOffset + clampedShadow + offsetX) * textureFactor + textureOffset;
        v[26].shadowT = shadowOffsetY * textureFactor + textureOffset;
        v[26].shapeS = (midW + midWOffset + clampedShadow) * textureFactor + textureOffset;
        v[26].shapeT = clampedShadow * textureFactor + textureOffset;
        v[26].color = packedColor;
        v[27].x = w;
        v[27].y = h;
        v[27].shadowS = shadowOffsetX * textureFactor + textureOffset;
        v[27].shadowT = shadowOffsetY * textureFactor + textureOffset;
        v[27].shapeS = clampedShadow * textureFactor + textureOffset;
        v[27].shapeT = clampedShadow * textureFactor + textureOffset;
        v[27].color = packedColor;
        x1 = qBound(midW, /*shadowOffsetX*/cornerLimit + offsetX, w - cornerLimit);
        y1 = qBound(midH, /*shadowOffsetY*/cornerLimit + offsetY, h - cornerLimit);
        x2 = qBound(midW, w - /*clampedShadow*/cornerLimit + offsetX, w - cornerLimit);
        y2 = qBound(midH, h - /*clampedShadow*/cornerLimit + offsetY, h - cornerLimit);
        v[28].x = x1;
        v[28].y = y1;
        v[28].shadowS = (w - x1 + shadowOffsetX) * textureFactor + textureOffset;
        v[28].shadowT = (h - y1 + shadowOffsetY) * textureFactor + textureOffset;
        v[28].shapeS = (w - x1 + clampedShadow) * textureFactor + textureOffset;
        v[28].shapeT = (h - y1 + clampedShadow) * textureFactor + textureOffset;
        v[28].color = packedColor;
        v[29].x = x2;
        v[29].y = y1;
        v[29].shadowS = (w - x2 + shadowOffsetX) * textureFactor + textureOffset;
        v[29].shadowT = (h - y1 + shadowOffsetY) * textureFactor + textureOffset;
        v[29].shapeS = (w - x2 + clampedShadow) * textureFactor + textureOffset;
        v[29].shapeT = (h - y1 + clampedShadow) * textureFactor + textureOffset;
        v[29].color = packedColor;
        v[30].x = x1;
        v[30].y = y2;
        v[30].shadowS = (w - x1 + shadowOffsetX) * textureFactor + textureOffset;
        v[30].shadowT = (h - y2 + shadowOffsetY) * textureFactor + textureOffset;
        v[30].shapeS = (w - x1 + clampedShadow) * textureFactor + textureOffset;
        v[30].shapeT = (h - y2 + clampedShadow) * textureFactor + textureOffset;
        v[30].color = packedColor;
        v[31].x = x2;
        v[31].y = y2;
        v[31].shadowS = (w - x2 + shadowOffsetX) * textureFactor + textureOffset;
        v[31].shadowT = (h - y2 + shadowOffsetY) * textureFactor + textureOffset;
        v[31].shapeS = (w - x2 + clampedShadow) * textureFactor + textureOffset;
        v[31].shapeT = (h - y2 + clampedShadow) * textureFactor + textureOffset;
        v[31].color = packedColor;
    }

    markDirty(QSGNode::DirtyGeometry);

    // Update data for the preprocess() call.
    const quint8 deviceShadow = static_cast<quint8>(clampedShadow * devicePixelRatio);
    if (m_shadow != deviceShadow) {
        m_newShadow = deviceShadow;
    }
    const quint8 deviceRadius = static_cast<quint8>(clampedRadius * devicePixelRatio);
    if (m_radius != deviceRadius) {
        m_newRadius = deviceRadius;
    }
}

// --- Item ---

UCShadow::UCShadow(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_angle(0)
    , m_distance(0)
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
    STATIC_ASSERT(maxShadow <= 255);  // Quantized to 8 bits.
    const quint8 quantizedSize = static_cast<quint8>(qBound(0, qRound(size), maxShadow));
    if (m_size != quantizedSize) {
        m_size = quantizedSize;
        update();
        Q_EMIT sizeChanged();
    }
}

void UCShadow::setRadius(qreal radius)
{
    STATIC_ASSERT(maxRadius <= 255);  // Quantized to 8 bits.
    const quint8 quantizedRadius = static_cast<quint8>(qBound(0, qRound(radius), maxRadius));
    if (m_radius != quantizedRadius) {
        m_radius = quantizedRadius;
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

void UCShadow::setAngle(qreal angle)
{
    float quantizedAngle = fmodf(static_cast<float>(angle), 360.0f);
    if (quantizedAngle < 0.0f) {
        quantizedAngle += 360.0f;
    }
    quantizedAngle = quantizeToU16(quantizedAngle, 360.0f);
    if (m_angle != quantizedAngle) {
        m_angle = quantizedAngle;
        update();
        Q_EMIT angleChanged();
    }
}

void UCShadow::setDistance(qreal distance)
{
    const quint16 quantizedDistance =
        quantizeToU16(qBound(0.0f, static_cast<float>(distance), maxDistance), maxDistance);
    if (m_distance != quantizedDistance) {
        m_distance = quantizedDistance;
        update();
        Q_EMIT distanceChanged();
    }
}

QSGNode* UCShadow::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty() || m_size <= 0.0f || qAlpha(m_color) == 0) {
        delete oldNode;
        return Q_NULLPTR;
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
        itemSize, static_cast<float>(m_size), static_cast<float>(m_radius),
        unquantizeFromU16(m_angle, 360.0f), unquantizeFromU16(m_distance, 255.0f), m_color);

    return node;
}
