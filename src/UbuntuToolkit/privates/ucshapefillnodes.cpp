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

// FIXME(loicm) The midShadow coords force a dependent texture read which is
//     quite slow on some platforms. We should maybe add geometry to deal with
//     the coords inversion. That will add complexity but improve perf.

#include "ucshapefillnodes_p.h"

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QGuiApplication>

UCShapeFillCenterNode::UCShapeFillCenterNode()
    : QSGGeometryNode()
    , m_resources(Q_NULLPTR)
    , m_radius(0)
    , m_shadow(0)
    , m_borderRadius(0)
    , m_flags(0)
    , m_shape(UCShapeType::Squircle)
{
    DLOG("creating UCShapeFillCenterNode");
    qsgnode_set_description(this, QLatin1String("shapefillcenter"));
}

UCShapeFillCenterNode::~UCShapeFillCenterNode()
{
    DLOG("detroying UCShapeFillCenterNode");
    delete m_resources;
}

void UCShapeFillCenterNode::preprocess()
{
    DASSERT(m_flags & Textured);

    // FIXME(loicm) Could be made more consise (texture update methods in the resources?).

    switch (m_flags & StyleMask) {
    case (HasColor | HasShadow):
        if (m_flags & (DirtyRadius | DirtyShadow | DirtyShape)) {
            static_cast<UCShapeFillCenterShadowMaterial<false>*>(
                m_resources->material())->updateShadowTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
            static_cast<UCShapeFillCenterShadowMaterial<true>*>(
                m_resources->opaqueMaterial())->updateShadowTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
        }
        break;

    case (HasColor | HasBorder):
        if (m_flags & (DirtyRadius | DirtyBorderRadius | DirtyShape)) {
            static_cast<UCShapeFillCenterBorderMaterial<false>*>(
                m_resources->material())->updateBorderTexture(
                    static_cast<UCShapeType>(m_shape), m_borderRadius);
            static_cast<UCShapeFillCenterBorderMaterial<true>*>(
                m_resources->opaqueMaterial())->updateBorderTexture(
                    static_cast<UCShapeType>(m_shape), m_borderRadius);
        }
        break;

    case (HasColor | HasShadow | HasBorder):
        if (m_flags & (DirtyRadius | DirtyShadow | DirtyShape)) {
            static_cast<UCShapeFillCenterShadowBorderMaterial<false>*>(
                m_resources->material())->updateShadowTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
            static_cast<UCShapeFillCenterShadowBorderMaterial<true>*>(
                m_resources->opaqueMaterial())->updateShadowTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
        }
        if (m_flags & (DirtyRadius | DirtyBorderRadius | DirtyShape)) {
            static_cast<UCShapeFillCenterShadowBorderMaterial<false>*>(
                m_resources->material())->updateBorderTexture(
                    static_cast<UCShapeType>(m_shape), m_borderRadius);
            static_cast<UCShapeFillCenterShadowBorderMaterial<true>*>(
                m_resources->opaqueMaterial())->updateBorderTexture(
                    static_cast<UCShapeType>(m_shape), m_borderRadius);
        }
        break;

    default:
        NOT_REACHED();
    }

    m_flags &= ~DirtyMask;
}

