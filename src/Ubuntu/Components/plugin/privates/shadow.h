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

#ifndef DROPSHADOW_H
#define DROPSHADOW_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

class UCShadow : public QQuickItem
{
    Q_OBJECT

    Q_ENUMS(Style)
    Q_ENUMS(Shape)
    Q_PROPERTY(Style style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCShadow(QQuickItem* parent = 0);

    enum Style { Drop = 0, Inner = 1 };
    enum Shape { Squircle = 0, Circle = 1 };

    Style style() const { return static_cast<Style>(m_style); }
    void setStyle(Style style);
    Shape shape() const { return static_cast<Shape>(m_shape); }
    void setShape(Shape shape);
    qreal size() const { return m_size; }
    void setSize(qreal size);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QColor color() const {
      return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void sizeChanged();
    void radiusChanged();
    void colorChanged();
    void styleChanged();
    void shapeChanged();

private:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    enum { DirtyStyle = (1 << 0), DirtyShape = (1 << 1) };

    QRgb m_color;
    quint8 m_size;
    quint8 m_radius;
    quint8 m_style : 1;
    quint8 m_shape : 1;
    quint8 __padding : 6;
    quint8 m_flags;

    Q_DISABLE_COPY(UCShadow)
};

class UCShadowMaterial : public QSGMaterial
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

    static Q_CONSTEXPR quint32 makeTextureHashKey(
        UCShadow::Shape shape, quint8 shadow, quint8 radius) {
        return static_cast<quint32>(shape) << 16  // 1 bit
            | static_cast<quint32>(shadow) << 8   // 8 bit
            | static_cast<quint32>(radius);       // 8 bit
    }

    UCShadowMaterial(UCShadow::Style style);
    ~UCShadowMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;

    quint32 textureId() const { return m_textureId; }
    void setStyle(UCShadow::Style style) { m_style = style; }
    UCShadow::Style style() const { return static_cast<UCShadow::Style>(m_style); }
    void updateTexture(
        UCShadow::Shape shape, int shadow, int radius, UCShadow::Shape newShape, int newShadow,
        int newRadius);

private:
    TextureHash* m_textureHash;
    quint32 m_textureId;
    quint32 m_key;
    quint8 m_style : 1;
    quint8 __padding : 7;
};

class UCShadowNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, s, t; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCShadowNode(UCShadow::Style style, UCShadow::Shape shape);
    void preprocess();
    void setStyle(UCShadow::Style style);
    void setShape(UCShadow::Shape shape) { m_newShape = shape; }
    void updateGeometry(const QSizeF& itemSize, float shadow, float radius, QRgb color);

private:
    UCShadowMaterial m_material;
    QSGGeometry m_geometry;
    quint8 m_shadow;
    quint8 m_newShadow;
    quint8 m_radius;
    quint8 m_newRadius;
    quint8 m_style : 1;
    quint8 m_shape : 1;
    quint8 m_newShape : 1;
    quint8 __padding : 5;
};

QML_DECLARE_TYPE(UCShadow)

#endif  // DROPSHADOW_H
