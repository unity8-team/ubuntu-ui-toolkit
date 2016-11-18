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

// FIXME(loicm) Add a description of the techniques and terms used in the shape
//     item and nodes.

// TODO(loicm): Try using half-sized textures to speed up CPU-based shadow
//     rendering, switching to bilinear texture sampling at runtime. Evaluate
//     the trade-off between texture creation (faster generation allows higher
//     max radius and shadow sizes) and rendering speed.

// TODO(loicm): Outer shadow rendering should provide a clippedShape (or
//     knockedOutShape) mode that knocks out the pixels of the shape area. This
//     is often what we need and that would allow to optimise rendering by
//     removing transparent pixels (that are still rasterised, shaded and
//     blended by the GPU...) with a dedicated mesh.

// TODO(loicm): Add support for negative distances.

#include "ucshape_p.h"

#include <math.h>

#include "ucshapedropshadownodes_p.h"
#include "ucshapefillnodes_p.h"

Q_CONSTEXPR qreal maxRadius = 128.0;
// Gaussian kernels are precomputed at a specific size with an off-line tool.
Q_STATIC_ASSERT(static_cast<int>(maxRadius) <= gaussianCount);

UCShape::UCShape(QQuickItem* parent)
    : QQuickItem(parent)
    , m_fillColor(qRgba(0xe9, 0x54, 0x20, 0xff))  // Ubuntu orange.
    , m_dropShadowColor(qRgba(0, 0, 0, 0xff))
    , m_innerShadowColor(qRgba(0, 0, 0, 0xff))
    , m_borderColor(qRgba(0xff, 0xff, 0xff, 0xff))
    , m_radius(0)
    , m_dropShadowSize(0)
    , m_dropShadowDistance(0)
    , m_dropShadowAngle(0)
    , m_innerShadowSize(0)
    , m_innerShadowDistance(0)
    , m_innerShadowAngle(0)
    , m_borderSize(0)
    // the flags must be in sync with the default values above!
    , m_flags(FillCenterVisible)
    , m_shape(Squircle)
{
    setFlag(ItemHasContents);
}

UCShape::Shape UCShape::shape() const
{
    return static_cast<Shape>(m_shape);
}

void UCShape::setShape(Shape shape)
{
    const quint8 newShape = shape;
    if (m_shape != newShape) {
        m_shape = newShape;
        update();
        Q_EMIT shapeChanged();
    }
}

qreal UCShape::radius() const
{
    return unquantizeFromU16(m_radius);
}

void UCShape::setRadius(qreal radius)
{
    const quint16 quantizedRadius = quantizeToU16(radius);
    if (m_radius != quantizedRadius) {
        if ((m_radius > 0) != (quantizedRadius > 0)) {
            if (qAlpha(m_fillColor) > 0) {
                if (quantizedRadius > 0) {
                    m_flags |= FillCornersVisible | DirtyFillCornersVisibility;
                } else {
                    m_flags = (m_flags & ~FillCornersVisible) | DirtyFillCornersVisibility;
                }
            }
        }
        m_radius = quantizedRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

QColor UCShape::fillColor() const
{
    return QColor(qRed(m_fillColor), qGreen(m_fillColor), qBlue(m_fillColor), qAlpha(m_fillColor));
}

void UCShape::setFillColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_fillColor != rgbColor) {
        if ((qAlpha(m_fillColor) > 0) != (qAlpha(rgbColor) > 0)) {
            if (qAlpha(rgbColor) > 0) {
                m_flags |= FillCenterVisible | DirtyFillCenterVisibility;
                if (m_radius > 0) {
                    m_flags |= FillCornersVisible | DirtyFillCornersVisibility;
                }
            } else {
                m_flags &= ~FillCenterVisible;
                m_flags |= DirtyFillCenterVisibility;
                if (m_radius > 0) {
                    m_flags &= ~FillCornersVisible;
                    m_flags |= DirtyFillCornersVisibility;
                }
            }
        }
        m_fillColor = rgbColor;
        update();
        Q_EMIT fillColorChanged();
    }
}

qreal UCShape::dropShadowSize() const
{
    return unquantizeFromU16(m_dropShadowSize);
}

void UCShape::setDropShadowSize(qreal size)
{
    const quint16 quantizedSize = quantizeToU16(size);
    if (m_dropShadowSize != quantizedSize) {
        if (((m_dropShadowSize > 0) != (quantizedSize > 0))
            && ((m_dropShadowDistance == 0) && (qAlpha(m_dropShadowColor) > 0))) {
            if (quantizedSize > 0) {
                m_flags |= DropShadowVisible;
            } else {
                m_flags &= ~DropShadowVisible;
            }
            m_flags |= DirtyDropShadowVisibility;
        }
        m_dropShadowSize = quantizedSize;
        update();
        Q_EMIT dropShadowSizeChanged();
    }
}

qreal UCShape::dropShadowDistance() const
{
    return unquantizeFromU16(m_dropShadowDistance);
}

