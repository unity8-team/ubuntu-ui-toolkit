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

#ifndef UCSHAPEFILLNODES_P_H
#define UCSHAPEFILLNODES_P_H

#include <QtQuick/QSGNode>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>
#include <UbuntuToolkit/private/ucshaperesources_p.h>

// ################################
// # Fill center shadow materials #
// ################################

class UCShapeFillCenterShadowOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCenterShadowOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/uc/privates/shaders/fillcentershadow.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/fillcentershadow_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "shadowCoordAttrib", "midShadowCoordAttrib", "colorAttrib",
            "shadowColorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("shadowTexture", 0);
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial*) Q_DECL_OVERRIDE {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        UCShapeTextureProvider* provider = dynamic_cast<UCShapeTextureProvider*>(newEffect);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId());
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeFillCenterShadowShader : public UCShapeFillCenterShadowOpaqueShader
{
public:
    UCShapeFillCenterShadowShader() : UCShapeFillCenterShadowOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/fillcentershadow.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCenterShadowOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCenterShadowOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCenterShadowMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCenterShadowMaterial() : m_textureId(0) {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new UCShapeFillCenterShadowOpaqueShader : new UCShapeFillCenterShadowShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        return reinterpret_cast<const UCShapeTextureProvider*>(other)->textureId() - m_textureId;
    }

    quint32 textureId(int) const Q_DECL_OVERRIDE {
        return m_textureId;
    }
    void updateTexture(UCShapeType type, quint16 radius, quint16 shadow) {
        m_textureId = m_textureFactory.shadowTexture(0, type, radius, shadow);
    }

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeFillCenterShadowResources : public UCShapeResources
{
public:
    UCShapeFillCenterShadowResources(
        int vertexCount, int indexCount, int indexType = GL_UNSIGNED_SHORT)
        : m_geometry(attributeSet(), vertexCount, indexCount, indexType) {
        m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
        m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
        m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    }

    QSGMaterial* material() Q_DECL_OVERRIDE { return &m_material; }
    QSGMaterial* opaqueMaterial() Q_DECL_OVERRIDE { return &m_opaqueMaterial; }
    QSGGeometry* geometry() Q_DECL_OVERRIDE { return &m_geometry; }

    struct Vertex {
        float x, y;
        float shadowS, shadowT;
        float midShadowS, midShadowT;
        quint32 color;
        quint32 shadowColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // shadowS, shadowT
            QSGGeometry::Attribute::create(2, 2, GL_FLOAT),          // midShadowS, midShadowT
            QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(4, 4, GL_UNSIGNED_BYTE)   // shadowColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            5, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCenterShadowMaterial<false> m_material;
    UCShapeFillCenterShadowMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

// ####################
// # Fill center node #
// ####################

class UCShapeFillCenterNode : public QSGGeometryNode
{
public:
    UCShapeFillCenterNode();
    ~UCShapeFillCenterNode();

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return !(m_flags & Visible); }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, QRgb color, float shadowSize,
        float shadowAngle, float shadowDistance, QRgb shadowColor);

private:
    enum {
        HasColor    = (1 << 0),
        HasShadow   = (1 << 1),
        HasBorder   = (1 << 2),
        StyleMask   = (HasColor | HasShadow | HasBorder),
        Textured    = (HasShadow | HasBorder),
        DirtyRadius = (1 << 3),
        DirtyShadow = (1 << 4),
        DirtyShape  = (1 << 5),
        DirtyMask   = (DirtyRadius | DirtyShadow | DirtyShape),
        Visible     = (1 << 6),
        Blending    = (1 << 7)
    };

    UCShapeResources* m_resources;
    quint16 m_radius;
    quint16 m_shadow;
    quint8 m_shape;
    quint8 m_flags;
};

// #####################
// # Fill corners node #
// #####################

class UCShapeFillCornersNode : public QSGGeometryNode
{
public:
    UCShapeFillCornersNode();
    ~UCShapeFillCornersNode();

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return !(m_flags & Visible); }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, QRgb color, float shadowSize,
        float shadowAngle, float shadowDistance, QRgb shadowColor);

private:
    enum {
        HasColor    = (1 << 0),
        HasShadow   = (1 << 1),
        HasBorder   = (1 << 2),
        StyleMask   = (HasColor | HasShadow | HasBorder),
        DirtyRadius = (1 << 3),
        DirtyShadow = (1 << 4),
        DirtyShape  = (1 << 5),
        DirtyMask   = (DirtyRadius | DirtyShadow | DirtyShape),
        Visible     = (1 << 6),
        Blending    = (1 << 7)
    };

    UCShapeResources* m_resources;
    quint16 m_radius;
    quint16 m_shadow;
    quint8 m_shape;
    quint8 m_flags;
};

#endif  // UCSHAPEFILLNODES_P_H
