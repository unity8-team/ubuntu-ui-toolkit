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

#ifndef UCSHAPE_P_H
#define UCSHAPE_P_H

#include <QtQuick/QQuickItem>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>

class UCShape : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Shape)

    // Shape of the corner.
    Q_PROPERTY(Shape shape READ shape WRITE setShape NOTIFY shapeChanged)

    // Radius of the corners in pixels.
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

    // Fill color of the shape.
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)

    // Size of the drop shadow in pixels.
    Q_PROPERTY(qreal dropShadowSize READ dropShadowSize WRITE setDropShadowSize
               NOTIFY dropShadowSizeChanged)

    // Offset of the drop shadow in pixels.
    Q_PROPERTY(qreal dropShadowDistance READ dropShadowDistance WRITE setDropShadowDistance
               NOTIFY dropShadowDistanceChanged)

    // Offset angle of the drop shadow in degrees. The virtual light points to
    // the left by default and is rotated counter clockwise.
    Q_PROPERTY(qreal dropShadowAngle READ dropShadowAngle WRITE setDropShadowAngle
               NOTIFY dropShadowAngleChanged)

    // Color of the drop shadow.
    Q_PROPERTY(QColor dropShadowColor READ dropShadowColor WRITE setDropShadowColor
               NOTIFY dropShadowColorChanged)

    // Size of the inner shadow in pixels.
    Q_PROPERTY(qreal innerShadowSize READ innerShadowSize WRITE setInnerShadowSize
               NOTIFY innerShadowSizeChanged)

    // Offset of the inner shadow in pixels.
    Q_PROPERTY(qreal innerShadowDistance READ innerShadowDistance WRITE setInnerShadowDistance
               NOTIFY innerShadowDistanceChanged)

    // Offset angle of the inner shadow in degrees. The virtual light points to
    // the left by default and is rotated counter clockwise.
    Q_PROPERTY(qreal innerShadowAngle READ innerShadowAngle WRITE setInnerShadowAngle
               NOTIFY innerShadowAngleChanged)

    // Color of the inner shadow.
    Q_PROPERTY(QColor innerShadowColor READ innerShadowColor WRITE setInnerShadowColor
               NOTIFY innerShadowColorChanged)

    // Size of the border in pixels.
    Q_PROPERTY(qreal borderSize READ borderSize WRITE setBorderSize NOTIFY borderSizeChanged)

    // Color of the border.
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)

public:
    UCShape(QQuickItem* parent = 0);

    enum Shape { Squircle = UCShapeType::Squircle, Circle = UCShapeType::Circle };

    Shape shape() const;
    void setShape(Shape shape);
    qreal radius() const;
    void setRadius(qreal radius);
    QColor fillColor() const;
    void setFillColor(const QColor& fillColor);
    qreal dropShadowSize() const;
    void setDropShadowSize(qreal size);
    qreal dropShadowDistance() const;
    void setDropShadowDistance(qreal distance);
    qreal dropShadowAngle() const;
    void setDropShadowAngle(qreal angle);
    QColor dropShadowColor() const;
    void setDropShadowColor(const QColor& color);
    qreal innerShadowSize() const;
    void setInnerShadowSize(qreal size);
    qreal innerShadowDistance() const;
    void setInnerShadowDistance(qreal distance);
    qreal innerShadowAngle() const;
    void setInnerShadowAngle(qreal angle);
    QColor innerShadowColor() const;
    void setInnerShadowColor(const QColor& color);
    qreal borderSize() const;
    void setBorderSize(qreal size);
    QColor borderColor() const;
    void setBorderColor(const QColor& color);

Q_SIGNALS:
    void shapeChanged();
    void radiusChanged();
    void fillColorChanged();
    void dropShadowSizeChanged();
    void dropShadowDistanceChanged();
    void dropShadowAngleChanged();
    void dropShadowColorChanged();
    void innerShadowSizeChanged();
    void innerShadowDistanceChanged();
    void innerShadowAngleChanged();
    void innerShadowColorChanged();
    void borderSizeChanged();
    void borderColorChanged();

private:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) Q_DECL_OVERRIDE;

    enum {
        DropShadowVisible          = (1 << 0),
        FillCenterVisible          = (1 << 1),
        FillCornersVisible         = (1 << 2),
        DirtyDropShadowVisibility  = (1 << 3),
        DirtyFillCenterVisibility  = (1 << 4),
        DirtyFillCornersVisibility = (1 << 5),
        DirtyFlags =
            DirtyDropShadowVisibility | DirtyFillCenterVisibility | DirtyFillCornersVisibility
    };

    QRgb m_fillColor;
    QRgb m_dropShadowColor;
    QRgb m_innerShadowColor;
    QRgb m_borderColor;
    quint16 m_radius;
    quint16 m_dropShadowSize;
    quint16 m_dropShadowDistance;
    quint16 m_dropShadowAngle;
    quint16 m_innerShadowSize;
    quint16 m_innerShadowDistance;
    quint16 m_innerShadowAngle;
    quint16 m_borderSize;
    quint16 m_flags;
    quint8 m_shape : 1;
    quint8 __padding : 7;

    Q_DISABLE_COPY(UCShape)
};

QML_DECLARE_TYPE(UCShape)

#endif  // UCSHAPE_P_H
