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

#ifndef UCSHAPEFILLNODES_P_H
#define UCSHAPEFILLNODES_P_H

#include <QtQuick/QSGNode>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>

class UCShapeFillCenterNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCShapeFillCenterNode();
    ~UCShapeFillCenterNode() { DLOG("detroying UCShapeFillCenterNode"); }

    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return m_visible == 0; }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, float radius, QRgb color, float shadowSize, float shadowAngle,
        float shadowDistance, QRgb shadowColor);

private:
    UCShapeOpaqueColorMaterial m_opaqueMaterial;
    UCShapeColorMaterial m_material;
    QSGGeometry m_geometry;
    quint8 m_visible : 1;
    quint8 m_blending : 1;
    quint8 __padding : 6;
};

class UCShapeFillCornersMaterial : public QSGMaterial
{
public:
    UCShapeFillCornersMaterial();
    QSGMaterialType* type() const Q_DECL_OVERRIDE;
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE;
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE;

    quint32 textureId() const { return m_textureId; }
    void updateTexture(UCShapeType type, quint16 radius);

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeFillCornersNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, s, t; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCShapeFillCornersNode();
    ~UCShapeFillCornersNode() { DLOG("detroying UCShapeFillCornersNode"); }

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return m_visible == 0; }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, QRgb color, float shadowSize,
        float shadowAngle, float shadowDistance, QRgb shadowColor);

private:
    UCShapeFillCornersMaterial m_material;
    QSGGeometry m_geometry;
    quint8 m_radius;
    quint8 m_newRadius;
    quint8 m_type : 1;
    quint8 m_newType : 1;
    quint8 m_visible : 1;
    quint8 __padding : 5;
};

#endif  // UCSHAPEFILLNODES_P_H
