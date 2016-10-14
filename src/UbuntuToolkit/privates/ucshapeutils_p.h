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
#define LOG(...) qDebug(__VA_ARGS__)
#define WARN(...) qWarning(__VA_ARGS__)
#define ASSERT(cond) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion `"#cond"' failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define ASSERT_X(cond,message) do { if (Q_UNLIKELY(!(cond))) \
    qFatal("Assertion "#message" failed in file %s, line %d", __FILE__, __LINE__); } while (0)
#define NOT_REACHED() \
    qFatal("Assertion `not reached' failed in file %s, line %d", __FILE__, __LINE__);
#if !defined(QT_NO_DEBUG)
#define DLOG(...) LOG(__VA_ARGS__)
#define DWARN(...) WARN(__VA_ARGS__)
#define DASSERT(cond) ASSERT(cond)
#define DASSERT_X(cond,message) ASSERT(cond, message)
#define DNOT_REACHED(...) NOT_REACHED()
#else
#define DLOG(...) qt_noop()
#define DWARN(...) qt_noop()
#define DASSERT(cond) qt_noop()
#define DASSERT_X(cond,message) qt_noop()
#define DNOT_REACHED(...) qt_noop()
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

// Allocates size bytes and returns an pointer to the aligned memory. alignment
// must be a power-of-two and size a multiple of alignment.
// FIXME(loicm) Add a statically compiled lib in src/ to share functions like
//     that between the different UITK libs.
static inline void* alignedAlloc(size_t alignment, size_t size)
{
    DASSERT(IS_POWER_OF_TWO(alignment));
    // DASSERT(((size % alignment) == 0));  // FIXME(loicm) ASSERT doesn't support '%'...

#if defined(__APPLE__)
    void* pointer;
    posix_memalign(&pointer, alignment, size);
    return pointer;
#elif defined(_WIN32)
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

class UCShapeColorOpaqueShader : public QSGMaterialShader
{
public:
    UCShapeColorOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Vertex,
                            QStringLiteral(":/uc/privates/shaders/color.vert"));
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/opaquecolor.frag"));
    }
    char const* const* attributeNames() const Q_DECL_OVERRIDE {
        static char const* const attributes[] = { "positionAttrib", "colorAttrib", 0 };
        return attributes;
    }
    void initialize() Q_DECL_OVERRIDE {
        QSGMaterialShader::initialize();
        m_matrixId = program()->uniformLocation("matrix");
    }
    void updateState(
        const RenderState& state, QSGMaterial*, QSGMaterial*) Q_DECL_OVERRIDE {
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_matrixId, state.combinedMatrix());
        }
    }

private:
    int m_matrixId;
};

class UCShapeColorShader : public UCShapeColorOpaqueShader
{
public:
    UCShapeColorShader() : UCShapeColorOpaqueShader() {
        setShaderSourceFile(QOpenGLShader::Fragment,
                            QStringLiteral(":/uc/privates/shaders/color.frag"));
    }
    void initialize() Q_DECL_OVERRIDE {
        UCShapeColorOpaqueShader::initialize();
        m_opacityId = program()->uniformLocation("opacity");
    }
    void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect) Q_DECL_OVERRIDE {
        UCShapeColorOpaqueShader::updateState(state, newEffect, oldEffect);
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_opacityId, state.opacity());
        }
    }

private:
    int m_opacityId;
};

template <bool opaque>
class UCShapeColorMaterial : public QSGMaterial
{
public:
    UCShapeColorMaterial() {
        setFlag(Blending, !opaque);
    }
    QSGMaterialType* type() const Q_DECL_OVERRIDE {
        static QSGMaterialType type[2];
        return opaque ? &type[0] : &type[1];
    }
    QSGMaterialShader* createShader() const Q_DECL_OVERRIDE {
        return opaque ? new UCShapeColorOpaqueShader : new UCShapeColorShader;
    }
    int compare(const QSGMaterial*) const Q_DECL_OVERRIDE {
        return 0;
    }
};

class UCShapeResources
{
public:
    virtual ~UCShapeResources() {}
    virtual QSGMaterial* material() = 0;
    virtual QSGMaterial* opaqueMaterial() = 0;
    virtual QSGGeometry* geometry() = 0;
};

class UCShapeColorResources : public UCShapeResources
{
public:
    UCShapeColorResources(int vertexCount, int indexCount, int indexType = GL_UNSIGNED_SHORT)
        : m_geometry(attributeSet(), vertexCount, indexCount, indexType) {
        m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
        m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
        m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    }

    QSGMaterial* material() Q_DECL_OVERRIDE { return &m_material; }
    QSGMaterial* opaqueMaterial() Q_DECL_OVERRIDE { return &m_opaqueMaterial; }
    QSGGeometry* geometry() Q_DECL_OVERRIDE { return &m_geometry; }

    struct Vertex { float x, y; quint32 color; };

private:
    static const QSGGeometry::AttributeSet& attributeSet() {
        static const QSGGeometry::Attribute attributes[] = {
            QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),   // x, y
            QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE)  // color
        };
        static const QSGGeometry::AttributeSet attributeSet = {
            2, sizeof(Vertex), attributes
        };
        return attributeSet;
    }

    UCShapeColorMaterial<false> m_material;
    UCShapeColorMaterial<true> m_opaqueMaterial;
    QSGGeometry m_geometry;
};

#endif  // UCSHAPEUTILS_P_H
