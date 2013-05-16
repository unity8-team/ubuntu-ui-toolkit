/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef UBUNTU_COMPONENTS_SHAPE_H
#define UBUNTU_COMPONENTS_SHAPE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QtQuick/qsgtexture.h>
#include <QtGui/QOpenGLFunctions>

// QtQuick item.

struct MaterialData
{
    MaterialData();
    QSGTextureProvider* sourceTextureProvider;
    QVector4D baseColorPremultiplied;
    QVector4D overlayColorPremultiplied;
    QVector4D shadowColorInPremultiplied;
    QVector4D shadowColorOutPremultiplied;
    QVector4D overlayGeometry;
    QVector4D shadowRadiusOut;
    QVector4D shadowRadiusIn;
    float squirclePow;
};

class ShapeItem2 : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(FillMode)
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_PROPERTY(float squirclePow READ squirclePow WRITE setSquirclePow NOTIFY squirclePowChanged)
    Q_PROPERTY(float cornerRadius READ cornerRadius WRITE setCornerRadius
               NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QRectF overlayGeometry READ overlayGeometry WRITE setOverlayGeometry
               NOTIFY overlayGeometryChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor
               NOTIFY overlayColorChanged)
    Q_PROPERTY(QVector4D shadowRadiusIn READ shadowRadiusIn WRITE setShadowRadiusIn
               NOTIFY shadowRadiusInChanged)
    Q_PROPERTY(QColor shadowColorIn READ shadowColorIn WRITE setShadowColorIn
               NOTIFY shadowColorInChanged)
    Q_PROPERTY(QVector4D shadowRadiusOut READ shadowRadiusOut WRITE setShadowRadiusOut
               NOTIFY shadowRadiusOutChanged)
    Q_PROPERTY(QColor shadowColorOut READ shadowColorOut WRITE setShadowColorOut
               NOTIFY shadowColorOutChanged)
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(HAlignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment
               NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment
               NOTIFY verticalAlignmentChanged)

public:
    enum HAlignment { AlignLeft = Qt::AlignLeft, AlignRight = Qt::AlignRight,
                      AlignHCenter = Qt::AlignHCenter };
    enum VAlignment { AlignTop = Qt::AlignTop, AlignBottom = Qt::AlignBottom,
                      AlignVCenter = Qt::AlignVCenter };
    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically,
                    TileHorizontally, Pad };

    ShapeItem2(QQuickItem* parent=0);
    ~ShapeItem2();
    float squirclePow() const { return materialData_.squirclePow; }
    void setSquirclePow(float squirclePow);
    float cornerRadius() const { return cornerRadius_; }
    void setCornerRadius(float cornerRadius);
    QColor baseColor() const { return baseColor_; }
    void setBaseColor(const QColor& baseColor);
    QRectF overlayGeometry() const;
    void setOverlayGeometry(const QRectF& overlayGeometry);
    QColor overlayColor() const { return overlayColor_; }
    void setOverlayColor(const QColor& overlayColor);
    QVector4D shadowRadiusIn() const { return materialData_.shadowRadiusIn; }
    void setShadowRadiusIn(const QVector4D& shadowRadiusIn);
    QColor shadowColorIn() const { return shadowColorIn_; }
    void setShadowColorIn(const QColor& shadowColorIn);
    QVector4D shadowRadiusOut() const { return materialData_.shadowRadiusOut; }
    void setShadowRadiusOut(const QVector4D& shadowRadiusOut);
    QColor shadowColorOut() const { return shadowColorOut_; }
    void setShadowColorOut(const QColor& shadowColorOut);
    QVariant source() const { return QVariant::fromValue(source_); }
    void setSource(QVariant source);
    FillMode fillMode() const { return fillMode_; }
    void setFillMode(FillMode fillMode);
    HAlignment horizontalAlignment() const { return horizontalAlignment_; }
    void setHorizontalAlignment(HAlignment horizontalAlignment);
    VAlignment verticalAlignment() const { return verticalAlignment_; }
    void setVerticalAlignment(VAlignment verticalAlignment);

Q_SIGNALS:
    void squirclePowChanged();
    void cornerRadiusChanged();
    void baseColorChanged();
    void overlayGeometryChanged();
    void overlayColorChanged();
    void shadowRadiusInChanged();
    void shadowColorInChanged();
    void shadowRadiusOutChanged();
    void shadowColorOutChanged();
    void sourceChanged();
    void fillModeChanged();
    void horizontalAlignmentChanged();
    void verticalAlignmentChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

private:
    QQuickItem* source_;
    QRectF geometry_;
    QColor baseColor_;
    QColor overlayColor_;
    QColor shadowColorIn_;
    QColor shadowColorOut_;
    FillMode fillMode_;
    HAlignment horizontalAlignment_;
    VAlignment verticalAlignment_;
    float cornerRadius_;
    MaterialData materialData_;

    Q_DISABLE_COPY(ShapeItem2)
};

// Scene graph textured material.

class ShapeMaterial2 : public QSGMaterial
{
public:
    ShapeMaterial2();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    MaterialData* data() { return &data_; }
    void setData(MaterialData* data);

private:
    MaterialData data_;
};

// Scene graph textured material shader.

class ShapeShader2 : public QSGMaterialShader
{
public:
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(const RenderState& state, QSGMaterial* newEffect,
                             QSGMaterial* oldEffect);

private:
    virtual const char* vertexShader() const;
    virtual const char* fragmentShader() const;

    int matrixId_;
    int opacityId_;
    int baseColorId_;
    // int overlayColorId_;
    // int shadowColorInId_;
    int shadowColorOutId_;
    // int overlayGeometryId_;
    // int shadowRadiusInId_;
    int shadowRadiusOutId_;
    int squirclePowId_;
};

// Scene graph node.

class ShapeNode2 : public QObject, public QSGGeometryNode
{
    Q_OBJECT

public:
    struct Vertex {
        float position[2];
        float shapeCoordinate[4];
        float sourceCoordinate[2];
        // float padding[0];  // Ensure 32 bytes alignment.
    };

    ShapeNode2(ShapeItem2* item);
    ShapeMaterial2* material() { return &material_; }
    void setVertices(const QRectF& geometry, float noShadowWidth, float noShadowHeight,
                     float cornerRadius, const QVector4D& shadowRadiusOut,
                     const QVector4D& shadowRadiusIn, QSGTexture* texture,
                     ShapeItem2::FillMode fillMode, ShapeItem2::HAlignment horizontalAlignment,
                     ShapeItem2::VAlignment verticalAlignment);

private:
    ShapeItem2* item_;
    QSGGeometry geometry_;
    ShapeMaterial2 material_;
};

QML_DECLARE_TYPE(ShapeItem2)

#endif // UBUNTU_COMPONENTS_SHAPE_H
