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

#ifndef UC_UBUNTU_SHAPE_H_
#define UC_UBUNTU_SHAPE_H_

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QtQuick/qsgtexture.h>
#include <QtGui/QOpenGLFunctions>

// Remove once QtDeclarative patches are merged.
// #define HAS_QTDECLARATIVE_UPDATECLIPRECT
// #define HAS_QTDECLARATIVE_CLAMPTOBORDER

struct MaterialData
{
    MaterialData();
    QSGTextureProvider* provider[3];
    QVector4D backgroundColorPremultiplied[2];
    QVector4D overlayColorPremultiplied;
    QVector4D shadowColorPremultiplied[2];
    QVector4D overlaySteps;
    QVector4D cornerTransform;
    QVector4D noCornerTransform;
    QVector2D shadowSize;
    QVector2D shadowOffset[2];
    QVector2D sourceOpacity;
    QVector4D sourceFill[2];
    float adjustedCornerRadius;
    int backgroundGradientIndex;
    QSGTexture::WrapMode wrap[2][2];
    bool hasOverlay;
    bool hasOverlayBlendingSourceOver;
    bool hasShadowOut;
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    bool hasSoftwareClampToBorder[2][2];
#endif
};

struct ShadowBorder
{
    ShadowBorder(float l, float r, float t, float b) : left(l), right(r), top(t), bottom(b) {}
    float left, right, top, bottom;
};

// QtQuick item.

