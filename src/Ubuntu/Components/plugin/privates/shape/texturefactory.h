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

#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "utils.h"
#include <QtCore/qglobal.h>

const int textureStride = 32;

// Reference counted wrapper for textures.
class Texture
{
public:
    Texture(quint32 id = 0) : m_id(id), m_refCount(1) {}

    enum Shape { Squircle = 0, Circle = 1 };

    quint32 id() const { return m_id; }
    quint32 ref() { DASSERT(m_refCount < UINT_MAX); m_refCount++; return m_id; }
    quint32 unref() { DASSERT(m_refCount > 0); return --m_refCount; }

private:
    quint32 m_id;
    quint32 m_refCount;
};

class QOpenGLContext;
template <typename, typename> class QHash;

// Creates and manages shape textures. Must be used in a QSGMaterial
// (constructed in QSGMaterial's constructor and destructed in QSGMaterial's
// destructor). We assume here that QtQuick associates the same OpenGL context
// to a QSGMaterial for its entire lifetime.
template <int N>
class TextureFactory
{
public:
    TextureFactory();
    ~TextureFactory();

    // Gets the 8-bit texture id of a shape at the given index for the given
    // shape type and radius. The texture size is rounded up to textureStride
    // (for GPU optimisation reasons), the content padded to the bottom/right
    // and there's a one pixel border on the edges (for clamping reasons). If
    // the given radius is 0, the returned texture is filled with 0.
    quint32 shapeTexture(int index, Texture::Shape shape, int radius);

    // Gets the 16-bit texture id of a gaussian blurred shape at the given index
    // for the given shape type and radius. The texture size is rounded up to
    // textureStride (for GPU optimisation reasons) and the content padded to
    // the bottom/right (for clamping reasons). The least significant byte
    // stores the outer shadow with the inner area knocked out, the most
    // significant byte stored the inner shadow with the outer area knocked out.
    quint32 shadowTexture(int index, Texture::Shape shape, int radius, int shadow);

private:
    quint32 acquireTexture(
        int index, quint32 currentKey, quint32 newKey, bool* needsUpdate, bool* isNewtexture);

    static Q_CONSTEXPR quint32 makeShapeTextureKey(Texture::Shape shape, quint8 radius) {
        return 1 << 24  // shape texture bit (to distingish from shadow textures)
            | static_cast<quint32>(shape) << 8  // 1 bit
            | static_cast<quint32>(radius);     // 8 bit
    }
    static Q_CONSTEXPR int shapeTextureSizeFromKey(quint32 key) {
        return getStride((key & 0xff) + 2, 1, textureStride);
    }
    static Q_CONSTEXPR int shapeTextureSize(quint8 radius) {
        return getStride(radius + 2, 1, textureStride);
    }

    static Q_CONSTEXPR quint32 makeShadowTextureKey(
        Texture::Shape shape, quint8 radius, quint8 shadow) {
        return static_cast<quint32>(shape) << 16  // 1 bit
            | static_cast<quint32>(radius) << 8   // 8 bit
            | static_cast<quint32>(shadow);       // 8 bit
    }
    static Q_CONSTEXPR int shadowTextureSizeFromKey(quint32 key) {
        return getStride(2 * (key & 0xff) + ((key >> 8) & 0xff), 1, textureStride);
    }
    static Q_CONSTEXPR int shadowTextureSize(quint8 radius, quint8 shadow) {
        return getStride(2 * shadow + radius, 1, textureStride);
    }

    const quint32 invalidKey = 0xffffffff;

    QOpenGLContext* m_context;
    QHash<quint32, Texture>* m_keyHash;
    quint32 m_keys[N];
};

#endif  // TEXTUREFACTORY_H
