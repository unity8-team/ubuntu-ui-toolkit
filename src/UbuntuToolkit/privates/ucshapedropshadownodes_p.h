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

#include <UbuntuToolkit/private/ucshaperesources_p.h>

class UCShapeDropShadowNode : public QSGGeometryNode
{
public:
    UCShapeDropShadowNode();
    ~UCShapeDropShadowNode();

    void preprocess() Q_DECL_OVERRIDE;
    bool isSubtreeBlocked() const Q_DECL_OVERRIDE { return !(m_flags & Visible); }

    void setVisible(bool visible);
    void update(
        const QSizeF& itemSize, UCShapeType type, float radius, float shadowSize, float shadowAngle,
        float shadowDistance, QRgb shadowColor);

private:
    enum {
        DirtyRadius = (1 << 0),
        DirtyShadow = (1 << 1),
        DirtyShape  = (1 << 2),
        DirtyMask   = (DirtyRadius | DirtyShadow | DirtyShape),
        Visible     = (1 << 3),
        Blending    = (1 << 4)
    };

    UCShapeColorMaskResources m_resources;
    quint16 m_radius;
    quint16 m_shadow;
    quint8 m_shape;
    quint8 m_flags;
};

#endif  // UCSHAPEDROPSHADOWNODES_P_H