class UCUbuntuShape : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)       // Deprecated
    Q_PROPERTY(QColor gradientColor READ gradientColor WRITE setGradientColor
               NOTIFY gradientColorChanged)                                      // Deprecated
    Q_PROPERTY(QString radius READ radius WRITE setRadius NOTIFY radiusChanged)  // Deprecated
    Q_PROPERTY(QString borderSource READ borderSource WRITE setBorderSource
               NOTIFY borderSourceChanged)                                       // Deprecated
    Q_PROPERTY(QVariant image READ image WRITE setImage NOTIFY imageChanged)     // Deprecated

    // Shape properties.
    Q_ENUMS(ShapeMode)
    Q_PROPERTY(ShapeMode shapeMode READ shapeMode WRITE setShapeMode NOTIFY shapeModeChanged)
    Q_PROPERTY(float cornerRadius READ cornerRadius WRITE setCornerRadius
               NOTIFY cornerRadiusChanged)
            //Q_PROPERTY(Corner disabledCorners READ disabledCorners WRITE setDisabledCorners)
    Q_PROPERTY(QVariant customShape READ customShape WRITE setCustomShape NOTIFY customShapeChanged)

    // Background properties.
    Q_ENUMS(BackgroundMode)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor
               NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor secondaryBackgroundColor READ secondaryBackgroundColor
               WRITE setSecondaryBackgroundColor NOTIFY secondaryBackgroundColorChanged)
    Q_PROPERTY(BackgroundMode backgroundMode READ backgroundMode WRITE setBackgroundMode
               NOTIFY backgroundModeChanged)

    // Overlay properties.
    Q_ENUMS(Blending)
    Q_PROPERTY(QRectF overlayGeometry READ overlayGeometry WRITE setOverlayGeometry
               NOTIFY overlayGeometryChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor
               NOTIFY overlayColorChanged)
    Q_PROPERTY(Blending overlayBlending READ overlayBlending WRITE setOverlayBlending
               NOTIFY overlayBlendingChanged)

    // Shadow properties.
    Q_PROPERTY(float shadowSizeIn READ shadowSizeIn WRITE setShadowSizeIn
               NOTIFY shadowSizeInChanged)
    Q_PROPERTY(QColor shadowColorIn READ shadowColorIn WRITE setShadowColorIn
               NOTIFY shadowColorInChanged)
    Q_PROPERTY(float shadowDistanceIn READ shadowDistanceIn WRITE setShadowDistanceIn
               NOTIFY shadowDistanceInChanged)
    Q_PROPERTY(float shadowAngleIn READ shadowAngleIn WRITE setShadowAngleIn
               NOTIFY shadowAngleInChanged)
    Q_PROPERTY(float shadowSizeOut READ shadowSizeOut WRITE setShadowSizeOut
               NOTIFY shadowSizeOutChanged)
    Q_PROPERTY(QColor shadowColorOut READ shadowColorOut WRITE setShadowColorOut
               NOTIFY shadowColorOutChanged)
    Q_PROPERTY(float shadowDistanceOut READ shadowDistanceOut WRITE setShadowDistanceOut
               NOTIFY shadowDistanceOutChanged)
    Q_PROPERTY(float shadowAngleOut READ shadowAngleOut WRITE setShadowAngleOut
               NOTIFY shadowAngleOutChanged)

    // Source properties.
    Q_ENUMS(FillMode)
    Q_ENUMS(WrapMode)
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float sourceOpacity READ sourceOpacity WRITE setSourceOpacity
               NOTIFY sourceOpacityChanged)
    Q_PROPERTY(FillMode sourceFillMode READ sourceFillMode WRITE setSourceFillMode
               NOTIFY sourceFillModeChanged)
    Q_PROPERTY(WrapMode sourceHorizontalWrapMode READ sourceHorizontalWrapMode
               WRITE setSourceHorizontalWrapMode NOTIFY sourceHorizontalWrapModeChanged)
    Q_PROPERTY(WrapMode sourceVerticalWrapMode READ sourceVerticalWrapMode
               WRITE setSourceVerticalWrapMode NOTIFY sourceVerticalWrapModeChanged)
    Q_PROPERTY(HAlignment sourceHorizontalAlignment READ sourceHorizontalAlignment
               WRITE setSourceHorizontalAlignment NOTIFY sourceHorizontalAlignmentChanged)
    Q_PROPERTY(VAlignment sourceVerticalAlignment READ sourceVerticalAlignment
               WRITE setSourceVerticalAlignment NOTIFY sourceVerticalAlignmentChanged)
    Q_PROPERTY(QVector2D sourceTranslation READ sourceTranslation WRITE setSourceTranslation
               NOTIFY sourceTranslationChanged)
    Q_PROPERTY(QVector2D sourceScale READ sourceScale WRITE setSourceScale
               NOTIFY sourceScaleChanged)
    Q_PROPERTY(QVariant secondarySource READ secondarySource WRITE setSecondarySource
               NOTIFY secondarySourceChanged)
    Q_PROPERTY(float secondarySourceOpacity READ secondarySourceOpacity
               WRITE setSecondarySourceOpacity NOTIFY secondarySourceOpacityChanged)
    Q_PROPERTY(FillMode secondarySourceFillMode READ secondarySourceFillMode
               WRITE setSecondarySourceFillMode NOTIFY secondarySourceFillModeChanged)
    Q_PROPERTY(WrapMode secondarySourceHorizontalWrapMode READ secondarySourceHorizontalWrapMode
               WRITE setSecondarySourceHorizontalWrapMode
               NOTIFY secondarySourceHorizontalWrapModeChanged)
    Q_PROPERTY(WrapMode secondarySourceVerticalWrapMode READ secondarySourceVerticalWrapMode
               WRITE setSecondarySourceVerticalWrapMode
               NOTIFY secondarySourceVerticalWrapModeChanged)
    Q_PROPERTY(HAlignment secondarySourceHorizontalAlignment READ secondarySourceHorizontalAlignment
               WRITE setSecondarySourceHorizontalAlignment
               NOTIFY secondarySourceHorizontalAlignmentChanged)
    Q_PROPERTY(VAlignment secondarySourceVerticalAlignment READ secondarySourceVerticalAlignment
               WRITE setSecondarySourceVerticalAlignment
               NOTIFY secondarySourceVerticalAlignmentChanged)
    Q_PROPERTY(QVector2D secondarySourceTranslation READ secondarySourceTranslation
               WRITE setSecondarySourceTranslation NOTIFY secondarySourceTranslationChanged)
    Q_PROPERTY(QVector2D secondarySourceScale READ secondarySourceScale
               WRITE setSecondarySourceScale NOTIFY secondarySourceScaleChanged)