void UCShape::setDropShadowDistance(qreal distance)
{
    const quint16 quantizedDistance = quantizeToU16(distance);
    if (m_dropShadowDistance != quantizedDistance) {
        if ((m_dropShadowDistance > 0) != (quantizedDistance > 0)
            && ((m_dropShadowSize == 0) && (qAlpha(m_dropShadowColor) > 0))) {
            if (quantizedDistance > 0) {
                m_flags |= DropShadowVisible;
            } else {
                m_flags &= ~DropShadowVisible;
            }
            m_flags |= DirtyDropShadowVisibility;
        }
        m_dropShadowDistance = quantizedDistance;
        update();
        Q_EMIT dropShadowDistanceChanged();
    }
}

qreal UCShape::dropShadowAngle() const
{
    return unquantizeFromU16(m_dropShadowAngle);
}

void UCShape::setDropShadowAngle(qreal angle)
{
    double clampedAngle = fmod(static_cast<double>(angle), 360.0);
    if (clampedAngle < 0.0) {
        clampedAngle += 360.0;
    }
    const quint16 quantizedAngle = quantizeToU16Clamped(clampedAngle);
    if (m_dropShadowAngle != quantizedAngle) {
        m_dropShadowAngle = quantizedAngle;
        update();
        Q_EMIT dropShadowAngleChanged();
    }
}

QColor UCShape::dropShadowColor() const
{
    return QColor(
        qRed(m_dropShadowColor), qGreen(m_dropShadowColor), qBlue(m_dropShadowColor),
        qAlpha(m_dropShadowColor));
}

void UCShape::setDropShadowColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_dropShadowColor != rgbColor) {
        if (((qAlpha(m_dropShadowColor) > 0) != (qAlpha(rgbColor) > 0))
            && ((m_dropShadowDistance > 0) || (m_dropShadowSize > 0))) {
            if (qAlpha(rgbColor) > 0) {
                m_flags |= DropShadowVisible;
            } else {
                m_flags &= ~DropShadowVisible;
            }
            m_flags |= DirtyDropShadowVisibility;
        }
        m_dropShadowColor = rgbColor;
        update();
        Q_EMIT dropShadowColorChanged();
    }
}

qreal UCShape::innerShadowSize() const
{
    return unquantizeFromU16(m_innerShadowSize);
}

void UCShape::setInnerShadowSize(qreal size)
{
    const quint16 quantizedSize = quantizeToU16(size);
    if (m_innerShadowSize != quantizedSize) {
        m_innerShadowSize = quantizedSize;
        update();
        Q_EMIT innerShadowSizeChanged();
    }
}

qreal UCShape::innerShadowDistance() const
{
    return unquantizeFromU16(m_innerShadowDistance);
}

void UCShape::setInnerShadowDistance(qreal distance)
{
    const quint16 quantizedDistance = quantizeToU16(distance);
    if (m_innerShadowDistance != quantizedDistance) {
        m_innerShadowDistance = quantizedDistance;
        update();
        Q_EMIT innerShadowDistanceChanged();
    }
}

qreal UCShape::innerShadowAngle() const
{
    return unquantizeFromU16(m_innerShadowAngle);
}

void UCShape::setInnerShadowAngle(qreal angle)
{
    double clampedAngle = fmod(static_cast<double>(angle), 360.0);
    if (clampedAngle < 0.0) {
        clampedAngle += 360.0;
    }
    const quint16 quantizedAngle = quantizeToU16Clamped(clampedAngle);
    if (m_innerShadowAngle != quantizedAngle) {
        m_innerShadowAngle = quantizedAngle;
        update();
        Q_EMIT innerShadowAngleChanged();
    }
}

QColor UCShape::innerShadowColor() const
{
    return QColor(
        qRed(m_innerShadowColor), qGreen(m_innerShadowColor), qBlue(m_innerShadowColor),
        qAlpha(m_innerShadowColor));
}

void UCShape::setInnerShadowColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_innerShadowColor != rgbColor) {
        m_innerShadowColor = rgbColor;
        update();
        Q_EMIT innerShadowColorChanged();
    }
}

qreal UCShape::borderSize() const
{
    return unquantizeFromU16(m_borderSize);
}

void UCShape::setBorderSize(qreal size)
{
    const quint16 quantizedSize = quantizeToU16(size);
    if (m_borderSize != quantizedSize) {
        m_borderSize = quantizedSize;
        update();
        Q_EMIT borderSizeChanged();
    }
}

QColor UCShape::borderColor() const
{
    return QColor(
        qRed(m_borderColor), qGreen(m_borderColor), qBlue(m_borderColor), qAlpha(m_borderColor));
}

void UCShape::setBorderColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_borderColor != rgbColor) {
        m_borderColor = rgbColor;
        update();
        Q_EMIT borderColorChanged();
    }
}

