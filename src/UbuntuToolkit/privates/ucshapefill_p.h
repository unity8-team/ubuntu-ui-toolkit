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

#ifndef UCSHAPEFILL_P_H
#define UCSHAPEFILL_P_H

#include <QtQuick/QSGNode>
#include <QtQuick/QQuickItem>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>
#include <UbuntuToolkit/private/ucshapeutils_p.h>

// Renders a colored fill shape.
class UCFill : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Shape)

    // Shape to use at corners.
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)

    // Radius of the shape in pixels.
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

    // Fill color. Translucent colors are supported.
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCFill(QQuickItem* parent = 0);

    enum Shape { Squircle = Texture::Squircle, Circle = Texture::Circle };

    Shape shape() const { return static_cast<Shape>(m_shape); }
    void setShape(Shape shape);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QColor color() const {
      return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void shapeChanged();
    void radiusChanged();
    void colorChanged();

private:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;

    enum { DirtyBlending = (1 << 0), DirtyShape = (1 << 1), DirtyCornerVisibility = (1 << 2) };

    QRgb m_color;
    quint8 m_radius;
    quint8 m_shape : 1;
    quint8 __padding : 7;
    quint8 m_flags;

    Q_DISABLE_COPY(UCFill)
};

class UCCornerMaterial : public QSGMaterial
{
public:
    UCCornerMaterial();
    QSGMaterialType* type() const override;
    QSGMaterialShader* createShader() const override;
    int compare(const QSGMaterial* other) const override;

    quint32 textureId() const { return m_textureId; }
    void updateTexture(UCFill::Shape shape, int radius);

private:
    TextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCCornerNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, s, t; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCCornerNode(UCFill::Shape shape, bool visible);
    void preprocess() override;
    bool isSubtreeBlocked() const override { return m_visible == 0; }

    void setVisible(bool visible);
    void setShape(UCFill::Shape shape) { m_newShape = shape; }
    void updateGeometry(const QSizeF& itemSize, float radius, QRgb color);

private:
    UCCornerMaterial m_material;
    QSGGeometry m_geometry;
    quint8 m_radius;
    quint8 m_newRadius;
    quint8 m_shape : 1;
    quint8 m_newShape : 1;
    quint8 m_visible : 1;
    quint8 __padding : 5;
};

class UCColorNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCColorNode(bool blending);
    void updateBlending(bool blending);
    void updateGeometry(const QSizeF& itemSize, float radius, QRgb color);

private:
    UCOpaqueColorMaterial m_opaqueMaterial;
    UCColorMaterial m_material;
    QSGGeometry m_geometry;
};

QML_DECLARE_TYPE(UCFill)

#endif  // UCSHAPEFILL_P_H
