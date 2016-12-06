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

#include "ucshapedropshadownodes_p.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFunctions>

#include "ucshaperesources_p.h"

// The geometry is made of 9 vertices indexed with a triangle strip mode.
//     0 --- 1 --- 2
//     |  /  |  /  |
//     3 --- 4 --- 5
//     |  /  |  /  |
//     6 --- 7 --- 8
const quint16 indices[] = {
    0, 3, 1, 4, 2, 5,
    5, 3,  // Degenerate triangle.
    3, 6, 4, 7, 5, 8
};
const int indexCount = ARRAY_SIZE(indices);
const int vertexCount = 9;

UCShapeDropShadowNode::UCShapeDropShadowNode()
    : QSGGeometryNode()
    , m_resources(vertexCount, indexCount)
    , m_radius(0)
    , m_shadow(0)
    , m_shape(UCShapeType::Squircle)
    , m_flags(0)
{
    DLOG("creating UCShapeDropShadowNode");
    setFlag(QSGNode::UsePreprocess, true);
    setMaterial(m_resources.material());
    m_resources.opaqueMaterial()->setFlag(QSGMaterial::Blending);
    setOpaqueMaterial(m_resources.opaqueMaterial());
    memcpy(m_resources.geometry()->indexData(), indices, indexCount * sizeof(quint16));
    setGeometry(m_resources.geometry());
    qsgnode_set_description(this, QLatin1String("shapedropshadow"));
}

UCShapeDropShadowNode::~UCShapeDropShadowNode()
{
    DLOG("detroying UCShapeDropShadowNode");
}

void UCShapeDropShadowNode::preprocess()
{
    if (m_flags & DirtyMask) {
        static_cast<UCShapeColorMaskMaterial<false>*>(
            m_resources.material())->updateShadowTexture(
                static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
        static_cast<UCShapeColorMaskMaterial<true>*>(
            m_resources.opaqueMaterial())->updateShadowTexture(
                static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
    }
    m_flags &= ~DirtyMask;
}

void UCShapeDropShadowNode::setVisible(bool visible)
{
    DLOG("UCShapeDropShadowNode::setVisible %d", visible);
    if (static_cast<bool>(m_flags & Visible) != visible) {
        m_flags = (m_flags & ~Visible) | (visible ? Visible : 0);
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeDropShadowNode::update(
    const QSizeF& itemSize, UCShapeType type, float radius, float shadowSize, float shadowAngle,
    float shadowDistance, QRgb shadowColor)
{
    UCShapeColorMaskResources::Vertex* v =
        reinterpret_cast<UCShapeColorMaskResources::Vertex*>(m_resources.geometry()->vertexData());
    float s, c;
    sincosf((shadowAngle + 180.0f) * -(M_PI / 180.0f), &s, &c);
    const float offsetX = roundf(c * shadowDistance);
    const float offsetY = roundf(s * shadowDistance);
    const float dpr = qGuiApp->devicePixelRatio();
    const float w = floorf(static_cast<float>(itemSize.width()));
    const float h = floorf(static_cast<float>(itemSize.height()));
    // Rounded down since renderShape() doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedRadius = qMin(floorf(radius), maxSize);
    const quint32 packedShadowColor = packColor(shadowColor);
    const float clampedShadow = qMin(floorf(shadowSize), maxSize);
    const float border = 1.0f;
    const float textureSize = (2.0f * clampedShadow + 2.0f * border + clampedRadius) * dpr;
    const float textureSizeRounded = roundUp(static_cast<int>(textureSize), textureRounding);
    const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
    const float textureFactor = ((1.0f - textureOffset) * dpr) / textureSize;
    const float midW = w * 0.5f;
    const float midH = h * 0.5f;
    const float midShadowS = (border + clampedShadow + midW) * textureFactor + textureOffset;
    const float midShadowT = (border + clampedShadow + midH) * textureFactor + textureOffset;

    v[0].x = -clampedShadow + offsetX;
    v[0].y = -clampedShadow + offsetY;
    v[0].maskS = border * textureFactor + textureOffset;
    v[0].maskT = border * textureFactor + textureOffset;
    v[0].color = packedShadowColor;
    v[1].x = midW + offsetX;
    v[1].y = -clampedShadow + offsetY;
    v[1].maskS = midShadowS;
    v[1].maskT = border * textureFactor + textureOffset;
    v[1].color = packedShadowColor;
    v[2].x = w + clampedShadow + offsetX;
    v[2].y = -clampedShadow + offsetY;
    v[2].maskS = border * textureFactor + textureOffset;
    v[2].maskT = border * textureFactor + textureOffset;
    v[2].color = packedShadowColor;
    v[3].x = -clampedShadow + offsetX;
    v[3].y = midH + offsetY;
    v[3].maskS = border * textureFactor + textureOffset;
    v[3].maskT = midShadowT;
    v[3].color = packedShadowColor;
    v[4].x = midW + offsetX;
    v[4].y = midH + offsetY;
    v[4].maskS = midShadowS;
    v[4].maskT = midShadowT;
    v[4].color = packedShadowColor;
    v[5].x = w + clampedShadow + offsetX;
    v[5].y = midH + offsetY;
    v[5].maskS = border * textureFactor + textureOffset;
    v[5].maskT = midShadowT;
    v[5].color = packedShadowColor;
    v[6].x = -clampedShadow + offsetX;
    v[6].y = h + clampedShadow + offsetY;
    v[6].maskS = border * textureFactor + textureOffset;
    v[6].maskT = border * textureFactor + textureOffset;
    v[6].color = packedShadowColor;
    v[7].x = midW + offsetX;
    v[7].y = h + clampedShadow + offsetY;
    v[7].maskS = midShadowS;
    v[7].maskT = border * textureFactor + textureOffset;
    v[7].color = packedShadowColor;
    v[8].x = w + clampedShadow + offsetX;
    v[8].y = h + clampedShadow + offsetY;
    v[8].maskS = border * textureFactor + textureOffset;
    v[8].maskT = border * textureFactor + textureOffset;
    v[8].color = packedShadowColor;
    markDirty(QSGNode::DirtyGeometry);

    // Update data for the preprocess() call.
    const quint16 deviceShadow = static_cast<quint16>(clampedShadow * dpr);
    if (m_shadow != deviceShadow) {
        m_shadow = deviceShadow;
        m_flags |= DirtyShadow;
    }
    const quint16 deviceRadius = static_cast<quint16>(clampedRadius * dpr);
    if (m_radius != deviceRadius) {
        m_radius = deviceRadius;
        m_flags |= DirtyRadius;
    }
    if (m_shape != static_cast<quint8>(type)) {
        m_shape = static_cast<quint8>(type);
        m_flags |= DirtyShape;
    }
}
