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

#ifndef UCSHAPEFRAMENODES_P_H
#define UCSHAPEFRAMENODES_P_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>

class UCShapeFrameEdgesNode : public QSGGeometryNode
{
public:
    UCShapeFrameEdgesNode();
    ~UCShapeFrameEdgesNode();

    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return m_visible == 0; }

    void setVisible(bool visible);
    void update(const QSizeF& itemSize, float radius, float thickness, float space, QRgb color);

private:
    UCShapeColorResources m_resources;
    quint8 m_visible : 1;
    quint8 m_blending : 1;
    quint8 __padding : 6;
};

class UCShapeFrameCornersMaterial : public QSGMaterial
{
public:
    UCShapeFrameCornersMaterial();
    QSGMaterialType* type() const Q_DECL_OVERRIDE;
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE;
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE;

    quint32 outerTextureId() const { return m_textureId[0]; }
    quint32 innerTextureId() const { return m_textureId[1]; }
    void updateTexture(int index, UCShapeType type, quint16 radius);

private:
    UCShapeTextureFactory<2> m_textureFactory;
    quint32 m_textureId[2];
};

class UCShapeFrameCornersNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, outerS, outerT, innerS, innerT; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCShapeFrameCornersNode();
    ~UCShapeFrameCornersNode() { DLOG("detroying UCShapeFrameCornersNode"); }

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return m_visible == 0; }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, float thickness, float space,
        QRgb color);

private:
    UCShapeFrameCornersMaterial m_material;
    QSGGeometry m_geometry;
    // FIXME(loicm) just set a flag instead of storing the 2 values.
    quint16 m_radius[2];
    quint16 m_newRadius[2];
    quint8 m_type : 1;
    quint8 m_newType : 1;
    quint8 m_visible : 1;
    quint8 __padding : 5;
};

#endif  // UCSHAPEFRAMENODES_P_H
