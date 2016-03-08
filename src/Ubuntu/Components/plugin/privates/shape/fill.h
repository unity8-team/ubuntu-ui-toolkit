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

#ifndef UCCOLOR_H
#define UCCOLOR_H

#include "utils.h"
#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>

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

    enum Shape { Squircle = 0, Circle = 1 };

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
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    enum { DirtyShape = (1 << 0), DirtyCornerVisibility = (1 << 1) };

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
    class Texture {
    public:
        Texture(quint32 id = 0) : m_id(id), m_refCount(1) {}
        quint32 id() const { return m_id; }
        quint32 ref() { Q_ASSERT(m_refCount < UINT_MAX); m_refCount++; return m_id; }
        quint32 unref() { Q_ASSERT(m_refCount > 0); return --m_refCount; }

    private:
        quint32 m_id;
        quint32 m_refCount;
    };

    typedef QHash<quint32, Texture> TextureHash;

    static Q_CONSTEXPR quint32 makeTextureHashKey(UCFill::Shape shape, quint8 radius) {
        return static_cast<quint32>(shape) << 8  // 1 bit
            | static_cast<quint32>(radius);      // 8 bit
    }

    UCCornerMaterial();
    ~UCCornerMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;

    quint32 textureId() const { return m_textureId; }
    void updateTexture(UCFill::Shape shape, int radius, UCFill::Shape newShape, int newRadius);

private:
    TextureHash* m_textureHash;
    quint32 m_textureId;
    quint32 m_key;
};

class UCCornerNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, s, t; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCCornerNode(UCFill::Shape shape, bool visible);
    virtual void preprocess();
    virtual bool isSubtreeBlocked() const { return m_visible == 0; }

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

    UCColorNode();
    void updateGeometry(const QSizeF& itemSize, float radius, QRgb color);

private:
    UCOpaqueColorMaterial m_opaqueMaterial;
    UCColorMaterial m_material;
    QSGGeometry m_geometry;
};

QML_DECLARE_TYPE(UCFill)

#endif  // UCCOLOR_H
