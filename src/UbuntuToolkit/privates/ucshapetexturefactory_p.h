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

#ifndef UCSHAPETEXTUREFACTORY_P_H
#define UCSHAPETEXTUREFACTORY_P_H

#include <QtCore/QtGlobal>

#include <unistd.h>

#include <UbuntuToolkit/private/ucshapeutils_p.h>

const int textureRounding = 32;

enum UCShapeType { Squircle = 0, Circle = 1 };

// Reference counted wrapper for textures.
class UCShapeTexture
{
public:
    UCShapeTexture(quint32 id = 0) : m_id(id), m_refCount(1) {}

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
class UCShapeTextureFactory
{
public:
    UCShapeTextureFactory();
    ~UCShapeTextureFactory();

    // Gets the 8-bit texture id of a shape at the given index for the given
    // shape type and radius. The texture size is rounded up to textureRounding
    // (for GPU optimisation reasons), the content padded to the bottom/right
    // and there's a one pixel border on the edges (for clamping reasons). If
    // the given radius is 0, the returned texture is filled with 0.
    quint32 maskTexture(int index, UCShapeType type, quint16 radius);

    // Gets the 16-bit texture id of a gaussian blurred shape at the given index
    // for the given shape type and radius. The texture size is rounded up to
    // textureRounding (for GPU optimisation reasons) and the content padded to
    // the bottom/right (for clamping reasons). The least significant byte
    // stores the outer shadow with the inner area knocked out, the most
    // significant byte stored the inner shadow with the outer area knocked out.
    quint32 shadowTexture(int index, UCShapeType type, quint16 radius, quint16 shadow);

private:
    quint32 acquireTexture(
        int index, quint32 currentKey, quint32 newKey, bool* needsUpdate, bool* isNewtexture);

    // 32-bit key layout:
    //
    //    3                   2                   1                   0
    //  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    // |-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|
    // | | |  Unused   |        Radius         |        Shadow         |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //  ^ ^
    //  | +-- Shape type (0 for Squircle, 1 for Circle).
    //  +---- Texture type (0 for Mask, 1 for Shadow).

    static quint32 makeMaskTextureKey(UCShapeType type, quint16 radius) {
        DASSERT((radius & 0xf000) == 0);
        return static_cast<quint32>(type) << 30
            | static_cast<quint32>(radius) << 12;
    }
    static int maskTextureSize(quint16 radius) {
        DASSERT((radius & 0xf000) == 0);
        return getStride(radius + 2, 1, textureRounding);
    }
    static Q_CONSTEXPR int maskTextureSizeFromKey(quint32 key) {
        return maskTextureSize((key >> 12) & 0xfff);
    }

    static quint32 makeShadowTextureKey(
        UCShapeType type, quint16 radius, quint16 shadow) {
        DASSERT((radius & 0xf000) == 0);
        DASSERT((shadow & 0xf000) == 0);
        return 1 << 31
            | static_cast<quint32>(type) << 30
            | static_cast<quint32>(radius) << 12
            | static_cast<quint32>(shadow);
    }
    static int shadowTextureSize(quint16 radius, quint16 shadow) {
        DASSERT((radius & 0xf000) == 0);
        DASSERT((shadow & 0xf000) == 0);
        return getStride(2 * shadow + radius + 2, 1, textureRounding);
    }
    static Q_CONSTEXPR int shadowTextureSizeFromKey(quint32 key) {
        return shadowTextureSize((key >> 12) & 0xfff, key & 0xfff);
    }

    static int cpuCacheLineSize() {
        static int size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        return size;
    }

    static void renderShape(void* buffer, UCShapeType type, int radius, int stride);
    static quint8* renderMaskTexture(UCShapeType type, int radius);
    static quint16* renderShadowTexture(UCShapeType type, int radius, int shadow);
    static quint16* renderShadowTextureNoShadow(UCShapeType type, int radius);

    const quint32 invalidKey = 0xffffffff;

    // FIXME(loicm) There's one context per key hash, so there might be a way to save a few bytes.
    QOpenGLContext* m_context;
    QHash<quint32, UCShapeTexture>* m_keyHash;
    quint32 m_keys[N];
};

#endif  // UCSHAPETEXTUREFACTORY_P_H
