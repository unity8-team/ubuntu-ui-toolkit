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

#ifndef UCSHAPEUTILS_P_H
#define UCSHAPEUTILS_P_H

#include <QtGui/QColor>
#include <QtQuick/QSGMaterial>

// Logging macros, debug macros are compiled out for release builds.
#define ASSERT(cond) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion `"#cond"' failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define ASSERT_X(cond,message) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion "#message" failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define NOT_REACHED() \
    qFatal("Assertion `not reached' failed in file %s, line %d", __FILE__, __LINE__);
#if !defined(QT_NO_DEBUG)
#define DNOT_REACHED(...) NOT_REACHED()
#define DASSERT(cond) ASSERT(cond)
#define DASSERT_X(cond,message) ASSERT(cond, message)
#else
#define DNOT_REACHED(...) qt_noop()
#define DASSERT(cond) qt_noop()
#define DASSERT_X(cond,message) qt_noop()
#endif

// Compile-time constant representing the number of elements in an array.
template<typename T, size_t N> Q_CONSTEXPR size_t ARRAY_SIZE(T (&)[N]) { return N; }

#define IS_POWER_OF_TWO(n) !((n) & ((n) - 1))

// Squircle SVG defintion and approximate normalised coverage at Pi/4 (used to
// correctly set vertex positions and minimise the amount of rasterised
// fragments of geometry nodes).
const char squircleSvg[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<svg><g>"
    "<path d=\"M35.9998055,36.0003433 L0,36.000344 C0,3.372032 3.345315,0 35.999805,0 "
    "          L35.9998055,36.0003433 Z\" fill=\"#ffffff\"></path>"
    "</g></svg>";
const float squircleCoverage = 0.172f;

// Gaussian kernels. Changing one field requires an update of the others, use
// creategaussianarrays in the tools folder.
const int gaussianCount = 128;
extern const int gaussianOffsets[];
extern const float gaussianKernels[];
extern const float gaussianSums[];

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

Q_CONSTEXPR qreal quantizedU16Max = 4096.0;
Q_CONSTEXPR qreal u16Max = static_cast<qreal>(std::numeric_limits<quint16>::max());
Q_STATIC_ASSERT(quantizedU16Max >= 1.0 && quantizedU16Max <= u16Max);

// Quantize a value in the range [0, quantizedU16Max] from qreal to U16.
static inline quint16 quantizeToU16Clamped(qreal value)
{
    DASSERT(value >= 0.0 && value <= quantizedU16Max);
    return static_cast<quint16>((value * (u16Max / quantizedU16Max)) + 0.5);
}

// Clamp value to the range [0, quantizedU16Max] and quantize from qreal to U16.
static inline quint16 quantizeToU16(qreal value)
{
    return quantizeToU16Clamped(qBound(0.0, value, quantizedU16Max));
}

// Unquantize a value in the range [0, quantizedU16Max] from U16 to qreal.
static inline qreal unquantizeFromU16(quint16 value)
{
    return static_cast<qreal>(value) * (quantizedU16Max / u16Max);
}

// Opaque color material common to most shape items.
class UCShapeOpaqueColorMaterial : public QSGMaterial
{
public:
    UCShapeOpaqueColorMaterial(bool blending = false);
    int compare(const QSGMaterial* other) const override;
    QSGMaterialType* type() const override;
    QSGMaterialShader* createShader() const override;
};

// Color material common to most shape items.
class UCShapeColorMaterial : public UCShapeOpaqueColorMaterial
{
public:
    UCShapeColorMaterial();
    QSGMaterialType* type() const override;
    QSGMaterialShader* createShader() const override;
};

#endif  // UCSHAPEUTILS_P_H