void UCShapeFillCenterNode::setVisible(bool visible)
{
    DLOG("UCShapeFillCenterNode::setVisible %d", visible);
    if (static_cast<bool>(m_flags & Visible) != visible) {
        m_flags = (m_flags & ~Visible) | (visible ? Visible : 0);
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeFillCenterNode::update(
    const QSizeF& itemSize, UCShapeType type, float radius, QRgb color, float shadowSize,
    float shadowAngle, float shadowDistance, QRgb shadowColor, float borderSize, QRgb borderColor)
{
    // Without border, the geometry is made of 8 vertices indexed with a
    // triangle strip mode. With border, the geometry is made of 13 vertices
    // indexed with a triangle strip mode.
    //
    //      Without border:        With border:
    //         0 ----- 1            0 - 1 - 2
    //       /           \        /     |     \   ("-Werror=comment" GCC guard)
    //     2               3    3       |       4
    //     |               |    |       |       |
    //     |               |    5 ----- 6 ----- 7
    //     |               |    |       |       |
    //     4               5    8       |       9
    //       \           /        \     |     /
    //         6 ----- 7            10 -11-12
    const quint16 noBorderIndices[] = {
        2, 4, 0, 6, 1, 7, 3, 5
    };
    const quint16 borderIndices[] = {
        3, 5, 0, 6, 1, 7, 2, 4,
        4, 8,  // Degenerate triangle.
        8, 10, 5, 11, 6, 12, 7, 9
    };
    const int noBorderIndexCount = ARRAY_SIZE(noBorderIndices);
    const int borderIndexCount = ARRAY_SIZE(borderIndices);
    const int noBorderVertexCount = 8;
    const int borderVertexCount = 13;

    quint16 style = ((shadowSize <= 0.0f) || (qAlpha(shadowColor) == 0))
        ? HasColor : (HasColor | HasShadow);
    if (borderSize >= 1.0f) {
        style |= HasBorder;
    }

    // Create new material/geometry set if needed.
    if (style != (m_flags & StyleMask)) {
        delete m_resources;
        switch (style) {
        case HasColor:
            m_resources = new UCShapeColorResources(noBorderVertexCount, noBorderIndexCount);
            memcpy(m_resources->geometry()->indexData(), noBorderIndices,
                   noBorderIndexCount * sizeof(quint16));
            setFlag(QSGNode::UsePreprocess, false);
            break;
        case (HasColor | HasShadow):
            m_resources = new UCShapeFillCenterShadowResources(
                noBorderVertexCount, noBorderIndexCount);
            memcpy(m_resources->geometry()->indexData(), noBorderIndices,
                   noBorderIndexCount * sizeof(quint16));
            setFlag(QSGNode::UsePreprocess, true);
            break;
        case (HasColor | HasBorder):
            m_resources = new UCShapeFillCenterBorderResources(borderVertexCount, borderIndexCount);
            memcpy(m_resources->geometry()->indexData(), borderIndices,
                   borderIndexCount * sizeof(quint16));
            setFlag(QSGNode::UsePreprocess, true);
            break;
        case (HasColor | HasShadow | HasBorder):
            m_resources = new UCShapeFillCenterShadowBorderResources(
                borderVertexCount, borderIndexCount);
            memcpy(m_resources->geometry()->indexData(), borderIndices,
                   borderIndexCount * sizeof(quint16));
            setFlag(QSGNode::UsePreprocess, true);
            break;
        default:
            NOT_REACHED();
        }
        setMaterial(m_resources->material());
        setOpaqueMaterial(m_resources->opaqueMaterial());
        setGeometry(m_resources->geometry());
        m_flags = (m_flags & ~StyleMask) | style | DirtyMask;
    }

    const float dpr = qGuiApp->devicePixelRatio();
    const float w = floorf(static_cast<float>(itemSize.width()));
    const float h = floorf(static_cast<float>(itemSize.height()));
    // Rounded down since Shadow doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedRadius = qMin(floorf(radius), maxSize);
    const quint32 packedColor = packColor(color);

    // Update geometry depending on the style.
    switch (style) {
    case HasColor: {
        UCShapeColorResources::Vertex* v =
            reinterpret_cast<UCShapeColorResources::Vertex*>(m_resources->geometry()->vertexData());
        v[0].x = clampedRadius;
        v[0].y = 0.0f;
        v[0].color = packedColor;
        v[1].x = w - clampedRadius;
        v[1].y = 0.0f;
        v[1].color = packedColor;
        v[2].x = 0.0f;
        v[2].y = clampedRadius;
        v[2].color = packedColor;
        v[3].x = w;
        v[3].y = clampedRadius;
        v[3].color = packedColor;
        v[4].x = 0.0f;
        v[4].y = h - clampedRadius;
        v[4].color = packedColor;
        v[5].x = w;
        v[5].y = h - clampedRadius;
        v[5].color = packedColor;
        v[6].x = clampedRadius;
        v[6].y = h;
        v[6].color = packedColor;
        v[7].x = w - clampedRadius;
        v[7].y = h;
        v[7].color = packedColor;
        markDirty(QSGNode::DirtyGeometry);
        break;
    }

    case (HasColor | HasShadow): {
        UCShapeFillCenterShadowResources::Vertex* v =
            reinterpret_cast<UCShapeFillCenterShadowResources::Vertex*>(
                m_resources->geometry()->vertexData());
        float s, c;
        sincosf(shadowAngle * -(M_PI / 180.0f), &s, &c);
        const float offsetX = roundf(c * shadowDistance);
        const float offsetY = roundf(s * shadowDistance);
        const float clampedShadow = qMin(floorf(shadowSize), maxSize);
        const float textureSize = (2.0f * clampedShadow + clampedRadius) * dpr;
        const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
        const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
        const float textureFactor = ((1.0f - textureOffset) * dpr) / textureSize;
        const float midShadowS = (clampedShadow + (w * 0.5f)) * textureFactor + textureOffset;
        const float midShadowT = (clampedShadow + (h * 0.5f)) * textureFactor + textureOffset;
        const quint32 packedShadowColor = packColor(shadowColor);
        v[0].x = clampedRadius;
        v[0].y = 0.0f;
        v[0].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[0].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[0].midShadowS = midShadowS;
        v[0].midShadowT = midShadowT;
        v[0].color = packedColor;
        v[0].shadowColor = packedShadowColor;
        v[1].x = w - clampedRadius;
        v[1].y = 0.0f;
        v[1].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[1].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[1].midShadowS = midShadowS;
        v[1].midShadowT = midShadowT;
        v[1].color = packedColor;
        v[1].shadowColor = packedShadowColor;
        v[2].x = 0.0f;
        v[2].y = clampedRadius;
        v[2].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[2].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[2].midShadowS = midShadowS;
        v[2].midShadowT = midShadowT;
        v[2].color = packedColor;
        v[2].shadowColor = packedShadowColor;
        v[3].x = w;
        v[3].y = clampedRadius;
        v[3].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[3].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[3].midShadowS = midShadowS;
        v[3].midShadowT = midShadowT;
        v[3].color = packedColor;
        v[3].shadowColor = packedShadowColor;
        v[4].x = 0.0f;
        v[4].y = h - clampedRadius;
        v[4].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[4].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[4].midShadowS = midShadowS;
        v[4].midShadowT = midShadowT;
        v[4].color = packedColor;
        v[4].shadowColor = packedShadowColor;
        v[5].x = w;
        v[5].y = h - clampedRadius;
        v[5].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[5].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[5].midShadowS = midShadowS;
        v[5].midShadowT = midShadowT;
        v[5].color = packedColor;
        v[5].shadowColor = packedShadowColor;
        v[6].x = clampedRadius;
        v[6].y = h;
        v[6].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[6].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[6].midShadowS = midShadowS;
        v[6].midShadowT = midShadowT;
        v[6].color = packedColor;
        v[6].shadowColor = packedShadowColor;
        v[7].x = w - clampedRadius;
        v[7].y = h;
        v[7].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[7].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[7].midShadowS = midShadowS;
        v[7].midShadowT = midShadowT;
        v[7].color = packedColor;
        v[7].shadowColor = packedShadowColor;
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
        break;
    }

    case (HasColor | HasBorder): {
        UCShapeFillCenterBorderResources::Vertex* v =
            reinterpret_cast<UCShapeFillCenterBorderResources::Vertex*>(
                m_resources->geometry()->vertexData());
        float s, c;
        sincosf(shadowAngle * -(M_PI / 180.0f), &s, &c);
        const float midW = w * 0.5f;
        const float midH = h * 0.5f;
        const float clampedBorder = qMin(floorf(borderSize), maxSize);
        const float clampedBorderRadius =
            floorf(((maxSize - clampedBorder) / maxSize) * clampedRadius);
        const float textureBorder = 1.0f;
        const float borderTextureSize = (2.0f * textureBorder + clampedBorderRadius) * dpr;
        const float borderTextureSizeRounded =
            getStride(static_cast<int>(borderTextureSize), 1, textureStride);
        const float borderTextureOffset =
            (borderTextureSizeRounded - borderTextureSize) / borderTextureSizeRounded;
        const float borderTextureFactor = ((1.0f - borderTextureOffset) * dpr) / borderTextureSize;
        const float borderOffset = -(clampedBorder - textureBorder);
        const quint32 packedBorderColor = packColor(borderColor);
        v[0].x = clampedRadius;
        v[0].y = 0.0f;
        v[0].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[0].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[0].color = packedColor;
        v[0].borderColor = packedBorderColor;
        v[1].x = midW;
        v[1].y = 0.0f;
        v[1].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[1].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[1].color = packedColor;
        v[1].borderColor = packedBorderColor;
        v[2].x = w - clampedRadius;
        v[2].y = 0.0f;
        v[2].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[2].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[2].color = packedColor;
        v[2].borderColor = packedBorderColor;
        v[3].x = 0.0f;
        v[3].y = clampedRadius;
        v[3].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[3].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[3].color = packedColor;
        v[3].borderColor = packedBorderColor;
        v[4].x = w;
        v[4].y = clampedRadius;
        v[4].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[4].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[4].color = packedColor;
        v[4].borderColor = packedBorderColor;
        v[5].x = 0.0f;
        v[5].y = midH;
        v[5].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[5].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[5].color = packedColor;
        v[5].borderColor = packedBorderColor;
        v[6].x = midW;
        v[6].y = midH;
        v[6].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[6].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[6].color = packedColor;
        v[6].borderColor = packedBorderColor;
        v[7].x = w;
        v[7].y = midH;
        v[7].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[7].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[7].color = packedColor;
        v[7].borderColor = packedBorderColor;
        v[8].x = 0.0f;
        v[8].y = h - clampedRadius;
        v[8].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[8].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[8].color = packedColor;
        v[8].borderColor = packedBorderColor;
        v[9].x = w;
        v[9].y = h - clampedRadius;
        v[9].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[9].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[9].color = packedColor;
        v[9].borderColor = packedBorderColor;
        v[10].x = clampedRadius;
        v[10].y = h;
        v[10].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[10].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[10].color = packedColor;
        v[10].borderColor = packedBorderColor;
        v[11].x = midW;
        v[11].y = h;
        v[11].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[11].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[11].color = packedColor;
        v[11].borderColor = packedBorderColor;
        v[12].x = w - clampedRadius;
        v[12].y = h;
        v[12].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[12].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[12].color = packedColor;
        v[12].borderColor = packedBorderColor;
        markDirty(QSGNode::DirtyGeometry);

        // Update data for the preprocess() call.
        const quint16 deviceRadius = static_cast<quint16>(clampedRadius * dpr);
        if (m_radius != deviceRadius) {
            m_radius = deviceRadius;
            m_flags |= DirtyRadius;
        }
        const quint16 deviceBorderRadius = static_cast<quint16>(clampedBorderRadius * dpr);
        if (m_borderRadius != deviceBorderRadius) {
            m_borderRadius = deviceBorderRadius;
            m_flags |= DirtyBorderRadius;
        }
        if (m_shape != static_cast<quint8>(type)) {
            m_shape = static_cast<quint8>(type);
            m_flags |= DirtyShape;
        }
        break;
    }

    case (HasColor | HasShadow | HasBorder): {
        UCShapeFillCenterShadowBorderResources::Vertex* v =
            reinterpret_cast<UCShapeFillCenterShadowBorderResources::Vertex*>(
                m_resources->geometry()->vertexData());
        float s, c;
        sincosf(shadowAngle * -(M_PI / 180.0f), &s, &c);
        const float offsetX = roundf(c * shadowDistance);
        const float offsetY = roundf(s * shadowDistance);
        const float clampedShadow = qMin(floorf(shadowSize), maxSize);
        const float textureSize = (2.0f * clampedShadow + clampedRadius) * dpr;
        const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
        const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
        const float textureFactor = ((1.0f - textureOffset) * dpr) / textureSize;
        const float midW = w * 0.5f;
        const float midH = h * 0.5f;
        const float midShadowS = (clampedShadow + midW) * textureFactor + textureOffset;
        const float midShadowT = (clampedShadow + midH) * textureFactor + textureOffset;
        const float clampedBorder = qMin(floorf(borderSize), maxSize);
        const float clampedBorderRadius =
            floorf(((maxSize - clampedBorder) / maxSize) * clampedRadius);
        const float textureBorder = 1.0f;
        const float borderTextureSize = (2.0f * textureBorder + clampedBorderRadius) * dpr;
        const float borderTextureSizeRounded =
            getStride(static_cast<int>(borderTextureSize), 1, textureStride);
        const float borderTextureOffset =
            (borderTextureSizeRounded - borderTextureSize) / borderTextureSizeRounded;
        const float borderTextureFactor = ((1.0f - borderTextureOffset) * dpr) / borderTextureSize;
        const float borderOffset = -(clampedBorder - textureBorder);
        const quint32 packedShadowColor = packColor(shadowColor);
        const quint32 packedBorderColor = packColor(borderColor);
        v[0].x = clampedRadius;
        v[0].y = 0.0f;
        v[0].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[0].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[0].midShadowS = midShadowS;
        v[0].midShadowT = midShadowT;
        v[0].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[0].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[0].color = packedColor;
        v[0].shadowColor = packedShadowColor;
        v[0].borderColor = packedBorderColor;
        v[1].x = midW;
        v[1].y = 0.0f;
        v[1].shadowS = (offsetX + clampedShadow + midW) * textureFactor + textureOffset;
        v[1].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[1].midShadowS = midShadowS;
        v[1].midShadowT = midShadowT;
        v[1].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[1].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[1].color = packedColor;
        v[1].shadowColor = packedShadowColor;
        v[1].borderColor = packedBorderColor;
        v[2].x = w - clampedRadius;
        v[2].y = 0.0f;
        v[2].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[2].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[2].midShadowS = midShadowS;
        v[2].midShadowT = midShadowT;
        v[2].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[2].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[2].color = packedColor;
        v[2].shadowColor = packedShadowColor;
        v[2].borderColor = packedBorderColor;
        v[3].x = 0.0f;
        v[3].y = clampedRadius;
        v[3].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[3].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[3].midShadowS = midShadowS;
        v[3].midShadowT = midShadowT;
        v[3].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[3].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[3].color = packedColor;
        v[3].shadowColor = packedShadowColor;
        v[3].borderColor = packedBorderColor;
        v[4].x = w;
        v[4].y = clampedRadius;
        v[4].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[4].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[4].midShadowS = midShadowS;
        v[4].midShadowT = midShadowT;
        v[4].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[4].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[4].color = packedColor;
        v[4].shadowColor = packedShadowColor;
        v[4].borderColor = packedBorderColor;
        v[5].x = 0.0f;
        v[5].y = midH;
        v[5].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[5].shadowT = (offsetY + clampedShadow + midH) * textureFactor + textureOffset;
        v[5].midShadowS = midShadowS;
        v[5].midShadowT = midShadowT;
        v[5].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[5].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[5].color = packedColor;
        v[5].shadowColor = packedShadowColor;
        v[5].borderColor = packedBorderColor;
        v[6].x = midW;
        v[6].y = midH;
        v[6].shadowS = (offsetX + clampedShadow + midW) * textureFactor + textureOffset;
        v[6].shadowT = (offsetY + clampedShadow + midH) * textureFactor + textureOffset;
        v[6].midShadowS = midShadowS;
        v[6].midShadowT = midShadowT;
        v[6].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[6].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[6].color = packedColor;
        v[6].shadowColor = packedShadowColor;
        v[6].borderColor = packedBorderColor;
        v[7].x = w;
        v[7].y = midH;
        v[7].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[7].shadowT = (offsetY + clampedShadow + midH) * textureFactor + textureOffset;
        v[7].midShadowS = midShadowS;
        v[7].midShadowT = midShadowT;
        v[7].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[7].borderT = (midH + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[7].color = packedColor;
        v[7].shadowColor = packedShadowColor;
        v[7].borderColor = packedBorderColor;
        v[8].x = 0.0f;
        v[8].y = h - clampedRadius;
        v[8].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[8].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[8].midShadowS = midShadowS;
        v[8].midShadowT = midShadowT;
        v[8].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[8].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[8].color = packedColor;
        v[8].shadowColor = packedShadowColor;
        v[8].borderColor = packedBorderColor;
        v[9].x = w;
        v[9].y = h - clampedRadius;
        v[9].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[9].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[9].midShadowS = midShadowS;
        v[9].midShadowT = midShadowT;
        v[9].borderS = borderOffset * borderTextureFactor + borderTextureOffset;
        v[9].borderT = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[9].color = packedColor;
        v[9].shadowColor = packedShadowColor;
        v[9].borderColor = packedBorderColor;
        v[10].x = clampedRadius;
        v[10].y = h;
        v[10].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[10].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[10].midShadowS = midShadowS;
        v[10].midShadowT = midShadowT;
        v[10].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[10].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[10].color = packedColor;
        v[10].shadowColor = packedShadowColor;
        v[10].borderColor = packedBorderColor;
        v[11].x = midW;
        v[11].y = h;
        v[11].shadowS = (offsetX + clampedShadow + midW) * textureFactor + textureOffset;
        v[11].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[11].midShadowS = midShadowS;
        v[11].midShadowT = midShadowT;
        v[11].borderS = (midW + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[11].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[11].color = packedColor;
        v[11].shadowColor = packedShadowColor;
        v[11].borderColor = packedBorderColor;
        v[12].x = w - clampedRadius;
        v[12].y = h;
        v[12].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[12].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[12].midShadowS = midShadowS;
        v[12].midShadowT = midShadowT;
        v[12].borderS = (clampedRadius + borderOffset) * borderTextureFactor + borderTextureOffset;
        v[12].borderT = borderOffset * borderTextureFactor + borderTextureOffset;
        v[12].color = packedColor;
        v[12].shadowColor = packedShadowColor;
        v[12].borderColor = packedBorderColor;
        markDirty(QSGNode::DirtyGeometry);

        // Update data for the preprocess() call.
        const quint16 deviceRadius = static_cast<quint16>(clampedRadius * dpr);
        if (m_radius != deviceRadius) {
            m_radius = deviceRadius;
            m_flags |= DirtyRadius;
        }
        const quint16 deviceShadow = static_cast<quint16>(clampedShadow * dpr);
        if (m_shadow != deviceShadow) {
            m_shadow = deviceShadow;
            m_flags |= DirtyShadow;
        }
        const quint16 deviceBorderRadius = static_cast<quint16>(clampedBorderRadius * dpr);
        if (m_borderRadius != deviceBorderRadius) {
            m_borderRadius = deviceBorderRadius;
            m_flags |= DirtyBorderRadius;
        }
        if (m_shape != static_cast<quint8>(type)) {
            m_shape = static_cast<quint8>(type);
            m_flags |= DirtyShape;
        }
        break;
    }

    default:
        DNOT_REACHED();
    }

    // Update the blending state of the opaque material (in QSG terms, an opaque
    // material is the material automatically used when the opacity is 1, but
    // even if the opacity is 1 we have to handle the case where the alpha of
    // the specified color is less than 1).
    const bool blending = qAlpha(color) < 255;
    if (blending != static_cast<bool>(m_flags & Blending)) {
        m_resources->opaqueMaterial()->setFlag(QSGMaterial::Blending, blending);
        markDirty(QSGNode::DirtyMaterial);
        m_flags = (m_flags & ~Blending) | (blending ? Blending : 0);
    }
}

UCShapeFillCornersNode::UCShapeFillCornersNode()
    : QSGGeometryNode()
    , m_resources(Q_NULLPTR)
    , m_radius(0)
    , m_shadow(0)
    , m_shape(UCShapeType::Squircle)
    , m_flags(0)
{
    DLOG("creating UCShapeFillCornersNode");
    setFlag(QSGNode::UsePreprocess, true);
    qsgnode_set_description(this, QLatin1String("shapefillcorners"));
}

UCShapeFillCornersNode::~UCShapeFillCornersNode()
{
    DLOG("detroying UCShapeFillCornersNode");
    delete m_resources;
}

void UCShapeFillCornersNode::preprocess()
{
    if (!(m_flags & HasShadow)) {
        if (m_flags & (DirtyRadius | DirtyShape)) {
            static_cast<UCShapeColorMaskMaterial<false>*>(
                m_resources->material())->updateMaskTexture(
                    static_cast<UCShapeType>(m_shape), m_radius);
            static_cast<UCShapeColorMaskMaterial<true>*>(
                m_resources->opaqueMaterial())->updateMaskTexture(
                    static_cast<UCShapeType>(m_shape), m_radius);
            m_flags &= ~DirtyMask;
        }
    } else {
        if (m_flags & DirtyMask) {
            static_cast<UCShapeFillCornersShadowMaterial<false>*>(
                m_resources->material())->updateTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
            static_cast<UCShapeFillCornersShadowMaterial<true>*>(
                m_resources->opaqueMaterial())->updateTexture(
                    static_cast<UCShapeType>(m_shape), m_radius, m_shadow);
            m_flags &= ~DirtyMask;
        }
    }
}

void UCShapeFillCornersNode::setVisible(bool visible)
{
    DLOG("UCShapeFillCornersNode::setVisible %d", visible);
    if (static_cast<bool>(m_flags & Visible) != visible) {
        m_flags = (m_flags & ~Visible) | (visible ? Visible : 0);
        markDirty(DirtySubtreeBlocked);
    }
}

void UCShapeFillCornersNode::update(
    const QSizeF& itemSize, UCShapeType type, float radius, QRgb color, float shadowSize,
    float shadowAngle, float shadowDistance, QRgb shadowColor, float borderSize, QRgb borderColor)
{
    // FIXME(loicm)
    Q_UNUSED(borderSize); Q_UNUSED(borderColor);

    // The geometry is made of 12 vertices indexed with a triangles mode.
    //     0 - 1    2 - 3
    //     | /        \ |
    //     4            5
    //
    //     6            7
    //     | \        / |
    //     8 - 9   10 - 11
    const quint16 indices[] = { 1, 0, 4, 6, 8, 9, 10, 11, 7, 5, 3, 2 };
    const int indexCount = ARRAY_SIZE(indices);
    const int vertexCount = 12;

    const quint8 style =
        ((shadowSize <= 0.0f) || (qAlpha(shadowColor) == 0)) ? HasColor : (HasColor | HasShadow);

    // Create new material/geometry set if needed.
    if (style != (m_flags & StyleMask)) {
        delete m_resources;
        switch (style) {
        case HasColor:
            m_resources = new UCShapeColorMaskResources(vertexCount, indexCount);
            break;
        case (HasColor | HasShadow):
            m_resources = new UCShapeFillCornersShadowResources(vertexCount, indexCount);
            break;
        default:
            NOT_REACHED();
        }
        setMaterial(m_resources->material());
        m_resources->opaqueMaterial()->setFlag(QSGMaterial::Blending);
        setOpaqueMaterial(m_resources->opaqueMaterial());
        memcpy(m_resources->geometry()->indexData(), indices, indexCount * sizeof(quint16));
        setGeometry(m_resources->geometry());
        m_flags = (m_flags & ~StyleMask) | style;
    }

    const float dpr = qGuiApp->devicePixelRatio();
    const float w = floorf(static_cast<float>(itemSize.width()));
    const float h = floorf(static_cast<float>(itemSize.height()));
    // Rounded down since Shadow doesn't support sub-pixel rendering.
    const float maxSize = floorf(qMin(w, h) * 0.5f);
    const float clampedRadius = qMin(floorf(radius), maxSize);
    const quint32 packedColor = packColor(color);

    // Update geometry depending on the style.
    switch (style) {
    case HasColor: {
        UCShapeColorMaskResources::Vertex* v =
            reinterpret_cast<UCShapeColorMaskResources::Vertex*>(
                m_resources->geometry()->vertexData());
        const float deviceRadius = clampedRadius * dpr;
        const float border = 1.0f;
        const float textureSize = deviceRadius + 2 * border;
        const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
        const float textureStart = (textureSizeRounded - textureSize + border) / textureSizeRounded;
        const float textureEnd = (textureSizeRounded - border) / textureSizeRounded;
        v[0].x = 0.0f;
        v[0].y = 0.0f;
        v[0].maskS = textureStart;
        v[0].maskT = textureStart;
        v[0].color = packedColor;
        v[1].x = clampedRadius;
        v[1].y = 0.0f;
        v[1].maskS = textureEnd;
        v[1].maskT = textureStart;
        v[1].color = packedColor;
        v[2].x = w - clampedRadius;
        v[2].y = 0.0f;
        v[2].maskS = textureEnd;
        v[2].maskT = textureStart;
        v[2].color = packedColor;
        v[3].x = w;
        v[3].y = 0.0f;
        v[3].maskS = textureStart;
        v[3].maskT = textureStart;
        v[3].color = packedColor;
        v[4].x = 0.0f;
        v[4].y = clampedRadius;
        v[4].maskS = textureStart;
        v[4].maskT = textureEnd;
        v[4].color = packedColor;
        v[5].x = w;
        v[5].y = clampedRadius;
        v[5].maskS = textureStart;
        v[5].maskT = textureEnd;
        v[5].color = packedColor;
        v[6].x = 0.0f;
        v[6].y = h - clampedRadius;
        v[6].maskS = textureStart;
        v[6].maskT = textureEnd;
        v[6].color = packedColor;
        v[7].x = w;
        v[7].y = h - clampedRadius;
        v[7].maskS = textureStart;
        v[7].maskT = textureEnd;
        v[7].color = packedColor;
        v[8].x = 0.0f;
        v[8].y = h;
        v[8].maskS = textureStart;
        v[8].maskT = textureStart;
        v[8].color = packedColor;
        v[9].x = clampedRadius;
        v[9].y = h;
        v[9].maskS = textureEnd;
        v[9].maskT = textureStart;
        v[9].color = packedColor;
        v[10].x = w - clampedRadius;
        v[10].y = h;
        v[10].maskS = textureEnd;
        v[10].maskT = textureStart;
        v[10].color = packedColor;
        v[11].x = w;
        v[11].y = h;
        v[11].maskS = textureStart;
        v[11].maskT = textureStart;
        v[11].color = packedColor;
        markDirty(QSGNode::DirtyGeometry);

        // Update data for the preprocess() call.
        if (m_radius != static_cast<quint16>(deviceRadius)) {
            m_radius = static_cast<quint16>(deviceRadius);
            m_flags |= DirtyRadius;
        }
        if (m_shape != static_cast<quint8>(type)) {
            m_shape = static_cast<quint8>(type);
            m_flags |= DirtyShape;
        }
        break;
    }

    case (HasColor | HasShadow): {
        UCShapeFillCornersShadowResources::Vertex* v =
            reinterpret_cast<UCShapeFillCornersShadowResources::Vertex*>(
                m_resources->geometry()->vertexData());
        float s, c;
        sincosf(shadowAngle * -(M_PI / 180.0f), &s, &c);
        const float offsetX = roundf(c * shadowDistance);
        const float offsetY = roundf(s * shadowDistance);
        const float clampedShadow = qMin(floorf(shadowSize), maxSize);
        const float textureSize = (2.0f * clampedShadow + clampedRadius) * dpr;
        const float textureSizeRounded = getStride(static_cast<int>(textureSize), 1, textureStride);
        const float textureOffset = (textureSizeRounded - textureSize) / textureSizeRounded;
        const float textureFactor = ((1.0f - textureOffset) * dpr) / textureSize;
        const float midShadowS = (clampedShadow + floorf(w * 0.5f)) * textureFactor + textureOffset;
        const float midShadowT = (clampedShadow + floorf(h * 0.5f)) * textureFactor + textureOffset;
        const quint32 packedShadowColor = packColor(shadowColor);
        v[0].x = 0.0f;
        v[0].y = 0.0f;
        v[0].maskS = clampedShadow * textureFactor + textureOffset;
        v[0].maskT = clampedShadow * textureFactor + textureOffset;
        v[0].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[0].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[0].midShadowS = midShadowS;
        v[0].midShadowT = midShadowT;
        v[0].color = packedColor;
        v[0].shadowColor = packedShadowColor;
        v[1].x = clampedRadius;
        v[1].y = 0.0f;
        v[1].maskS = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[1].maskT = clampedShadow * textureFactor + textureOffset;
        v[1].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[1].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[1].midShadowS = midShadowS;
        v[1].midShadowT = midShadowT;
        v[1].color = packedColor;
        v[1].shadowColor = packedShadowColor;
        v[2].x = w - clampedRadius;
        v[2].y = 0.0f;
        v[2].maskS = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[2].maskT = clampedShadow * textureFactor + textureOffset;
        v[2].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[2].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[2].midShadowS = midShadowS;
        v[2].midShadowT = midShadowT;
        v[2].color = packedColor;
        v[2].shadowColor = packedShadowColor;
        v[3].x = w;
        v[3].y = 0.0f;
        v[3].maskS = clampedShadow * textureFactor + textureOffset;
        v[3].maskT = clampedShadow * textureFactor + textureOffset;
        v[3].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[3].shadowT = (offsetY + clampedShadow) * textureFactor + textureOffset;
        v[3].midShadowS = midShadowS;
        v[3].midShadowT = midShadowT;
        v[3].color = packedColor;
        v[3].shadowColor = packedShadowColor;
        v[4].x = 0.0f;
        v[4].y = clampedRadius;
        v[4].maskS = clampedShadow * textureFactor + textureOffset;
        v[4].maskT = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[4].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[4].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[4].midShadowS = midShadowS;
        v[4].midShadowT = midShadowT;
        v[4].color = packedColor;
        v[4].shadowColor = packedShadowColor;
        v[5].x = w;
        v[5].y = clampedRadius;
        v[5].maskS = clampedShadow * textureFactor + textureOffset;
        v[5].maskT = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[5].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[5].shadowT = (offsetY + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[5].midShadowS = midShadowS;
        v[5].midShadowT = midShadowT;
        v[5].color = packedColor;
        v[5].shadowColor = packedShadowColor;
        v[6].x = 0.0f;
        v[6].y = h - clampedRadius;
        v[6].maskS = clampedShadow * textureFactor + textureOffset;
        v[6].maskT = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[6].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[6].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[6].midShadowS = midShadowS;
        v[6].midShadowT = midShadowT;
        v[6].color = packedColor;
        v[6].shadowColor = packedShadowColor;
        v[7].x = w;
        v[7].y = h - clampedRadius;
        v[7].maskS = clampedShadow * textureFactor + textureOffset;
        v[7].maskT = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[7].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[7].shadowT =
            (offsetY + clampedShadow + (h - clampedRadius)) * textureFactor + textureOffset;
        v[7].midShadowS = midShadowS;
        v[7].midShadowT = midShadowT;
        v[7].color = packedColor;
        v[7].shadowColor = packedShadowColor;
        v[8].x = 0.0f;
        v[8].y = h;
        v[8].maskS = clampedShadow * textureFactor + textureOffset;
        v[8].maskT = clampedShadow * textureFactor + textureOffset;
        v[8].shadowS = (offsetX + clampedShadow) * textureFactor + textureOffset;
        v[8].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[8].midShadowS = midShadowS;
        v[8].midShadowT = midShadowT;
        v[8].color = packedColor;
        v[8].shadowColor = packedShadowColor;
        v[9].x = clampedRadius;
        v[9].y = h;
        v[9].maskS = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[9].maskT = clampedShadow * textureFactor + textureOffset;
        v[9].shadowS = (offsetX + clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[9].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[9].midShadowS = midShadowS;
        v[9].midShadowT = midShadowT;
        v[9].color = packedColor;
        v[9].shadowColor = packedShadowColor;
        v[10].x = w - clampedRadius;
        v[10].y = h;
        v[10].maskS = (clampedShadow + clampedRadius) * textureFactor + textureOffset;
        v[10].maskT = clampedShadow * textureFactor + textureOffset;
        v[10].shadowS =
            (offsetX + clampedShadow + (w - clampedRadius)) * textureFactor + textureOffset;
        v[10].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[10].midShadowS = midShadowS;
        v[10].midShadowT = midShadowT;
        v[10].color = packedColor;
        v[10].shadowColor = packedShadowColor;
        v[11].x = w;
        v[11].y = h;
        v[11].maskS = clampedShadow * textureFactor + textureOffset;
        v[11].maskT = clampedShadow * textureFactor + textureOffset;
        v[11].shadowS = (offsetX + clampedShadow + w) * textureFactor + textureOffset;
        v[11].shadowT = (offsetY + clampedShadow + h) * textureFactor + textureOffset;
        v[11].midShadowS = midShadowS;
        v[11].midShadowT = midShadowT;
        v[11].color = packedColor;
        v[11].shadowColor = packedShadowColor;
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
        break;
    }

    default:
        DNOT_REACHED();
    }
}
