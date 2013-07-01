/*
 * Copyright 2013 Canonical Ltd.
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

// FIXME(loicm) Add support for corner squareness.
// FIXME(loicm) Add support for inner shadow.
// FIXME(loicm) Current outer shadow doesn't look good when adjacent radii difference is big.
// FIXME(loicm) Overlay glitches.

#include "shapeitem2.h"
#include "ucunits.h"
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>

// Retrieves the size of an array at compile time.
#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

static const char* const shapeVertexShader =
    "uniform highp mat4 matrix;                 \n"
    "attribute highp vec4 positionAttrib;       \n"
    "attribute highp vec4 shapeCoordAttrib;     \n"
    "attribute highp vec2 sourceCoordAttrib;    \n"
    "varying highp vec4 shapeCoord;             \n"
    "varying highp vec2 sourceCoord;            \n"
    "void main()                                \n"
    "{                                          \n"
    "    shapeCoord = shapeCoordAttrib;         \n"
    "    sourceCoord = sourceCoordAttrib;       \n"
    "    gl_Position = matrix * positionAttrib; \n"
    "}";

// FIXME(loicm) Detail the algorithm.
static const char* const shapeFragmentShader =
    "uniform highp float opacity;                                                                \n"
    "uniform highp vec4 bgColor;                                                                 \n"
    "uniform highp vec4 secondaryBgColor;                                                        \n"
    "uniform highp int gradientDirection;                                                        \n"
    "uniform highp vec4 overlayColor;                                                            \n"
    // "uniform highp vec4 shadowColorIn;                                                           \n"
    "uniform highp vec4 shadowColorOut;                                                          \n"
    "uniform highp vec4 overlaySteps;                                                         \n"
    // "uniform highp vec4 shadowRadiusIn;                                                          \n"
    // "uniform highp vec4 shadowRadiusOut;                                                         \n"
    // "uniform highp float cornerSquareness;                                                       \n"
    "uniform highp float cornerRadius;                                                           \n"
    "uniform sampler2D sourceTexture;                                                            \n"
    "varying highp vec4 shapeCoord;                                                              \n"
    "varying highp vec2 sourceCoord;                                                             \n"
    "void main()                                                                                 \n"
    "{                                                                                           \n"
    //   Early texture fetch to cover latency.
    "    highp vec4 source = texture2D(sourceTexture, sourceCoord);                              \n"
    //   Store data that's reused more than once.
    "    highp float coordLength = length(shapeCoord.xy) - 1.0;                                  \n"
    "    highp float halfPix = 0.5 / cornerRadius;                                               \n"
    //   Compute outer shadow.
    "    highp float angle = dot(vec2(0.0, 1.0), normalize(shapeCoord.xy / shapeCoord.zw));      \n"
    "    highp float gradientLength = mix(shapeCoord.z, shapeCoord.w, angle * angle) - 1.0;      \n"
    "    highp float gradient = min(1.0, (coordLength + halfPix) / (gradientLength + halfPix));  \n"
    "    highp float improvedGradient = (gradient * (gradient - 2.0)) + 1.0;                     \n"
    "    highp vec4 shadow = vec4(improvedGradient) * shadowColorOut;                            \n"
    //   Compute background.
    "    highp vec4 bgGradient = mix(bgColor, secondaryBgColor, sourceCoord[gradientDirection]); \n"
    //   Compute overlay.
    "    highp vec4 steps = step(overlaySteps, sourceCoord.xyxy);                                \n"
    "    steps.xy = steps.xy * (vec2(1.0) - steps.zw);                                           \n"
    "    highp vec4 overlay = vec4(steps.x * steps.y) * overlayColor;                            \n"
    //   Bend overlay over sources over background and shape the result.
    "    highp vec4 shape = source + vec4(1.0 - source.a) * bgGradient;                          \n"
    "    shape = overlay + vec4(1.0 - overlay.a) * shape;                                        \n"
    "    shape *= vec4(max(0.0, min(1.0, (coordLength - halfPix) * -cornerRadius)));             \n"
    //   Blend resulting shape over shadow.
    "    highp vec4 blend = shape + vec4(1.0 - shape.a) * shadow;                                \n"
    //   Store final fragment color taking care of scenegraph node opacity.
    "    gl_FragColor = blend * vec4(opacity);                                                   \n"
    "}";

static const unsigned short shapeMeshIndices[] __attribute__((aligned(16))) = {
    0, 4, 1, 5, 2, 6, 3, 7,       // Triangles 1 to 6.
    7, 4,                         // Degenerate triangles.
    4, 8, 5, 9, 6, 10, 7, 11,     // Triangles 7 to 12.
    11, 8,                        // Degenerate triangles.
    8, 12, 9, 13, 10, 14, 11, 15  // Triangles 13 to 18
};

static const struct {
    const unsigned short* const indices;
    int indexCount;        // Number of indices.
    int vertexCount;       // Number of vertices.
    int attributeCount;    // Number of attributes.
    int stride;            // Offset in bytes from one vertex to the other.
    int positionCount;     // Number of components per position.
    int positionType;      // OpenGL type of the position components.
    int shapeCoordCount;   // Number of components per shape texture coordinate.
    int shapeCoordType;    // OpenGL type of the shape texture coordinate components.
    int sourceCoordCount;  // Number of components per image texture coordinate.
    int sourceCoordType;   // OpenGL type of the image texture coordinate components.
    int indexType;         // OpenGL type of the indices.
} shapeMesh = {
    shapeMeshIndices, ARRAY_SIZE(shapeMeshIndices),
    16, 3, sizeof(ShapeNode2::Vertex), 2, GL_FLOAT, 4, GL_FLOAT, 2, GL_FLOAT, GL_UNSIGNED_SHORT
};

static const QSGGeometry::AttributeSet& getAttributes()
{
    static QSGGeometry::Attribute data[] = {
        QSGGeometry::Attribute::create(0, shapeMesh.positionCount, shapeMesh.positionType, true),
        QSGGeometry::Attribute::create(1, shapeMesh.shapeCoordCount, shapeMesh.shapeCoordType),
        QSGGeometry::Attribute::create(2, shapeMesh.sourceCoordCount, shapeMesh.sourceCoordType)
    };
    static QSGGeometry::AttributeSet attributes = {
        shapeMesh.attributeCount, shapeMesh.stride, data
    };
    return attributes;
}

// Gets the size in bytes of an OpenGL type in the range [GL_BYTE, GL_DOUBLE].
static int sizeOfType(GLenum type)
{
    static int sizes[] = {
        sizeof(char), sizeof(unsigned char), sizeof(short), sizeof(unsigned short), sizeof(int),
        sizeof(unsigned int), sizeof(float), 2, 3, 4, sizeof(double)
    };
    Q_ASSERT(type >= 0x1400 && type <= 0x140a);
    return sizes[type - 0x1400];
}

// --- QtQuick item ---

MaterialData::MaterialData()
    : sourceTextureProvider(NULL)
    , backgroundColorPremultiplied(1.0f, 1.0f, 1.0f, 1.0f)
    , secondaryBackgroundColorPremultiplied(1.0f, 1.0f, 1.0f, 1.0f)
    , overlayColorPremultiplied(0.0f, 0.0f, 0.0f, 0.75f)
    , shadowColorInPremultiplied(0.0f, 0.0f, 0.0f, 1.0f)
    , shadowColorOutPremultiplied(0.0f, 0.0f, 0.0f, 1.0f)
    , overlaySteps(0.0f, 0.0f, 0.0f, 0.0f)
    , shadowRadiusOut(0.0f, 0.0f, 0.0f, 0.0f)
    , shadowRadiusIn(0.0f, 0.0f, 0.0f, 0.0f)
    , cornerSquareness(0.0f)
    , adjustedCornerRadius(0.0f)
    , backgroundFillMode(ShapeItem2::BackgroundColor)
{
}

ShapeItem2::ShapeItem2(QQuickItem* parent)
    : QQuickItem(parent)
    , source_(NULL)
    , geometry_()
    , cornerRadius_(25.0f)
    , backgroundColor_(1.0f, 1.0f, 1.0f, 1.0f)
    , secondaryBackgroundColor_(1.0f, 1.0f, 1.0f, 1.0f)
    , overlayColor_(0.0f, 0.0f, 0.0f, 0.75f)
    , overlayGeometry_(0.0f, 0.0f, 0.0f, 0.0f)
    , shadowColorIn_(0.0f, 0.0f, 0.0f, 1.0f)
    , shadowColorOut_(0.0f, 0.0f, 0.0f, 1.0f)
    , fillMode_(ShapeItem2::PreserveAspectCrop)
    , horizontalAlignment_(ShapeItem2::AlignHCenter)
    , verticalAlignment_(ShapeItem2::AlignVCenter)
    , materialData_()
{
    setFlag(ItemHasContents);
    update();
}

ShapeItem2::~ShapeItem2()
{
    // FIXME(loicm) Should unparent the source item here?
}

void ShapeItem2::setCornerSquareness(float cornerSquareness)
{
    if (materialData_.cornerSquareness != cornerSquareness) {
        materialData_.cornerSquareness = cornerSquareness;
        update();
        Q_EMIT cornerSquarenessChanged();
    }
}

void ShapeItem2::setCornerRadius(float cornerRadius)
{
    cornerRadius = qMax(1.0f, cornerRadius);
    if (cornerRadius_ != cornerRadius) {
        // The adjusted corner radius is computed in updatePaintNode().
        cornerRadius_ = cornerRadius;
        update();
        Q_EMIT cornerRadiusChanged();
    }
}

void ShapeItem2::setBackgroundColor(const QColor& color)
{
    if (backgroundColor_ != color) {
        const float alpha = 1.0f; // FIXME(loicm) Add support for translucent background.
        materialData_.backgroundColorPremultiplied = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        backgroundColor_ = color;
        update();
        Q_EMIT backgroundColorChanged();
    }
}

void ShapeItem2::setSecondaryBackgroundColor(const QColor& color)
{
    if (secondaryBackgroundColor_ != color) {
        const float alpha = 1.0f;  // FIXME(loicm) Add support for translucent background.
        materialData_.secondaryBackgroundColorPremultiplied = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        secondaryBackgroundColor_ = color;
        update();
        Q_EMIT secondaryBackgroundColorChanged();
    }
}

void ShapeItem2::setBackgroundFillMode(BackgroundFillMode backgroundFillMode)
{
    if (materialData_.backgroundFillMode != backgroundFillMode) {
        materialData_.backgroundFillMode = backgroundFillMode;
        update();
        Q_EMIT backgroundFillModeChanged();
    }
}

void ShapeItem2::setOverlayGeometry(const QRectF& overlayGeometry)
{
    if (overlayGeometry_ != overlayGeometry) {
        materialData_.overlaySteps = QVector4D(
            overlayGeometry.x(), overlayGeometry.y(), overlayGeometry.x() + overlayGeometry.width(),
            overlayGeometry.height() + overlayGeometry.y());
        overlayGeometry_ = overlayGeometry;
        update();
        Q_EMIT overlayGeometryChanged();
    }
}

void ShapeItem2::setOverlayColor(const QColor& color)
{
    if (overlayColor_ != color) {
        const float alpha = color.alphaF();
        materialData_.overlayColorPremultiplied = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        overlayColor_ = color;
        update();
        Q_EMIT overlayColorChanged();
    }
}

void ShapeItem2::setShadowRadiusIn(const QVector4D& shadowRadiusIn)
{
    QVector4D shadowRadiusInClamped(
        qMax(0.0f, shadowRadiusIn.x()), qMax(0.0f, shadowRadiusIn.y()),
        qMax(0.0f, shadowRadiusIn.z()), qMax(0.0f, shadowRadiusIn.w()));
    if (materialData_.shadowRadiusIn != shadowRadiusInClamped) {
        materialData_.shadowRadiusIn = shadowRadiusInClamped;
        update();
        Q_EMIT shadowRadiusInChanged();
    }
}

void ShapeItem2::setShadowColorIn(const QColor& color)
{
    if (shadowColorIn_ != color) {
        const float alpha = color.alphaF();
        materialData_.shadowColorInPremultiplied = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        shadowColorIn_ = color;
        update();
        Q_EMIT shadowColorInChanged();
    }
}

void ShapeItem2::setShadowRadiusOut(const QVector4D& shadowRadiusOut)
{
    QVector4D shadowRadiusOutClamped(
        qMax(0.0f, shadowRadiusOut.x()), qMax(0.0f, shadowRadiusOut.y()),
        qMax(0.0f, shadowRadiusOut.z()), qMax(0.0f, shadowRadiusOut.w()));
    if (materialData_.shadowRadiusOut != shadowRadiusOutClamped) {
        materialData_.shadowRadiusOut = shadowRadiusOutClamped;
        update();
        Q_EMIT shadowRadiusOutChanged();
    }
}

void ShapeItem2::setShadowColorOut(const QColor& color)
{
    if (shadowColorOut_ != color) {
        const float alpha = color.alphaF();
        materialData_.shadowColorOutPremultiplied = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        shadowColorOut_ = color;
        update();
        Q_EMIT shadowColorOutChanged();
    }
}

void ShapeItem2::setSource(QVariant source)
{
    QQuickItem* castedSource = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(source));
    if (source_ != castedSource) {
        source_ = castedSource;
        if (castedSource && !castedSource->parentItem()) {
            // Inlined images need a parent and must not be visible.
            castedSource->setParentItem(this);
            castedSource->setVisible(false);
        }
        update();
        Q_EMIT sourceChanged();
    }
}

void ShapeItem2::setFillMode(FillMode fillMode)
{
    if (fillMode_ != fillMode) {
        fillMode_ = fillMode;
        update();
        Q_EMIT fillModeChanged();
    }
}

void ShapeItem2::setHorizontalAlignment(HAlignment horizontalAlignment)
{
    if (horizontalAlignment_ != horizontalAlignment) {
        horizontalAlignment_ = horizontalAlignment;
        update();
        Q_EMIT horizontalAlignmentChanged();
    }
}

void ShapeItem2::setVerticalAlignment(VAlignment verticalAlignment)
{
    if (verticalAlignment_ != verticalAlignment) {
        verticalAlignment_ = verticalAlignment;
        update();
        Q_EMIT verticalAlignmentChanged();
    }
}

void ShapeItem2::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    geometry_ = newGeometry;
    update();
}

QSGNode* ShapeItem2::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    // Get dimensions without shadows.
    const float width = geometry_.width()
            - materialData_.shadowRadiusOut.x() - materialData_.shadowRadiusOut.y();
    const float height = geometry_.height()
            - materialData_.shadowRadiusOut.z() - materialData_.shadowRadiusOut.w();
    if (width <= 0.0f || height <= 0.0f) {
        delete oldNode;
        return NULL;
    }

    // Get source texture. Texture provider must be fetched from here, not from setSource(), since
    // it deals with the scene.
    QSGTexture* sourceTexture;
    materialData_.sourceTextureProvider = source_ ? source_->textureProvider() : NULL;
    if (materialData_.sourceTextureProvider) {
        sourceTexture = materialData_.sourceTextureProvider->texture();
        // The image item sets its texture in its updatePaintNode() method when QtQuick iterates
        // through the list of dirty items. When we're notified the image item has been changed
        // through setImage(), we mark the shape item as dirty by requesting an update. But
        // sometimes it leads to have the shape item being queued in the dirty list before the image
        // item. That case can be detected when the texture provider exists but not the texture
        // itself. When that's the case we push the shape item in the dirty list to be handled next
        // frame and we tell QtQuick not to render the item for the current frame.
        if (!sourceTexture) {
            // FIXME(loicm) Reached when image hasn't been loaded correctly, it shouldn't.
            update();
            delete oldNode;
            return NULL;
        }
    } else {
        sourceTexture = NULL;
    }

    // Adjust corner radius taking care of radii bigger than item dimensions.
    const float size = qMin(0.0f, qMin(width, height) - (2.0f * cornerRadius_));
    materialData_.adjustedCornerRadius = cornerRadius_ + size * 0.5;

    // .
    ShapeNode2* node = oldNode ? static_cast<ShapeNode2*>(oldNode) : new ShapeNode2(this);
    node->material()->setData(&materialData_);
    node->setVertices(geometry_, width, height, materialData_.adjustedCornerRadius,
                      materialData_.shadowRadiusOut, materialData_.shadowRadiusIn, sourceTexture,
                      fillMode_, horizontalAlignment_, verticalAlignment_);

    return node;
}

// --- Scene graph geometry node ---

ShapeNode2::ShapeNode2(ShapeItem2* item)
    : QSGGeometryNode()
    , item_(item)
    , geometry_(getAttributes(), shapeMesh.vertexCount, shapeMesh.indexCount, shapeMesh.indexType)
    , material_()
{
    memcpy(geometry_.indexData(), shapeMesh.indices,
           shapeMesh.indexCount * sizeOfType(shapeMesh.indexType));
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    setGeometry(&geometry_);
    setMaterial(&material_);
    setFlag(UsePreprocess, false);
}

void ShapeNode2::setVertices(const QRectF& geometry, float noShadowWidth, float noShadowHeight,
                             float cornerRadius, const QVector4D& shadowRadiusOut,
                             const QVector4D& shadowRadiusIn, QSGTexture* texture,
                             ShapeItem2::FillMode fillMode,
                             ShapeItem2::HAlignment horizontalAlignment,
                             ShapeItem2::VAlignment verticalAlignment)
{
    // float topCoordinate;
    // float bottomCoordinate;
    // float leftCoordinate;
    // float rightCoordinate;
    // float radiusCoordinateWidth;
    // float radiusCoordinateHeight;

    // if (!stretched && texture) {
    //     // Preserve source image aspect ratio cropping areas exceeding destination rectangle.
    //     const float factors[3] = { 0.0f, 0.5f, 1.0f };
    //     const QSize srcSize = texture->textureSize();
    //     const float srcRatio = static_cast<float>(srcSize.width()) / srcSize.height();
    //     const float dstRatio = static_cast<float>(width) / height;
    //     if (dstRatio <= srcRatio) {
    //         const float inCoordinateSize = dstRatio / srcRatio;
    //         const float outCoordinateSize = 1.0f - inCoordinateSize;
    //         topCoordinate = 0.0f;
    //         bottomCoordinate = 1.0f;
    //         leftCoordinate = outCoordinateSize * factors[hAlignment];
    //         rightCoordinate = 1.0f - (outCoordinateSize * (1.0f - factors[hAlignment]));
    //         radiusCoordinateHeight = cornerRadius / height;
    //         radiusCoordinateWidth = (cornerRadius / width) * inCoordinateSize;
    //     } else {
    //         const float inCoordinateSize = srcRatio / dstRatio;
    //         const float outCoordinateSize = 1.0f - inCoordinateSize;
    //         topCoordinate = outCoordinateSize * factors[vAlignment];
    //         bottomCoordinate = 1.0f - (outCoordinateSize * (1.0f - factors[vAlignment]));
    //         leftCoordinate = 0.0f;
    //         rightCoordinate = 1.0f;
    //         radiusCoordinateHeight = (cornerRadius / height) * inCoordinateSize;
    //         radiusCoordinateWidth = cornerRadius / width;
    //     }
    // } else {
        // Don't preserve source image aspect ratio stretching it in destination rectangle.
        // topCoordinate = 0.0f;
        // bottomCoordinate = 1.0f;
        // leftCoordinate = 0.0f;
        // rightCoordinate = 1.0f;
        // radiusCoordinateHeight = cornerRadius / height;
        // radiusCoordinateWidth = cornerRadius / width;
    // }

    ShapeNode2::Vertex* vertices = reinterpret_cast<ShapeNode2::Vertex*>(geometry_.vertexData());
    const float width = geometry.width();
    const float height = geometry.height();
    const float normCornerRadiusWidth = cornerRadius / noShadowWidth;
    const float normCornerRadiusHeight = cornerRadius / noShadowHeight;
    const float normShadowRadiusLeft = shadowRadiusOut.x() / noShadowWidth;
    const float normShadowRadiusRight = shadowRadiusOut.y() / noShadowWidth;
    const float normShadowRadiusTop = shadowRadiusOut.z() / noShadowHeight;
    const float normShadowRadiusBottom = shadowRadiusOut.w() / noShadowHeight;
    // FIXME(loicm) Handle divisions by 0.
    const float shapeCoordinateLeft = (shadowRadiusOut.x() + cornerRadius) / cornerRadius;
    const float shapeCoordinateRight = (shadowRadiusOut.y() + cornerRadius) / cornerRadius;
    const float shapeCoordinateTop = (shadowRadiusOut.z() + cornerRadius) / cornerRadius;
    const float shapeCoordinateBottom = (shadowRadiusOut.w() + cornerRadius) / cornerRadius;
    const float shapeCoordinateFactorLeft = shapeCoordinateLeft;
    const float shapeCoordinateFactorRight = shapeCoordinateRight;
    const float shapeCoordinateFactorTop = shapeCoordinateTop;
    const float shapeCoordinateFactorBottom = shapeCoordinateBottom;

    // Set top row of 4 vertices.
    vertices[0].position[0] = 0.0f;
    vertices[0].position[1] = 0.0f;
    vertices[0].shapeCoordinate[0] = shapeCoordinateLeft;
    vertices[0].shapeCoordinate[1] = shapeCoordinateTop;
    vertices[0].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[0].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[0].sourceCoordinate[0] = -normShadowRadiusLeft;
    vertices[0].sourceCoordinate[1] = -normShadowRadiusTop;
    vertices[1].position[0] = cornerRadius + shadowRadiusOut.x();
    vertices[1].position[1] = 0.0f;
    vertices[1].shapeCoordinate[0] = FLT_EPSILON;
    vertices[1].shapeCoordinate[1] = shapeCoordinateTop;
    vertices[1].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[1].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[1].sourceCoordinate[0] = normCornerRadiusWidth;
    vertices[1].sourceCoordinate[1] = -normShadowRadiusTop;
    vertices[2].position[0] = width - cornerRadius - shadowRadiusOut.y();
    vertices[2].position[1] = 0.0f;
    vertices[2].shapeCoordinate[0] = FLT_EPSILON;
    vertices[2].shapeCoordinate[1] = shapeCoordinateTop;
    vertices[2].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[2].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[2].sourceCoordinate[0] = 1.0f - normCornerRadiusWidth;
    vertices[2].sourceCoordinate[1] = -normShadowRadiusTop;
    vertices[3].position[0] = width;
    vertices[3].position[1] = 0.0f;
    vertices[3].shapeCoordinate[0] = shapeCoordinateRight;
    vertices[3].shapeCoordinate[1] = shapeCoordinateTop;
    vertices[3].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[3].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[3].sourceCoordinate[0] = 1.0f + normShadowRadiusRight;
    vertices[3].sourceCoordinate[1] = -normShadowRadiusTop;

    // Set middle-top row of 4 vertices.
    vertices[4].position[0] = 0.0f;
    vertices[4].position[1] = cornerRadius + shadowRadiusOut.z();
    vertices[4].shapeCoordinate[0] = shapeCoordinateLeft;
    vertices[4].shapeCoordinate[1] = FLT_EPSILON;
    vertices[4].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[4].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[4].sourceCoordinate[0] = -normShadowRadiusLeft;
    vertices[4].sourceCoordinate[1] = normCornerRadiusHeight;
    vertices[5].position[0] = cornerRadius + shadowRadiusOut.x();
    vertices[5].position[1] = cornerRadius + shadowRadiusOut.z();
    vertices[5].shapeCoordinate[0] = FLT_EPSILON;
    vertices[5].shapeCoordinate[1] = FLT_EPSILON;
    vertices[5].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[5].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[5].sourceCoordinate[0] = normCornerRadiusWidth;
    vertices[5].sourceCoordinate[1] = normCornerRadiusHeight;
    vertices[6].position[0] = width - cornerRadius - shadowRadiusOut.y();
    vertices[6].position[1] = cornerRadius + shadowRadiusOut.z();
    vertices[6].shapeCoordinate[0] = FLT_EPSILON;
    vertices[6].shapeCoordinate[1] = FLT_EPSILON;
    vertices[6].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[6].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[6].sourceCoordinate[0] = 1.0f - normCornerRadiusWidth;
    vertices[6].sourceCoordinate[1] = normCornerRadiusHeight;
    vertices[7].position[0] = width;
    vertices[7].position[1] = cornerRadius + shadowRadiusOut.z();
    vertices[7].shapeCoordinate[0] = shapeCoordinateRight;
    vertices[7].shapeCoordinate[1] = FLT_EPSILON;
    vertices[7].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[7].shapeCoordinate[3] = shapeCoordinateFactorTop;
    vertices[7].sourceCoordinate[0] = 1.0f + normShadowRadiusRight;
    vertices[7].sourceCoordinate[1] = normCornerRadiusHeight;

    // Set middle-bottom row of 4 vertices.
    vertices[8].position[0] = 0.0f;
    vertices[8].position[1] = height - cornerRadius - shadowRadiusOut.w();
    vertices[8].shapeCoordinate[0] = shapeCoordinateLeft;
    vertices[8].shapeCoordinate[1] = FLT_EPSILON;
    vertices[8].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[8].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[8].sourceCoordinate[0] = -normShadowRadiusLeft;
    vertices[8].sourceCoordinate[1] = 1.0f - normCornerRadiusHeight;
    vertices[9].position[0] = cornerRadius + shadowRadiusOut.x();
    vertices[9].position[1] = height - cornerRadius - shadowRadiusOut.w();
    vertices[9].shapeCoordinate[0] = FLT_EPSILON;
    vertices[9].shapeCoordinate[1] = FLT_EPSILON;
    vertices[9].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[9].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[9].sourceCoordinate[0] = normCornerRadiusWidth;
    vertices[9].sourceCoordinate[1] = 1.0f - normCornerRadiusHeight;
    vertices[10].position[0] = width - cornerRadius - shadowRadiusOut.y();
    vertices[10].position[1] = height - cornerRadius - shadowRadiusOut.w();
    vertices[10].shapeCoordinate[0] = FLT_EPSILON;
    vertices[10].shapeCoordinate[1] = FLT_EPSILON;
    vertices[10].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[10].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[10].sourceCoordinate[0] = 1.0f - normCornerRadiusWidth;
    vertices[10].sourceCoordinate[1] = 1.0f - normCornerRadiusHeight;
    vertices[11].position[0] = width;
    vertices[11].position[1] = height - cornerRadius - shadowRadiusOut.w();
    vertices[11].shapeCoordinate[0] = shapeCoordinateRight;
    vertices[11].shapeCoordinate[1] = FLT_EPSILON;
    vertices[11].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[11].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[11].sourceCoordinate[0] = 1.0f + normShadowRadiusRight;
    vertices[11].sourceCoordinate[1] = 1.0f - normCornerRadiusHeight;

    // Set bottom row of 4 vertices.
    vertices[12].position[0] = 0.0f;
    vertices[12].position[1] = height;
    vertices[12].shapeCoordinate[0] = shapeCoordinateLeft;
    vertices[12].shapeCoordinate[1] = shapeCoordinateBottom;
    vertices[12].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[12].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[12].sourceCoordinate[0] = -normShadowRadiusLeft;
    vertices[12].sourceCoordinate[1] = 1.0f + normShadowRadiusBottom;
    vertices[13].position[0] = cornerRadius + shadowRadiusOut.x();
    vertices[13].position[1] = height;
    vertices[13].shapeCoordinate[0] = FLT_EPSILON;
    vertices[13].shapeCoordinate[1] = shapeCoordinateBottom;
    vertices[13].shapeCoordinate[2] = shapeCoordinateFactorLeft;
    vertices[13].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[13].sourceCoordinate[0] = normCornerRadiusWidth;
    vertices[13].sourceCoordinate[1] = 1.0f + normShadowRadiusBottom;
    vertices[14].position[0] = width - cornerRadius - shadowRadiusOut.y();
    vertices[14].position[1] = height;
    vertices[14].shapeCoordinate[0] = FLT_EPSILON;
    vertices[14].shapeCoordinate[1] = shapeCoordinateBottom;
    vertices[14].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[14].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[14].sourceCoordinate[0] = 1.0f - normCornerRadiusWidth;
    vertices[14].sourceCoordinate[1] = 1.0f + normShadowRadiusBottom;
    vertices[15].position[0] = width;
    vertices[15].position[1] = height;
    vertices[15].shapeCoordinate[0] = shapeCoordinateRight;
    vertices[15].shapeCoordinate[1] = shapeCoordinateBottom;
    vertices[15].shapeCoordinate[2] = shapeCoordinateFactorRight;
    vertices[15].shapeCoordinate[3] = shapeCoordinateFactorBottom;
    vertices[15].sourceCoordinate[0] = 1.0f + normShadowRadiusRight;
    vertices[15].sourceCoordinate[1] = 1.0f + normShadowRadiusBottom;

    markDirty(DirtyGeometry);
}

// --- Scene graph material ---

ShapeMaterial2::ShapeMaterial2()
    : data_()
{
    setFlag(Blending);
}

QSGMaterialType* ShapeMaterial2::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* ShapeMaterial2::createShader() const
{
    return new ShapeShader2();
}

void ShapeMaterial2::setData(MaterialData* data)
{
    memcpy(&data_, data, sizeof(MaterialData));
}

// -- Scene graph material shader ---

const char *ShapeShader2::vertexShader() const
{
    return shapeVertexShader;
}

const char* ShapeShader2::fragmentShader() const
{
    return shapeFragmentShader;
}

char const* const* ShapeShader2::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "shapeCoordAttrib", "sourceCoordAttrib", 0
    };
    return attributes;
}

void ShapeShader2::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("sourceTexture", 0);
    matrixId_ = program()->uniformLocation("matrix");
    opacityId_ = program()->uniformLocation("opacity");
    backgroundColorId_ = program()->uniformLocation("bgColor");
    secondaryBackgroundColorId_ = program()->uniformLocation("secondaryBgColor");
    overlayColorId_ = program()->uniformLocation("overlayColor");
    // shadowColorInId_ = program()->uniformLocation("shadowColorIn");
    shadowColorOutId_ = program()->uniformLocation("shadowColorOut");
    overlayStepsId_ = program()->uniformLocation("overlaySteps");
    // shadowRadiusInId_ = program()->uniformLocation("shadowRadiusIn");
    // shadowRadiusOutId_ = program()->uniformLocation("shadowRadiusOut");
    // cornerSquarenessId_ = program()->uniformLocation("cornerSquareness");
    cornerRadiusId_ = program()->uniformLocation("cornerRadius");
    gradientDirectionId_ = program()->uniformLocation("gradientDirection");
}

void ShapeShader2::updateState(const RenderState& state, QSGMaterial* newEffect,
                               QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);
    const int gradientTable[] = { 0, 0, 1 };

    MaterialData* data = static_cast<ShapeMaterial2*>(newEffect)->data();

    QSGTextureProvider* textureProvider = data->sourceTextureProvider;
    QSGTexture* texture = textureProvider ? textureProvider->texture() : NULL;
    if (texture)
        texture->bind();
    else
        glBindTexture(GL_TEXTURE_2D, 0);

    if (state.isMatrixDirty())
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    if (state.isOpacityDirty())
        program()->setUniformValue(opacityId_, state.opacity());
    program()->setUniformValue(backgroundColorId_, data->backgroundColorPremultiplied);
    program()->setUniformValue(
        secondaryBackgroundColorId_, data->backgroundFillMode == ShapeItem2::BackgroundColor ?
        data->backgroundColorPremultiplied : data->secondaryBackgroundColorPremultiplied);
    program()->setUniformValue(overlayColorId_, data->overlayColorPremultiplied);
    // program()->setUniformValue(shadowColorInId_, data->shadowColorInPremultiplied);
    program()->setUniformValue(shadowColorOutId_, data->shadowColorOutPremultiplied);
    program()->setUniformValue(overlayStepsId_, data->overlaySteps);
    // program()->setUniformValue(shadowRadiusInId_, data->shadowRadiusIn);
    // program()->setUniformValue(shadowRadiusOutId_, data->shadowRadiusOut);
    // program()->setUniformValue(cornerSquarenessId_, data->cornerSquareness);
    program()->setUniformValue(cornerRadiusId_, data->adjustedCornerRadius);
    program()->setUniformValue(gradientDirectionId_, gradientTable[data->backgroundFillMode]);
}