public:
    // Explicitly enumerated values are used in look-up tables and shouldn't be changed!
    enum ShapeMode { Rounded, Custom };
    enum BackgroundMode { BackgroundColor = 0, HorizontalGradient = 1, VerticalGradient = 2 };
    enum Blending { SourceOver, Multiply };
    enum HAlignment { AlignLeft = 0, AlignHCenter = 1, AlignRight = 2 };
    enum VAlignment { AlignTop = 0, AlignVCenter = 1, AlignBottom = 2 };
    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Pad };
    enum WrapMode { Repeat = 0, ClampToEdge = 1, ClampToBorder = 2 };

    UCUbuntuShape(QQuickItem* parent=0);
    ~UCUbuntuShape();

    // UCUbuntuShape methods.
    QColor color() const { return backgroundColor_[0]; }              // Deprecated
    void setColor(const QColor& color);                               // Deprecated
    QColor gradientColor() const { return backgroundColor_[1]; }      // Deprecated
    void setGradientColor(const QColor& gradientColor);               // Deprecated
    QString radius() const { return QString(); }                      // Deprecated
    void setRadius(const QString& radius);                            // Deprecated
    QString borderSource() const { return QString(""); }              // Deprecated
    void setBorderSource(const QString& borderSource);                // Deprecated
    QVariant image() const { return QVariant::fromValue(item_[0]); }  // Deprecated
    void setImage(QVariant image);                                    // Deprecated
    ShapeMode shapeMode() const { return shapeMode_; }
    void setShapeMode(ShapeMode shapeMode);
    float cornerRadius() const { return cornerRadius_; }
    void setCornerRadius(float cornerRadius);
    QVariant customShape() const { return QVariant::fromValue(item_[3]); }
    void setCustomShape(QVariant customShape);
    QColor backgroundColor() const { return backgroundColor_[0]; }
    void setBackgroundColor(const QColor& backgroundColor);
    QColor secondaryBackgroundColor() const { return backgroundColor_[1]; }
    void setSecondaryBackgroundColor(const QColor& secondaryBackgroundColor);
    BackgroundMode backgroundMode() const { return backgroundMode_; }
    void setBackgroundMode(BackgroundMode backgroundMode);
    QRectF overlayGeometry() const { return overlayGeometry_; }
    void setOverlayGeometry(const QRectF& overlayGeometry);
    QColor overlayColor() const { return overlayColor_; }
    void setOverlayColor(const QColor& overlayColor);
    Blending overlayBlending() const { return overlayBlending_; }
    void setOverlayBlending(Blending blending);
    float shadowSizeIn() const { return shadowSize_[0]; }
    void setShadowSizeIn(float shadowSizeIn);
    QColor shadowColorIn() const { return shadowColor_[0]; }
    void setShadowColorIn(const QColor& shadowColorIn);
    float shadowDistanceIn() const { return shadowDistance_[0]; }
    void setShadowDistanceIn(float shadowDistanceIn);
    float shadowAngleIn() const { return shadowAngle_[0]; }
    void setShadowAngleIn(float shadowAngleIn);
    float shadowSizeOut() const { return shadowSize_[1]; }
    void setShadowSizeOut(float shadowSizeOut);
    QColor shadowColorOut() const { return shadowColor_[1]; }
    void setShadowColorOut(const QColor& shadowColorOut);
    float shadowDistanceOut() const { return shadowDistance_[1]; }
    void setShadowDistanceOut(float shadowDistanceOut);
    float shadowAngleOut() const { return shadowAngle_[1]; }
    void setShadowAngleOut(float shadowAngleOut);
    QVariant source() const { return QVariant::fromValue(item_[0]); }
    void setSource(QVariant source);
    float sourceOpacity() const { return sourceOpacity_[0]; }
    void setSourceOpacity(float sourceOpacity);
    FillMode sourceFillMode() const { return sourceFillMode_[0]; }
    void setSourceFillMode(FillMode sourceFillMode);
    WrapMode sourceHorizontalWrapMode() const { return sourceWrapMode_[0][0]; }
    void setSourceHorizontalWrapMode(WrapMode sourceHorizontalWrapMode);
    WrapMode sourceVerticalWrapMode() const { return sourceWrapMode_[0][1]; }
    void setSourceVerticalWrapMode(WrapMode sourceVerticalWrapMode);
    HAlignment sourceHorizontalAlignment() const { return sourceHorizontalAlignment_[0]; }
    void setSourceHorizontalAlignment(HAlignment sourceHorizontalAlignment);
    VAlignment sourceVerticalAlignment() const { return sourceVerticalAlignment_[0]; }
    void setSourceVerticalAlignment(VAlignment sourceVerticalAlignment);
    QVector2D sourceTranslation() const { return sourceTranslation_[0]; }
    void setSourceTranslation(const QVector2D& sourceTranslation);
    QVector2D sourceScale() const { return sourceScale_[0]; }
    void setSourceScale(const QVector2D& sourceScale);
    QVariant secondarySource() const { return QVariant::fromValue(item_[1]); }
    void setSecondarySource(QVariant secondarySource);
    float secondarySourceOpacity() const { return sourceOpacity_[1]; }
    void setSecondarySourceOpacity(float secondarySourceOpacity);
    FillMode secondarySourceFillMode() const { return sourceFillMode_[1]; }
    void setSecondarySourceFillMode(FillMode secondarySourceFillMode);
    WrapMode secondarySourceHorizontalWrapMode() const { return sourceWrapMode_[1][0]; }
    void setSecondarySourceHorizontalWrapMode(WrapMode secondarySourceHorizontalWrapMode);
    WrapMode secondarySourceVerticalWrapMode() const { return sourceWrapMode_[1][1]; }
    void setSecondarySourceVerticalWrapMode(WrapMode secondarySourceVerticalWrapMode);
    HAlignment secondarySourceHorizontalAlignment() const { return sourceHorizontalAlignment_[1]; }
    void setSecondarySourceHorizontalAlignment(HAlignment secondarySourceHorizontalAlignment);
    VAlignment secondarySourceVerticalAlignment() const { return sourceVerticalAlignment_[1]; }
    void setSecondarySourceVerticalAlignment(VAlignment secondarySourceVerticalAlignment);
    QVector2D secondarySourceTranslation() const { return sourceTranslation_[1]; }
    void setSecondarySourceTranslation(const QVector2D& secondarySourceTranslation);
    QVector2D secondarySourceScale() const { return sourceScale_[1]; }
    void setSecondarySourceScale(const QVector2D& secondarySourceScale);

    // QQuickItem methods.
    QRectF clipRect() const;