class UCShapeNode : public QSGNode
{
public:
    UCShapeNode() : QSGNode(), m_nodes{} { DLOG("creating UCShapeNode"); }
    ~UCShapeNode() { DLOG("detroying UCShapeNode"); }

    // Sorted by rendering order from back to front.
    enum NodeType { DropShadow = 0, FillCenter, FillCorners, NodeTypeCount };

    QSGNode* node(NodeType type, bool instantiate = true)
    {
        DASSERT(static_cast<quint32>(type) < NodeTypeCount);

        if (m_nodes[type]) {
            return m_nodes[type];
        } else if (!instantiate) {
            return Q_NULLPTR;
        } else {
            switch (type) {
                case DropShadow: m_nodes[DropShadow] = new UCShapeDropShadowNode; break;
                case FillCenter: m_nodes[FillCenter] = new UCShapeFillCenterNode; break;
                case FillCorners: m_nodes[FillCorners] = new UCShapeFillCornersNode; break;
                default: DNOT_REACHED(); return Q_NULLPTR;
            }
            for (int i = type + 1; i < NodeTypeCount; ++i) {
                if (m_nodes[i]) {
                    insertChildNodeBefore(m_nodes[type], m_nodes[i]);
                    return m_nodes[type];
                }
            }
            appendChildNode(m_nodes[type]);
            return m_nodes[type];
        }
    }

private:
    QSGNode* m_nodes[NodeTypeCount];
};

QSGNode* UCShape::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty()) {
        delete oldNode;
        return Q_NULLPTR;
    }

    UCShapeNode* shapeNode;
    if (oldNode) {
        shapeNode = static_cast<UCShapeNode*>(oldNode);
    } else {
        shapeNode = new UCShapeNode;
        // Make sure the right visibility is set on the nodes.
        m_flags |= DirtyFlags;
    }

    // The strategy here is to instantiate shape node's children only when
    // required (for instance corner nodes are instantiated only when radius is
    // greater than 0). Once instantiated a child node is freed only when the
    // parent is freed (for instance corner nodes are not freed when radius goes
    // back to 0). That allows to minimise the memory usage to what the user
    // needs (properties like radius and shadow sizes are set to 0 by default on
    // purpose), to avoid complexifying the code and to avoid costly memory
    // allocations when animating for instance a color back and forth to
    // transparent (which would require a node deletion).

    // Drop shadow node.
    const bool dropShadowVisible = static_cast<bool>(m_flags & DropShadowVisible);
    if (dropShadowVisible) {
        static_cast<UCShapeDropShadowNode*>(shapeNode->node(UCShapeNode::DropShadow))->update(
            itemSize, static_cast<UCShapeType>(m_shape), unquantizeFromU16(m_radius),
            unquantizeFromU16(m_dropShadowSize), unquantizeFromU16(m_dropShadowAngle),
            unquantizeFromU16(m_dropShadowDistance), m_dropShadowColor);
    }
    if (m_flags & DirtyDropShadowVisibility) {
        static_cast<UCShapeDropShadowNode*>(shapeNode->node(UCShapeNode::DropShadow))->setVisible(
            dropShadowVisible);
    }

    // Fill center node.
    const bool fillCenterVisible = static_cast<bool>(m_flags & FillCenterVisible);
    if (fillCenterVisible) {
        static_cast<UCShapeFillCenterNode*>(shapeNode->node(UCShapeNode::FillCenter))->update(
            itemSize, static_cast<UCShapeType>(m_shape), unquantizeFromU16(m_radius), m_fillColor,
            unquantizeFromU16(m_innerShadowSize), unquantizeFromU16(m_innerShadowAngle),
            unquantizeFromU16(m_innerShadowDistance), m_innerShadowColor,
            unquantizeFromU16(m_borderSize), m_borderColor);
    }
    if (m_flags & DirtyFillCenterVisibility) {
        static_cast<UCShapeFillCenterNode*>(shapeNode->node(UCShapeNode::FillCenter))->setVisible(
            fillCenterVisible);
    }

    // Fill corners node.
    const bool fillCornersVisible = static_cast<bool>(m_flags & FillCornersVisible);
    if (fillCornersVisible) {
        static_cast<UCShapeFillCornersNode*>(shapeNode->node(UCShapeNode::FillCorners))->update(
            itemSize, static_cast<UCShapeType>(m_shape), unquantizeFromU16(m_radius), m_fillColor,
            unquantizeFromU16(m_innerShadowSize), unquantizeFromU16(m_innerShadowAngle),
            unquantizeFromU16(m_innerShadowDistance), m_innerShadowColor,
            unquantizeFromU16(m_borderSize), m_borderColor);
    }
    if (m_flags & DirtyFillCornersVisibility) {
        UCShapeFillCornersNode* node = static_cast<UCShapeFillCornersNode*>(
            shapeNode->node(UCShapeNode::FillCorners, false));
        if (node) {
            node->setVisible(fillCornersVisible);
        }
    }

    m_flags &= ~DirtyFlags;
    return shapeNode;
}
