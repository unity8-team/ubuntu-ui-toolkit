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

#ifndef UCFRAME_H
#define UCFRAME_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>
#include "texturefactory.h"
#include "utils.h"

// Renders the frame (border) of a shape.
class UCFrame : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Shape)

    // Shape to use at corners.
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)

    // Thickness of the frame in pixels.
    Q_PROPERTY(qreal thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)

    // Radius of the shape in pixels.
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

    // Frame color. Translucent colors are supported.
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCFrame(QQuickItem* parent = 0);

    enum Shape { Squircle = Texture::Squircle, Circle = Texture::Circle };

    Shape shape() const { return static_cast<Shape>(m_shape); }
    void setShape(Shape shape);
    qreal thickness() const { return m_thickness; }
    void setThickness(qreal thickness);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QColor color() const {
        return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void shapeChanged();
    void thicknessChanged();
    void radiusChanged();
    void colorChanged();

private:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    enum { DirtyShape = (1 << 0), DirtyCornerVisibility = (1 << 1) };

    QRgb m_color;
    quint16 m_thickness;
    quint8 m_radius;
    quint8 m_shape : 1;
    quint8 __padding : 7;
    quint8 m_flags;

    Q_DISABLE_COPY(UCFrame)
};

class UCFrameCornerMaterial : public QSGMaterial
{
public:
    UCFrameCornerMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;

    quint32 outerTextureId() const { return m_textureId[0]; }
    quint32 innerTextureId() const { return m_textureId[1]; }
    void updateTexture(int index, UCFrame::Shape shape, int radius) {
        DASSERT(index >= 0 && index < 2);
        DASSERT(radius >= 0);
        m_textureId[index] =
            m_textureFactory.shapeTexture(index, static_cast<Texture::Shape>(shape), radius);
    }

private:
    TextureFactory<2> m_textureFactory;
    quint32 m_textureId[2];
};

class UCFrameCornerNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, outerS, outerT, innerS, innerT; quint32 color; };

    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCFrameCornerNode(UCFrame::Shape shape, bool visible);
    virtual void preprocess();
    virtual bool isSubtreeBlocked() const { return m_visible == 0; }

    void setVisible(bool visible);
    void setShape(UCFrame::Shape shape) { m_newShape = shape; }
    void updateGeometry(const QSizeF& itemSize, float thickness, float radius, QRgb color);

private:
    UCFrameCornerMaterial m_material;
    QSGGeometry m_geometry;
    quint8 m_radius[2];
    quint8 m_newRadius[2];
    quint8 m_shape : 1;
    quint8 m_newShape : 1;
    quint8 m_visible : 1;
    quint8 __padding : 5;
};

class UCFrameNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCFrameNode();
    void updateGeometry(const QSizeF& itemSize, float thickness, float radius, QRgb color);

private:
    UCOpaqueColorMaterial m_opaqueMaterial;
    UCColorMaterial m_material;
    QSGGeometry m_geometry;
};

QML_DECLARE_TYPE(UCFrame)

#endif  // UCFRAME_H