Q_SIGNALS:
    void colorChanged();          // Deprecated
    void gradientColorChanged();  // Deprecated
    void radiusChanged();         // Deprecated
    void borderSourceChanged();   // Deprecated
    void imageChanged();          // Deprecated
    void shapeModeChanged();
    void customShapeChanged();
    void cornerRadiusChanged();
    void backgroundColorChanged();
    void secondaryBackgroundColorChanged();
    void backgroundModeChanged();
    void overlayGeometryChanged();
    void overlayColorChanged();
    void overlayBlendingChanged();
    void shadowSizeInChanged();
    void shadowColorInChanged();
    void shadowDistanceInChanged();
    void shadowAngleInChanged();
    void shadowSizeOutChanged();
    void shadowColorOutChanged();
    void shadowDistanceOutChanged();
    void shadowAngleOutChanged();
    void sourceChanged();
    void sourceOpacityChanged();
    void sourceFillModeChanged();
    void sourceHorizontalWrapModeChanged();
    void sourceVerticalWrapModeChanged();
    void sourceHorizontalAlignmentChanged();
    void sourceVerticalAlignmentChanged();
    void sourceTranslationChanged();
    void sourceScaleChanged();
    void secondarySourceChanged();
    void secondarySourceOpacityChanged();
    void secondarySourceFillModeChanged();
    void secondarySourceHorizontalWrapModeChanged();
    void secondarySourceVerticalWrapModeChanged();
    void secondarySourceHorizontalAlignmentChanged();
    void secondarySourceVerticalAlignmentChanged();
    void secondarySourceTranslationChanged();
    void secondarySourceScaleChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

