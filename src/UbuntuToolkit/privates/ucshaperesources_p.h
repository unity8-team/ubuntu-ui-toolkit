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

#ifndef UCSHAPERESOURCES_P_H
#define UCSHAPERESOURCES_P_H

#include <QtQuick/QSGMaterial>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>

class UCShapeResources
{
public:
    virtual ~UCShapeResources() {}
    virtual QSGMaterial* material() = 0;
    virtual QSGMaterial* opaqueMaterial() = 0;
    virtual QSGGeometry* geometry() = 0;
};

class UCShapeTextureProvider
{
public:
    virtual ~UCShapeTextureProvider() {}
    virtual quint32 textureId(int index = 0) const = 0;
};

// ###################
// # Color materials #
// ###################

class UCShapeColorOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeColorOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/uc/privates/shaders/color.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/color_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = { "positionAttrib", "colorAttrib", 0 };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial*, QSGMaterial*) Q_DECL_OVERRIDE {
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeColorShader : public UCShapeColorOpaqueShader
{
public:
    UCShapeColorShader() : UCShapeColorOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/color.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeColorOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeColorOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeColorMaterial : public QSGMaterial
{
public:
    UCShapeColorMaterial() {
        setFlag(Blending, !opaque);
    }
    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new UCShapeColorOpaqueShader : new UCShapeColorShader;
    }
    int compare(const QSGMaterial*) const Q_DECL_OVERRIDE {
        return 0;
    }
};

class UCShapeColorResources : public UCShapeResources
{
public:
    UCShapeColorResources(int vertexCount, int indexCount, int indexType = GL_UNSIGNED_SHORT)
        : m_geometry(attributeSet(), vertexCount, indexCount, indexType) {
        m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
        m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
        m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    }

    QSGMaterial* material() Q_DECL_OVERRIDE { return &m_material; }
    QSGMaterial* opaqueMaterial() Q_DECL_OVERRIDE { return &m_opaqueMaterial; }
    QSGGeometry* geometry() Q_DECL_OVERRIDE { return &m_geometry; }

    struct Vertex { float x, y; quint32 color; };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),   // x, y
            QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE)  // color
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            2, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeColorMaterial<false> m_material;
    UCShapeColorMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

// ########################
// # Color mask materials #
// ########################

class UCShapeColorMaskOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeColorMaskOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/uc/privates/shaders/colormask.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/colormask_opaque.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = {
            "positionAttrib", "maskCoordAttrib", "colorAttrib", 0
        };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        program()->bind();
        program()->setUniformValue("maskTexture", 0);
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

class UCShapeColorMaskShader : public UCShapeColorMaskOpaqueShader
{
public:
    UCShapeColorMaskShader() : UCShapeColorMaskOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/colormask_opaque.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeColorMaskOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeColorMaskOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeColorMaskMaterial : public QSGMaterial, public UCShapeTextureProvider
{
public:
    UCShapeColorMaskMaterial() : m_textureId(0) {
        setFlag(Blending, !opaque);
    }

    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new UCShapeColorMaskOpaqueShader : new UCShapeColorMaskShader;
    }
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE {
        return reinterpret_cast<const UCShapeTextureProvider*>(other)->textureId() - m_textureId;
    }

    quint32 textureId(int) const Q_DECL_OVERRIDE {
        return m_textureId;
    }
    void updateTexture(UCShapeType type, quint16 radius) {
        DASSERT(radius > 0);
        m_textureId = m_textureFactory.maskTexture(0, type, radius);
    }

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeColorMaskResources : public UCShapeResources
{
public:
    UCShapeColorMaskResources(
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
        quint32 color;
    };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),   // x, y
            QSGGeometry::Attribute::create(1, 2, GL_FLOAT),         // maskS, maskT
            QSGGeometry::Attribute::create(2, 4, GL_UNSIGNED_BYTE)  // color
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            3, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeColorMaskMaterial<false> m_material;
    UCShapeColorMaskMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

#endif  // UCSHAPERESOURCES_P_H
