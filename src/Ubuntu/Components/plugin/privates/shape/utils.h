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

#ifndef UTILS_H
#define UTILS_H

#include <QtQuick/QSGMaterial>
#include <QtGui/QColor>

#define STATIC_ASSERT(cond) static_assert(cond, "`"#cond"'")

// Debug macros compiled out for release builds.
#if !defined(QT_NO_DEBUG)
#define DLOG(...) qDebug(__VA_ARGS__)
#define DLOG_IF(cond,...) do { if (cond) qDebug(__VA_ARGS__); } while (0)
#define DASSERT(cond) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion `"#cond"' failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#else
#define DLOG(...) qt_noop()
#define DLOG_IF(cond,...) qt_noop()
#define DASSERT(cond) qt_noop()
#endif

// Common constants for shape items.
const int defaultRadius = 50;
const int maxRadius = 128;

// Gaussian kernels. Changing one field requires an update of the others, use
// creategaussianarrays in the tools folder.
const int gaussianCount = 128;
extern const int gaussianOffsets[];
extern const float gaussianKernels[];
extern const float gaussianSums[];

// maxRadius can't be higher than gaussianCount. If maxRadius needs to
// be increased, the gaussian kernels must be adapted too.
STATIC_ASSERT(maxRadius == gaussianCount);

// Get the stride of a buffer of the given width and bytes per pixel for a
// specific alignment.
// FIXME(loicm) The bytesPerPixel thing seems broken.
static inline int getStride(int width, int bytesPerPixel, int alignment)
{
    DASSERT(!(bytesPerPixel & (bytesPerPixel - 1)));  // Power-of-two
    DASSERT(!(alignment & (alignment - 1)));          // Power-of-two
    return ((width * bytesPerPixel + alignment - 1) & ~(alignment - 1)) / bytesPerPixel;
}

// Pack a color in a premultiplied ABGR32 value.
static inline quint32 packColor(QRgb color)
{
    const quint32 a = qAlpha(color);
    const quint32 b = ((qBlue(color) * a) + 0xff) >> 8;
    const quint32 g = ((qGreen(color) * a) + 0xff) >> 8;
    const quint32 r = ((qRed(color) * a) + 0xff) >> 8;
    return (a << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
}

// Quantize a value in the range [0, higherBound] from F32 to U16.
static inline quint16 quantizeToU16(float value, float higherBound)
{
    const float u16Max = static_cast<float>(std::numeric_limits<quint16>::max());
    DASSERT(higherBound >= 0.0f || higherBound <= u16Max);
    DASSERT(value >= 0.0f || value <= u16Max);
    return static_cast<quint16>((value * (u16Max / higherBound)) + 0.5f);
}

// Unquantize a value in the range [0, higherBound] from U16 to F32.
static inline float unquantizeFromU16(quint16 value, float higherBound)
{
    const float u16Max = static_cast<float>(std::numeric_limits<quint16>::max());
    DASSERT(higherBound >= 0.0f || higherBound <= u16Max);
    return static_cast<float>(value) * (higherBound / u16Max);
}

// Opaque color material common to most shape items.
class UCOpaqueColorMaterial : public QSGMaterial
{
public:
    UCOpaqueColorMaterial(bool blending = false);
    virtual int compare(const QSGMaterial* other) const;
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
};

// Color material common to most shape items.
class UCColorMaterial : public UCOpaqueColorMaterial
{
public:
    UCColorMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
};

#endif  // UTILS_H
