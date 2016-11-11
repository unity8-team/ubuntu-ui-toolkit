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
    void updateShadowTexture(UCShapeType type, quint16 radius, quint16 shadow) {
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

// ################################
// # Fill center border materials #
// ################################

class UCShapeFillCenterBorderOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCenterBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/fillcenterborder.vert"));
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcenterborder_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "borderCoordAttrib", "colorAttrib", "borderColorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("borderTexture", 0);
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

class UCShapeFillCenterBorderShader : public UCShapeFillCenterBorderOpaqueShader
{
public:
    UCShapeFillCenterBorderShader() : UCShapeFillCenterBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(":/uc/privates/shaders/fillcenterborder.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCenterBorderOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCenterBorderOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCenterBorderMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCenterBorderMaterial() : m_textureId(0) {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ?
            new UCShapeFillCenterBorderOpaqueShader : new UCShapeFillCenterBorderShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        return reinterpret_cast<const UCShapeTextureProvider*>(other)->textureId() - m_textureId;
    }

    quint32 textureId(int) const Q_DECL_OVERRIDE {
        return m_textureId;
    }
    void updateBorderTexture(UCShapeType type, quint16 radius) {
        m_textureId = m_textureFactory.maskTexture(0, type, radius);
    }

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeFillCenterBorderResources : public UCShapeResources
{
public:
    UCShapeFillCenterBorderResources(
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
        float borderS, borderT;
        quint32 color;
        quint32 borderColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // borderS, borderT
            QSGGeometry::Attribute::create(2, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE)   // borderColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            4, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCenterBorderMaterial<false> m_material;
    UCShapeFillCenterBorderMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

// #######################################
// # Fill center shadow border materials #
// #######################################

class UCShapeFillCenterShadowBorderOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCenterShadowBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Vertex, QStringLiteral(
                ":/uc/privates/shaders/fillcentershadowborder.vert"));
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcentershadowborder_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "shadowCoordAttrib", "midShadowCoordAttrib", "borderCoordAttrib",
            "colorAttrib", "shadowColorAttrib", "borderColorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("shadowTexture", 0);
        program()->setUniformValue("borderTexture", 1);
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial*) Q_DECL_OVERRIDE {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        UCShapeTextureProvider* provider = dynamic_cast<UCShapeTextureProvider*>(newEffect);
        funcs->glActiveTexture(GL_TEXTURE1);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(1));
        funcs->glActiveTexture(GL_TEXTURE0);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(0));
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeFillCenterShadowBorderShader : public UCShapeFillCenterShadowBorderOpaqueShader
{
public:
    UCShapeFillCenterShadowBorderShader() : UCShapeFillCenterShadowBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcentershadowborder.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCenterShadowBorderOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCenterShadowBorderOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCenterShadowBorderMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCenterShadowBorderMaterial() : m_textureId{0, 0} {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ?
            new UCShapeFillCenterShadowBorderOpaqueShader : new UCShapeFillCenterShadowBorderShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        const UCShapeTextureProvider* provider =
            reinterpret_cast<const UCShapeTextureProvider*>(other);
        if (provider->textureId(0) == m_textureId[0]) {
            return provider->textureId(1) - m_textureId[1];
        } else {
            return -1;
        }
    }

    quint32 textureId(int index) const Q_DECL_OVERRIDE {
        DASSERT(index <= 1);
        return m_textureId[index];
    }
    void updateShadowTexture(UCShapeType type, quint16 radius, quint16 shadow) {
        m_textureId[0] = m_textureFactory.shadowTexture(0, type, radius, shadow);
    }
    void updateBorderTexture(UCShapeType type, quint16 radius) {
        m_textureId[1] = m_textureFactory.maskTexture(1, type, radius);
    }

private:
    UCShapeTextureFactory<2> m_textureFactory;
    quint32 m_textureId[2];
};

class UCShapeFillCenterShadowBorderResources : public UCShapeResources
{
public:
    UCShapeFillCenterShadowBorderResources(
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
        float borderS, borderT;
        quint32 color;
        quint32 shadowColor;
        quint32 borderColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // shadowS, shadowT
            QSGGeometry::Attribute::create(2, 2, GL_FLOAT),          // midShadowS, midShadowT
            QSGGeometry::Attribute::create(3, 2, GL_FLOAT),          // borderS, borderT
            QSGGeometry::Attribute::create(4, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(5, 4, GL_UNSIGNED_BYTE),  // shadowColor
            QSGGeometry::Attribute::create(6, 4, GL_UNSIGNED_BYTE)   // borderColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            7, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCenterShadowBorderMaterial<false> m_material;
    UCShapeFillCenterShadowBorderMaterial<true> m_opaqueMaterial;
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
        float shadowAngle, float shadowDistance, QRgb shadowColor, float borderSize,
        QRgb borderColor);

private:
    enum {
        HasColor          = (1 << 0),
        HasShadow         = (1 << 1),
        HasBorder         = (1 << 2),
        StyleMask         = (HasColor | HasShadow | HasBorder),
        Textured          = (HasShadow | HasBorder),
        DirtyRadius       = (1 << 3),
        DirtyShadow       = (1 << 4),
        DirtyShape        = (1 << 5),
        DirtyBorderRadius = (1 << 6),
        DirtyMask         = (DirtyRadius | DirtyShadow | DirtyShape | DirtyBorderRadius),
        Visible           = (1 << 7),
        Blending          = (1 << 8)
    };

    UCShapeResources* m_resources;
    quint16 m_radius;
    quint16 m_shadow;
    quint16 m_borderRadius;
    quint16 m_flags;
    quint8 m_shape;
};

// #################################
// # Fill corners shadow materials #
// #################################

class UCShapeFillCornersShadowOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCornersShadowOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/uc/privates/shaders/fillcornersshadow.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/fillcornersshadow_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "maskCoordAttrib", "shadowCoordAttrib", "midShadowCoordAttrib",
            "colorAttrib", "shadowColorAttrib", 0
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

class UCShapeFillCornersShadowShader : public UCShapeFillCornersShadowOpaqueShader
{
public:
    UCShapeFillCornersShadowShader() : UCShapeFillCornersShadowOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/fillcornersshadow.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCornersShadowOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCornersShadowOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCornersShadowMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCornersShadowMaterial() : m_textureId(0) {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ?
            new UCShapeFillCornersShadowOpaqueShader : new UCShapeFillCornersShadowShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        return reinterpret_cast<const UCShapeTextureProvider*>(other)->textureId() - m_textureId;
    }

    quint32 textureId(int) const Q_DECL_OVERRIDE {
        return m_textureId;
    }
    void updateShadowTexture(UCShapeType type, quint16 radius, quint16 shadow) {
        m_textureId = m_textureFactory.shadowTexture(0, type, radius, shadow);
    }

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeFillCornersShadowResources : public UCShapeResources
{
public:
    UCShapeFillCornersShadowResources(
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
        float maskS, maskT;
        float shadowS, shadowT;
        float midShadowS, midShadowT;
        quint32 color;
        quint32 shadowColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // maskS, maskT
            QSGGeometry::Attribute::create(2, 2, GL_FLOAT),          // shadowS, shadowT
            QSGGeometry::Attribute::create(3, 2, GL_FLOAT),          // midShadowS, midShadowT
            QSGGeometry::Attribute::create(4, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(5, 4, GL_UNSIGNED_BYTE)   // shadowColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            6, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCornersShadowMaterial<false> m_material;
    UCShapeFillCornersShadowMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

// #################################
// # Fill corners border materials #
// #################################

class UCShapeFillCornersBorderOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCornersBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/fillcornersborder.vert"));
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcornersborder_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "maskCoordAttrib", "borderCoordAttrib", "colorAttrib",
            "borderColorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("maskTexture", 0);
        program()->setUniformValue("borderTexture", 1);
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial*) Q_DECL_OVERRIDE {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        UCShapeTextureProvider* provider = dynamic_cast<UCShapeTextureProvider*>(newEffect);
        funcs->glActiveTexture(GL_TEXTURE1);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(1));
        funcs->glActiveTexture(GL_TEXTURE0);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(0));
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeFillCornersBorderShader : public UCShapeFillCornersBorderOpaqueShader
{
public:
    UCShapeFillCornersBorderShader() : UCShapeFillCornersBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcornersborder.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCornersBorderOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCornersBorderOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCornersBorderMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCornersBorderMaterial() : m_textureId{0, 0} {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ?
            new UCShapeFillCornersBorderOpaqueShader : new UCShapeFillCornersBorderShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        const UCShapeTextureProvider* provider =
            reinterpret_cast<const UCShapeTextureProvider*>(other);
        if (provider->textureId(0) == m_textureId[0]) {
            return provider->textureId(1) - m_textureId[1];
        } else {
            return -1;
        }
    }

    quint32 textureId(int index) const Q_DECL_OVERRIDE {
        DASSERT(index <= 1);
        return m_textureId[index];
    }
    void updateMaskTexture(UCShapeType type, quint16 radius) {
        m_textureId[0] = m_textureFactory.maskTexture(0, type, radius);
    }
    void updateBorderTexture(UCShapeType type, quint16 radius) {
        m_textureId[1] = m_textureFactory.maskTexture(1, type, radius);
    }

private:
    UCShapeTextureFactory<2> m_textureFactory;
    quint32 m_textureId[2];
};

class UCShapeFillCornersBorderResources : public UCShapeResources
{
public:
    UCShapeFillCornersBorderResources(
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
        float maskS, maskT;
        float borderS, borderT;
        quint32 color;
        quint32 borderColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // maskS, maskT
            QSGGeometry::Attribute::create(2, 2, GL_FLOAT),          // borderS, borderT
            QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(4, 4, GL_UNSIGNED_BYTE)   // borderColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            5, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCornersBorderMaterial<false> m_material;
    UCShapeFillCornersBorderMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

// ########################################
// # Fill corners shadow border materials #
// ########################################

class UCShapeFillCornersShadowBorderOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeFillCornersShadowBorderOpaqueShader() {
        setShaderSourceFile(
            QOpenGLShader::Vertex, QStringLiteral(
                ":/uc/privates/shaders/fillcornersshadowborder.vert"));
        setShaderSourceFile(
            QOpenGLShader::Fragment, QStringLiteral(
                ":/uc/privates/shaders/fillcornersshadowborder_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "maskCoordAttrib", "shadowCoordAttrib", "midShadowCoordAttrib",
            "borderCoordAttrib", "colorAttrib", "shadowColorAttrib", "borderColorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("shadowtexture", 0);
        program()->setUniformValue("borderTexture", 1);
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial*) Q_DECL_OVERRIDE {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        UCShapeTextureProvider* provider = dynamic_cast<UCShapeTextureProvider*>(newEffect);
        funcs->glActiveTexture(GL_TEXTURE1);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(1));
        funcs->glActiveTexture(GL_TEXTURE0);
        funcs->glBindTexture(GL_TEXTURE_2D, provider->textureId(0));
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeFillCornersShadowBorderShader : public UCShapeFillCornersShadowBorderOpaqueShader
{
public:
    UCShapeFillCornersShadowBorderShader() : UCShapeFillCornersShadowBorderOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/fillcornersshadowborder.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeFillCornersShadowBorderOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeFillCornersShadowBorderOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeFillCornersShadowBorderMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeFillCornersShadowBorderMaterial() : m_textureId{0, 0} {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ?
            new UCShapeFillCornersShadowBorderOpaqueShader :
            new UCShapeFillCornersShadowBorderShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        const UCShapeTextureProvider* provider =
            reinterpret_cast<const UCShapeTextureProvider*>(other);
        if (provider->textureId(0) == m_textureId[0]) {
            return provider->textureId(1) - m_textureId[1];
        } else {
            return -1;
        }
    }

    quint32 textureId(int index) const Q_DECL_OVERRIDE {
        DASSERT(index <= 1);
        return m_textureId[index];
    }
    void updateShadowTexture(UCShapeType type, quint16 radius, quint16 shadow) {
        m_textureId[0] = m_textureFactory.shadowTexture(0, type, radius, shadow);
    }
    void updateBorderTexture(UCShapeType type, quint16 radius) {
        m_textureId[1] = m_textureFactory.maskTexture(1, type, radius);
    }

private:
    UCShapeTextureFactory<2> m_textureFactory;
    quint32 m_textureId[2];
};

class UCShapeFillCornersShadowBorderResources : public UCShapeResources
{
public:
    UCShapeFillCornersShadowBorderResources(
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
        float maskS, maskT;
        float shadowS, shadowT;
        float midShadowS, midShadowT;
        float borderS, borderT;
        quint32 color;
        quint32 shadowColor;
        quint32 borderColor;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),    // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),          // maskS, maskT
            QSGGeometry::Attribute::create(2, 2, GL_FLOAT),          // shadowS, shadowT
            QSGGeometry::Attribute::create(3, 2, GL_FLOAT),          // midShadowS, midShadowT
            QSGGeometry::Attribute::create(4, 2, GL_FLOAT),          // borderS, borderT
            QSGGeometry::Attribute::create(5, 4, GL_UNSIGNED_BYTE),  // color
            QSGGeometry::Attribute::create(6, 4, GL_UNSIGNED_BYTE),  // shadowColor
            QSGGeometry::Attribute::create(7, 4, GL_UNSIGNED_BYTE)   // borderColor
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            8, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeFillCornersShadowBorderMaterial<false> m_material;
    UCShapeFillCornersShadowBorderMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
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
        float shadowAngle, float shadowDistance, QRgb shadowColor, float borderSize,
        QRgb borderColor);

private:
    enum {
        HasColor          = (1 << 0),
        HasShadow         = (1 << 1),
        HasBorder         = (1 << 2),
        StyleMask         = (HasColor | HasShadow | HasBorder),
        DirtyRadius       = (1 << 3),
        DirtyShadow       = (1 << 4),
        DirtyShape        = (1 << 5),
        DirtyBorderRadius = (1 << 6),
        DirtyMask         = (DirtyRadius | DirtyShadow | DirtyShape | DirtyBorderRadius),
        Visible           = (1 << 7),
        Blending          = (1 << 8)
    };

    UCShapeResources* m_resources;
    quint16 m_radius;
    quint16 m_shadow;
    quint16 m_borderRadius;
    quint16 m_flags;
    quint8 m_shape;
};

#endif  // UCSHAPEFILLNODES_P_H