private:
    void updateShadowLayout();
    void updateWrapMode(int index, int direction, WrapMode wrapMode);
    void updateFillMode(int index, QSGTexture* texture);
    Q_SLOT void gridUnitChanged();

    QString radius_;  // Deprecated
    QQuickItem* item_[3];
    QRectF geometry_;
    QRectF overlayGeometry_;
    QColor backgroundColor_[2];
    QColor overlayColor_;
    QColor shadowColor_[2];
    QVector2D sourceTranslation_[2];
    QVector2D sourceScale_[2];
    ShapeMode shapeMode_;
    BackgroundMode backgroundMode_;
    Blending overlayBlending_;
    FillMode sourceFillMode_[2];
    WrapMode sourceWrapMode_[2][2];
    HAlignment sourceHorizontalAlignment_[2];
    VAlignment sourceVerticalAlignment_[2];
    float cornerRadius_;
    float shadowSize_[2];
    float shadowDistance_[2];
    float shadowAngle_[2];
    ShadowBorder shadowBorder_;
    float sourceOpacity_[2];
    float gridUnit_;
    MaterialData materialData_;

    Q_DISABLE_COPY(UCUbuntuShape)
};

// Scene graph textured material.

class UCUbuntuShapeMaterial : public QSGMaterial
{
public:
    UCUbuntuShapeMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    MaterialData* data() { return &data_; }
    void setData(MaterialData* data);

private:
    MaterialData data_;
};

// Scene graph textured material shader.

class UCUbuntuShapeShader : public QSGMaterialShader
{
public:
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(const RenderState& state, QSGMaterial* newEffect,
                             QSGMaterial* oldEffect);

private:
    virtual const char* vertexShader() const;
    virtual const char* fragmentShader() const;

    QOpenGLFunctions* openglFunctions_;
    int matrixId_;
    int qtOpacityId_;
    int bgColorId_;
    int overlayColorId_;
    int overlayStepsId_;
    int shadowColorId_;
    int radiusId_;
    int cornerTransformId_;
    int noCornerTransformId_;
    int shadowSizeId_;
    int shadowOffsetId_;
    int bgGradientIndexId_;
    int sourceOpacityId_;
    int sourceFillId_;
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    int borderId_;
#endif
    int featuresId_;
};

// Scene graph node.

class UCUbuntuShapeNode : public QObject, public QSGGeometryNode
{
    Q_OBJECT

public:
    struct Vertex { float position[2]; float textureCoord[2]; };

    UCUbuntuShapeNode(UCUbuntuShape* item);
    UCUbuntuShapeMaterial* material() { return &material_; }
    void setVertices(const QRectF& geometry, const ShadowBorder& shadowBorder);

private:
    UCUbuntuShape* item_;
    QSGGeometry geometry_;
    UCUbuntuShapeMaterial material_;
};

QML_DECLARE_TYPE(UCUbuntuShape)

#endif // UC_UBUNTU_SHAPE_H_
