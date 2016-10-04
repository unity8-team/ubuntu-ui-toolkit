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

#ifndef UCSHAPEDROPSHADOWNODES_P_H
#define UCSHAPEDROPSHADOWNODES_P_H

#include <QtQuick/QSGNode>

#include <UbuntuToolkit/private/ucshapetexturefactory_p.h>

class UCShapeDropShadowMaterial : public QSGMaterial
{
public:
    UCShapeDropShadowMaterial();
    QSGMaterialType* type() const Q_DECL_OVERRIDE;
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE;
    int compare(const QSGMaterial* other) const Q_DECL_OVERRIDE;

    quint32 textureId() const { return m_textureId; }
    void updateTexture(UCShapeType type, quint16 radius, quint16 shadow);

private:
    UCShapeTextureFactory<1> m_textureFactory;
    quint32 m_textureId;
};

class UCShapeDropShadowNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y, s, t; quint32 color; };

    static const quint16* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCShapeDropShadowNode();
    ~UCShapeDropShadowNode() { qDebug() << "detroying UCShapeDropShadowNode"; }

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return m_visible == 0; }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, float size, float angle,
        float distance, QRgb color);

private:
    UCShapeDropShadowMaterial m_material;
    QSGGeometry m_geometry;
    quint16 m_size;
    quint16 m_newSize;
    quint16 m_radius;
    quint16 m_newRadius;
    quint8 m_type : 1;
    quint8 m_newType : 1;
    quint8 m_visible : 1;
    quint8 __padding : 5;
};

#endif  // UCSHAPEDROPSHADOWNODES_P_H
