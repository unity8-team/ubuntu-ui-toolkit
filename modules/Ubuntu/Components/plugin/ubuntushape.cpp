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

// TODO
//  - High priority:
//   - Add support for cutting sides (for aligned buttons).
//   - Add support inner shadows.
//   - Add support for custom shape mode.
//   - Make good use of precision modifiers.
//  - Low priority:
//   - Add support for pointers.
//   - Make the feature support in the shader more clear.
//   - Use http://www.khronos.org/registry/gles/extensions/NV/NV_texture_border_clamp.txt with
//     OpenGL ES 2 when available (need to patch QtDeclarative).

#include "ubuntushape.h"
#include "ucunits.h"
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <math.h>

// Retrieves the size of an array at compile time.
#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

const int kImplicitGridUnitWidth = 8.0f;
const int kImplicitGridUnitHeight = 8.0f;

// FIXME(loicm) Not sure if graphics drivers can discard sourceCoord varying interpolation through
//     static flow control. Profiling needs to be done to check if we might better switch to a
//     multiple shaders solution for texturing.
static const char* const kVertexShader =
    "uniform highp mat4 matrix;                                                                  \n"
    "uniform highp vec4 sourceFill[2];                                                           \n"
    "uniform int features[7];                                                                    \n"
    "attribute highp vec4 positionAttrib;                                                        \n"
    "attribute highp vec2 textureCoordAttrib;                                                    \n"
    "varying highp vec2 textureCoord;                                                            \n"
    "varying highp vec2 sourceCoord[2];                                                          \n"
    "void main()                                                                                 \n"
    "{                                                                                           \n"
    "    textureCoord = textureCoordAttrib;                                                      \n"
    "    if (features[0] != 0)                                                                   \n"
    "        sourceCoord[0] = textureCoordAttrib * sourceFill[0].xy + sourceFill[0].zw;          \n"
    "    if (features[2] != 0)                                                                   \n"
    "        sourceCoord[1] = textureCoordAttrib * sourceFill[1].xy + sourceFill[1].zw;          \n"
    "    gl_Position = matrix * positionAttrib;                                                  \n"
    "}";

static const char* const kFragmentShader =
    "uniform highp float qtOpacity;                                                              \n"
    "uniform highp vec4 bgColor[2];                                                              \n"
    "uniform highp int bgGradientIndex;                                                          \n"
    "uniform highp vec4 cornerTransform;                                                         \n"
    "uniform highp vec4 overlayColor;                                                            \n"
    "uniform highp vec4 overlaySteps;                                                            \n"
    "uniform highp vec4 shadowColor[2];                                                          \n"
    "uniform highp float radius;                                                                 \n"
    "uniform highp vec2 shadowSize;                                                              \n"
    "uniform highp vec2 shadowOffset[2];                                                         \n"
    "uniform highp vec2 sourceOpacity;                                                           \n"
    "uniform sampler2D sampler[2];                                                               \n"
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    "uniform highp float border[4];                                                              \n"
#endif
    "uniform int features[7];                                                                    \n"
    "varying highp vec2 textureCoord;                                                            \n"
    "varying highp vec2 sourceCoord[2];                                                          \n"
    "void main()                                                                                 \n"
    "{                                                                                           \n"
    //   Early texture fetches to cover latency. Since OpenGL ES 2 doesn't have support for border
    //   clamping in texture samplers, we have to do it in the fragment shader.
    "    highp vec4 texture[2];                                                                  \n"
    "    highp vec2 opacity = sourceOpacity;                                                     \n"
    "    for (int i = 0; i < 2; i++) {                                                           \n"
    "        if (features[i*2] != 0) {                                                           \n"
    "            texture[i] = texture2D(sampler[i], sourceCoord[i]);                             \n"
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    "            if (features[i*2+1] != 0) {                                                     \n"
    "                highp vec2 a = clamp(floor(abs(sourceCoord[0] * -2.0 + 1.0)), 0.0, 1.0);    \n"
    "                highp vec2 b = 1.0 - a;                                                     \n"
    "                opacity[i] *= (a.x * border[i*2] + b.x) * (a.y * border[i*2+1] + b.y);      \n"
    "            }                                                                               \n"
#endif
    "        } else {                                                                            \n"
    "            texture[i] = vec4(0.0);                                                         \n"
    "        }                                                                                   \n"
    "    }                                                                                       \n"

    //   Compute background.
    "    highp vec4 bg = mix(bgColor[0], bgColor[1], textureCoord[bgGradientIndex]);             \n"

    //   Compute overlay.
    "    highp vec4 overlay;                                                                     \n"
    "    if (features[4] != 0) {                                                                 \n"
    "        highp vec4 steps = step(overlaySteps, textureCoord.xyxy);                           \n"
    "        steps.xy = -steps.xy * steps.zw + steps.xy;                                         \n"
    "        overlay = vec4(steps.x * steps.y) * overlayColor;                                   \n"
    "    } else {                                                                                \n"
    "        overlay = vec4(0.0);                                                                \n"
    "    }                                                                                       \n"

    //   Blend components.
    "    texture[0] *= opacity[0];                                                               \n"
    "    texture[1] *= opacity[1];                                                               \n"
    "    highp vec4 shape = bg * (1.0 - texture[1].a) + texture[1];                              \n"
    "    shape = shape * (1.0 - texture[0].a) + texture[0];                                      \n"
    "    if (features[5] != 0) {                                                                 \n"
    "        shape = shape * (1.0 - overlay.a) + overlay;                                        \n"
    "    } else {                                                                                \n"
    "        shape = -shape * overlay + shape;                                                   \n"
    "    }                                                                                       \n"

    //   Shape components.
    "    highp vec2 coord = abs(textureCoord * 2.0 - 1.0);                                       \n"
    "    coord = max(coord * cornerTransform.xy + cornerTransform.zw, 0.0);                      \n"
    "    highp float shapeMask = clamp(-length(coord) * radius + radius + 0.5, 0.0, 1.0);        \n"
    "    shape = shape * shapeMask;                                                              \n"

    //   Compute shadow out. Shadow gradient is made non-linear to make it look better using
    //   "f(x) = 2x - x^2", it's composed with "g(x) = 1 - x" that's reverting gradient direction
    //   giving "g(f(x)) = x(x - 2) + 1". Shape is then masked out and blended over the shadow.
    //   FIXME(loicm) Shadow corner radius has one pixel less than shape corner radius which makes
    //       it look different at small shadow sizes.
    "    if (features[6] != 0) {                                                                 \n"
    "        highp float shadow;                                                                 \n"
    "        coord = abs((textureCoord + shadowOffset[1]) * 2.0 - 1.0);                          \n"
    "        coord = max(coord * cornerTransform.xy + cornerTransform.zw, 0.0);                  \n"
    "        shadow = clamp(-length(coord) * shadowSize[1] + shadowSize[1], 0.0, 1.0);           \n"
    "        shadow = shadow * (shadow - 2.0) + 1.0;                                             \n"
    "        shadow = clamp(-shadow * shapeMask + shadow, 0.0, 1.0);                             \n"
    "        shape = shadow * shadowColor[1] + shape;                                            \n"
    "    }                                                                                       \n"

    //   Store output color.
    "    gl_FragColor = shape * vec4(qtOpacity);                                                 \n"
    "}";

static const unsigned short kMeshIndices[] __attribute__((aligned(16))) = {
    0, 2, 1, 3
};

static const struct {
    const unsigned short* const indices;
    int indexCount;        // Number of indices.
    int vertexCount;       // Number of vertices.
    int attributeCount;    // Number of attributes.
    int stride;            // Offset in bytes from one vertex to the other.
    int positionCount;     // Number of components per position.
    int positionType;      // OpenGL type of the position components.
    int textureCoordCount; // Number of components per coordinate.
    int textureCoordType;  // OpenGL type of the coordinate components.
    int indexType;         // OpenGL type of the indices.
} kMesh = {
    kMeshIndices, ARRAY_SIZE(kMeshIndices),
    4, 3, sizeof(UbuntuShapeNode::Vertex), 2, GL_FLOAT, 2, GL_FLOAT, GL_UNSIGNED_SHORT
};

static const QSGGeometry::AttributeSet& getAttributes()
{
    static QSGGeometry::Attribute data[] = {
        QSGGeometry::Attribute::create(0, kMesh.positionCount, kMesh.positionType, true),
        QSGGeometry::Attribute::create(1, kMesh.textureCoordCount, kMesh.textureCoordType)
    };
    static QSGGeometry::AttributeSet attributes = {
        kMesh.attributeCount, kMesh.stride, data
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

// Gets the nearest boundary to coord in the texel grid of the given size.
static float roundTextureCoord(float coord, float size)
{
    return roundf(coord * size) / size;
}

// Gets an angle in radians.
static float degreesToRadians(float degrees)
{
    return (degrees / 180.0f) * M_PI;
}

MaterialData::MaterialData()
    : overlayColorPremultiplied(0.0f, 0.0f, 0.0f, 0.0f)
    , overlaySteps(0.0f, 0.0f, 0.0f, 0.0f)
    , cornerTransform(0.0f, 0.0f, 0.0f, 0.0f)
    , shadowSize(0.0f, 0.0f)
    , sourceOpacity(1.0f, 1.0f)
    , adjustedCornerRadius(0.0f)
    , backgroundGradientIndex(0)
    , hasOverlay(false)
    , hasOverlayBlendingSourceOver(true)
    , hasShadowOut(false)
{
    provider[0] = NULL;
    provider[1] = NULL;
    provider[2] = NULL;
    backgroundColorPremultiplied[0] = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
    backgroundColorPremultiplied[1] = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
    shadowColorPremultiplied[0] = QVector4D(0.0f, 0.0f, 0.0f, 0.75f);
    shadowColorPremultiplied[1] = QVector4D(0.75f, 0.75f, 0.75f, 0.75f);
    shadowOffset[0] = QVector2D(0.0f, 0.0f);
    shadowOffset[1] = QVector2D(0.0f, 0.0f);
    sourceFill[0] = QVector4D(1.0f, 1.0f, 0.0f, 0.0f);
    sourceFill[1] = QVector4D(1.0f, 1.0f, 0.0f, 0.0f);
    wrap[0][0] = QSGTexture::ClampToEdge;
    wrap[0][1] = QSGTexture::ClampToEdge;
    wrap[1][0] = QSGTexture::ClampToEdge;
    wrap[1][1] = QSGTexture::ClampToEdge;
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    hasSoftwareClampToBorder[0][0] = false;
    hasSoftwareClampToBorder[0][1] = false;
    hasSoftwareClampToBorder[1][0] = false;
    hasSoftwareClampToBorder[1][1] = false;
#endif
}

// --- QtQuick item ---

// FIXME(loicm) Add a schema describing the supported layers.
/*!
 * \qmltype UbuntuShape
 * \instantiates UbuntuShape
 * \inherits QQuickItem
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief Ubuntu UI toolkit's base graphical component.
 *
 * UbuntuShape is the base graphical component of the Ubuntu UI toolkit.
 *
 * UbuntuShape could be compared to a QML \c Image, on steroids. It allows to shape a set of layers
 * composed by an overlay, a primary source, a secondary source and a background. It also provides
 * support for casting shadows inside and outside of the shape. The default shape mode features
 * rounded corners but other shapes can be implemented with custom shapes. UbuntuShape's rendering
 * is entirely procedural, resolution independent, alias free and supports interactive framerates.
 *
 * UbuntuShape proposes an API that allows to drop a shadow around the item. That shadow is rendered
 * in the same batch than the shape but is entirely independent of the item size which means that
 * one can manipulate the item position and size without having to consider the shadow. That implies
 * one change compared to standard items, the clipping rectangle is bigger than the item and fits
 * both the shape and the shadow.
 *
 * Here is an UbuntuShape with a corner radius, a drop shadow and a centered Ubuntu logo rendered on
 * top of a grey background:
 *
 * \qml
 *     UbuntuShape {
 *         id: shape
 *         cornerRadius: 20.0
 *         shadowSizeOut: 50.0
 *         source: Image { source: "ubuntu_logo.png" }
 *         sourceHorizontalAlignment: UbuntuShape.AlignHCenter
 *         sourceVerticalAlignment: UbuntuShape.AlignVCenter
 *         backgroundColor: "grey"
 *     }
 * \endqml
 */

UbuntuShape::UbuntuShape(QQuickItem* parent)
    : QQuickItem(parent)
    , radius_(QString("small"))
    , geometry_()
    , overlayGeometry_(0.0f, 0.0, 0.0f, 0.0f)
    , overlayColor_(0.0f, 0.0f, 0.0f, 0.0f)
    , backgroundMode_(UbuntuShape::BackgroundColor)
    , overlayBlending_(UbuntuShape::SourceOver)
    , cornerRadius_(0.0f)
    , shadowBorder_(0.0f, 0.0f, 0.0f, 0.0f)
    , gridUnit_(UCUnits::instance().gridUnit())
    , materialData_()
{
    item_[0] = NULL;
    item_[1] = NULL;
    item_[2] = NULL;
    backgroundColor_[0] = QColor(0.0f, 0.0f, 0.0f, 0.0f);
    backgroundColor_[1] = QColor(0.0f, 0.0f, 0.0f, 0.0f);
    shadowColor_[0] = QColor(0.0f, 0.0f, 0.0f, 0.75f);
    shadowColor_[1] = QColor(1.0f, 1.0f, 1.0f, 0.75f);
    sourceTranslation_[0] = QVector2D(0.0f, 0.0f);
    sourceTranslation_[1] = QVector2D(0.0f, 0.0f);
    sourceScale_[0] = QVector2D(1.0f, 1.0f);
    sourceScale_[1] = QVector2D(1.0f, 1.0f);
    sourceFillMode_[0] = UbuntuShape::PreserveAspectCrop;
    sourceFillMode_[1] = UbuntuShape::PreserveAspectCrop;
    sourceWrapMode_[0][0] = UbuntuShape::ClampToEdge;
    sourceWrapMode_[0][1] = UbuntuShape::ClampToEdge;
    sourceWrapMode_[1][0] = UbuntuShape::ClampToEdge;
    sourceWrapMode_[1][1] = UbuntuShape::ClampToEdge;
    sourceHorizontalAlignment_[0] = UbuntuShape::AlignLeft;
    sourceHorizontalAlignment_[1] = UbuntuShape::AlignLeft;
    sourceVerticalAlignment_[0] = UbuntuShape::AlignTop;
    sourceVerticalAlignment_[1] = UbuntuShape::AlignTop;
    shadowSize_[0] = 0.0f;
    shadowSize_[1] = 0.0f;
    shadowDistance_[0] = 0.0f;
    shadowDistance_[1] = 0.0f;
    shadowAngle_[0] = 0.0f;
    shadowAngle_[1] = 0.0f;
    sourceOpacity_[0] = 1.0f;
    sourceOpacity_[1] = 1.0f;

    setFlag(ItemHasContents);
    QObject::connect(
        &UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(gridUnitChanged()));
    setImplicitWidth(kImplicitGridUnitWidth * gridUnit_);
    setImplicitHeight(kImplicitGridUnitHeight * gridUnit_);
}

UbuntuShape::~UbuntuShape()
{
    // FIXME(loicm) Should unparent the source items here?
}

/*!
 * \qmlproperty color Ubuntu.Components::UbuntuShape::color
 * \obsolete
 *
 * Use \l backgroundColor instead.
 */
void UbuntuShape::setColor(const QColor& color)
{
    qWarning() << "UbuntuShape: 'color' is deprecated, use 'backgroundColor' instead.";
    setBackgroundColor(color);
    Q_EMIT colorChanged();
}

/*!
 * \qmlproperty color Ubuntu.Components::UbuntuShape::gradientColor
 * \obsolete
 *
 * Use \l secondaryBackgroundColor instead.
 */
void UbuntuShape::setGradientColor(const QColor& gradientColor)
{
    qWarning() << "UbuntuShape: 'gradientColor' is deprecated, use 'secondaryBackgroundColor' "
        "instead.";
    setBackgroundMode(UbuntuShape::VerticalGradient);
    setSecondaryBackgroundColor(gradientColor);
    Q_EMIT gradientColorChanged();
}

/*!
 * \qmlproperty string Ubuntu.Components::UbuntuShape::radius
 * \obsolete
 *
 * Use \l cornerRadius instead.
 */
void UbuntuShape::setRadius(const QString& radius)
{
    qWarning() << "UbuntuShape: 'radius' is deprecated, use 'cornerRadius' instead.";
    if (radius == "small") {
        radius_ = "small";
        setCornerRadius(8.0f);  // FIXME(loicm) Default value to be validated.
        Q_EMIT radiusChanged();
    } else if (radius == "medium") {
        radius_ = "medium";
        setCornerRadius(16.0f);  // FIXME(loicm) Default value to be validated.
        Q_EMIT radiusChanged();
    }
}

/*!
 * \qmlproperty string Ubuntu.Components::UbuntuShape::borderSource
 * \obsolete
 *
 * Use \l customShape instead.
 */
void UbuntuShape::setBorderSource(const QString& borderSource)
{
    Q_UNUSED(borderSource);

    qWarning() << "UbuntuShape: 'borderSource' is deprecated, use 'customShape' instead.";
    Q_EMIT borderSourceChanged();
}

/*!
 * \qmlproperty variant Ubuntu.Components::UbuntuShape::image
 * \obsolete
 *
 * Use \l source instead.
 */
void UbuntuShape::setImage(QVariant image)
{
    qWarning() << "UbuntuShape: 'image' is deprecated, use 'source' instead.";
    setSource(image);
    Q_EMIT imageChanged();
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::shapeMode
 *
 * This property defines the rendering mode used by the UbuntuShape to render its corners. Default
 * value is \c Rounded.
 *
 * \list
 * \li UbuntuShape.Rounded - circle based corners
 * \li UbuntuShape.Custom - custom corners defined with \l customShape
 * \endlist
 */
void UbuntuShape::setShapeMode(UbuntuShape::ShapeMode shapeMode)
{
    if (shapeMode_ != shapeMode) {
        shapeMode_ = shapeMode;
        update();
        Q_EMIT shapeModeChanged();
    }
}

// FIXME(loicm) Document dedicated tool.
/*!
 * \qmlproperty variant Ubuntu.Components::UbuntuShape::customShape
 *
 * This property holds the custom shape used by the UbuntuShape to render its corners when
 * \l shapeMode is set to \c Custom. Default value is \c null.
 *
 * UbuntuShape implements that feature using
 * \l {http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf}
 * {distance fields}. This technique allows to efficiently render custom corners at different sizes
 * without losing quality. \c customShape is defined as an image generated with a dedicated tool.
 *
 * \qml
 *     UbuntuShape {
 *         shapeMode: UbuntuShape.Custom
 *         customShape: Image { source: "custom_shape.png" }
 *     }
 * \endqml
 */
void UbuntuShape::setCustomShape(QVariant customShape)
{
    QQuickItem* item = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(customShape));
    if (item_[2] != item) {
        item_[2] = item;
        if (item && !item->parentItem()) {
            // Inlined images need a parent and must not be visible.
            item->setParentItem(this);
            item->setVisible(false);
        }
        update();
        Q_EMIT customShapeChanged();
    }
}

/*!
 * \qmlproperty real Ubuntu.Components::UbuntuShape::cornerRadius
 *
 * This property defines the radius of the corners in pixels. Default value is \c 0.0.
 */
void UbuntuShape::setCornerRadius(float cornerRadius)
{
    cornerRadius = qMax(0.0f, cornerRadius);
    if (cornerRadius_ != cornerRadius) {
        // The adjusted corner radius is computed in updatePaintNode().
        cornerRadius_ = cornerRadius;
        update();
        Q_EMIT cornerRadiusChanged();
    }
}

/*!
 * \qmlproperty color Ubuntu.Components::UbuntuShape::backgroundColor
 * \qmlproperty color Ubuntu.Components::UbuntuShape::secondaryBackgroundColor
 *
 * These properties define the background colors of the UbuntuShape. \c secondaryBackgroundColor is
 * only used when \l backgroundMode is set to \c HorizontalGradient or \c VerticalGradient. Default
 * value is transparent black for both.
 */
void UbuntuShape::setBackgroundColor(const QColor& color)
{
    if (backgroundColor_[0] != color) {
        backgroundColor_[0] = color;
        const float alpha = color.alphaF();
        materialData_.backgroundColorPremultiplied[0] = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        if (backgroundMode_ == UbuntuShape::BackgroundColor) {
            materialData_.backgroundColorPremultiplied[1] =
                materialData_.backgroundColorPremultiplied[0];
        }
        update();
        Q_EMIT backgroundColorChanged();
    }
}

void UbuntuShape::setSecondaryBackgroundColor(const QColor& color)
{
    if (backgroundColor_[1] != color) {
        backgroundColor_[1] = color;
        if (backgroundMode_ != UbuntuShape::BackgroundColor) {
            const float alpha = color.alphaF();
            materialData_.backgroundColorPremultiplied[1] = QVector4D(
                color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        }
        update();
        Q_EMIT secondaryBackgroundColorChanged();
    }
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::backgroundMode
 *
 * This property defines the mode used by the UbuntuShape to render its background. Default value
 * is \c BackgroundColor.
 *
 * \list
 * \li UbuntuShape.BackgroundColor - background color is \l backgroundColor
 * \li UbuntuShape.HorizontalGradient - background color is an horizontal gradient from
 *     \l backgroundColor to \l secondaryBackgroundColor
 * \li UbuntuShape.VerticalGradient - background color is a vertical gradient from
 *     \l backgroundColor to \l secondaryBackgroundColor
 * \endlist
 */
void UbuntuShape::setBackgroundMode(BackgroundMode backgroundMode)
{
    if (backgroundMode_ != backgroundMode) {
        backgroundMode_ = backgroundMode;
        if (backgroundMode != UbuntuShape::BackgroundColor) {
            const float alpha = backgroundColor_[1].alphaF();
            materialData_.backgroundColorPremultiplied[1] = QVector4D(
                backgroundColor_[1].redF() * alpha, backgroundColor_[1].greenF() * alpha,
                backgroundColor_[1].blueF() * alpha, alpha);
        } else {
            materialData_.backgroundColorPremultiplied[1] =
                materialData_.backgroundColorPremultiplied[0];
        }
        const int table[3] = { 0, 0, 1 };
        materialData_.backgroundGradientIndex = table[static_cast<int>(backgroundMode)];
        update();
        Q_EMIT backgroundModeChanged();
    }
}

/*!
 * \qmlproperty rect Ubuntu.Components::UbuntuShape::overlayGeometry
 *
 * This property defines the rectangle geometry overlaying the UbuntuShape. Default value is an
 * empty rectangle.
 *
 * It is defined by a position and a size in normalized coordinates (in the range [0.0, 1.0]). An
 * overlay covering all the bottom part and starting from the middle of the UbuntuShape can be done
 * like this:
 *
 * \qml
 *     UbuntuShape {
 *         overlayGeometry: Qt.rect(0.0, 0.5, 1.0, 0.5)
 *     }
 * \endqml
 *
 * /note The area potentially exceeding the UbuntuShape is cropped.
 */
void UbuntuShape::setOverlayGeometry(const QRectF& overlayGeometry)
{
    QRectF rect(qMax(0.0, qMin(1.0, overlayGeometry.x())),
                qMax(0.0, qMin(1.0, overlayGeometry.y())),
                qMax(0.0, qMin(1.0, overlayGeometry.width())),
                qMax(0.0, qMin(1.0, overlayGeometry.height())));
    if (overlayGeometry_ != rect) {
        overlayGeometry_ = rect;
        if (rect.isValid()) {
            materialData_.overlaySteps = QVector4D(
                rect.x(), rect.y(), rect.x() + rect.width(), rect.height() + rect.y());
            materialData_.hasOverlay = true;
        } else {
            materialData_.hasOverlay = false;
        }
        update();
        Q_EMIT overlayGeometryChanged();
    }
}

/*!
 * \qmlproperty color Ubuntu.Components::UbuntuShape::overlayColor
 *
 * This property defines the color of the rectangle overlaying the UbuntuShape. The meaning of that
 * property depends on the \l overlayBlending that is defined. Default value is transparent black.
 */
void UbuntuShape::setOverlayColor(const QColor& color)
{
    // FIXME(loicm) Disable overlay when alpha is 0 in SourceOver mode and when all the components
    //     are 1 in Multiply mode.
    if (overlayColor_ != color) {
        overlayColor_ = color;
        if (overlayBlending_ == UbuntuShape::SourceOver) {
            const float alpha = color.alphaF();
            materialData_.overlayColorPremultiplied = QVector4D(
                color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        } else {
            // Color is inverted here to simplify the fragment shader.
            materialData_.overlayColorPremultiplied = QVector4D(
                1.0f - color.redF(), 1.0f - color.greenF(), 1.0f - color.blueF(),
                1.0f - color.alphaF());
        }
        update();
        Q_EMIT overlayColorChanged();
    }
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::overlayBlending
 *
 * This property defines the blending used to composite the rectangle overlaying the UbuntuShape.
 * Default value is \c SourceOver.
 *
 * \list
 * \li UbuntuShape.SourceOver - classical "source over" Porter/Duff blending
 * \li UbuntuShape.Multiply - multiply \l overlayColor with color values underlying the overlay,
 *     useful to mask out parts of the UbuntuShape
 * \endlist
 */
void UbuntuShape::setOverlayBlending(Blending overlayBlending)
{
    if (overlayBlending_ != overlayBlending) {
        overlayBlending_ = overlayBlending;
        if (overlayBlending_ == UbuntuShape::SourceOver) {
            const float alpha = overlayColor_.alphaF();
            materialData_.overlayColorPremultiplied = QVector4D(
                overlayColor_.redF() * alpha, overlayColor_.greenF() * alpha,
                overlayColor_.blueF() * alpha, alpha);
            materialData_.hasOverlayBlendingSourceOver = true;
        } else {
            // Color is inverted here to simplify the fragment shader.
            materialData_.overlayColorPremultiplied = QVector4D(
                1.0f - overlayColor_.redF(), 1.0f - overlayColor_.greenF(),
                1.0f - overlayColor_.blueF(), 1.0f - overlayColor_.alphaF());
            materialData_.hasOverlayBlendingSourceOver = false;
        }
        update();
        Q_EMIT overlayBlendingChanged();
    }
}

/*!
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowSizeIn
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowSizeOut
 *
 * These properties define the sizes of the shadows dropped inside and outside the UbuntuShape in
 * pixels. Default value is \c 0.0 for both.
 */
void UbuntuShape::setShadowSizeIn(float shadowSizeIn)
{
    shadowSizeIn = qMax(0.0f, shadowSizeIn);
    if (shadowSize_[0] != shadowSizeIn) {
        shadowSize_[0] = shadowSizeIn;
        update();
        Q_EMIT shadowSizeInChanged();
    }
}

/*!
 * \qmlproperty Item Ubuntu.Components::UbuntuShape::shadowColorIn
 * \qmlproperty Item Ubuntu.Components::UbuntuShape::shadowColorOut
 *
 * These properties define the colors of the shadows dropped inside and outside the UbuntuShape.
 * Default value is 75% translucent black for the shadow inside and 75% translucent white for the
 * shadow outside.
 */
void UbuntuShape::setShadowColorIn(const QColor& color)
{
    if (shadowColor_[0] != color) {
        shadowColor_[0] = color;
        const float alpha = color.alphaF();
        materialData_.shadowColorPremultiplied[0] = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        update();
        Q_EMIT shadowColorInChanged();
    }
}

/*!
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowDistanceIn
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowDistanceOut
 *
 * These properties define the distances in pixels used to drop the shadows inside and outdide the
 * UbuntuShape along the direction defined by \l shadowAngleIn and \l shadowAngleOut. Default
 * value is \c 0.0 for both.
 */
void UbuntuShape::setShadowDistanceIn(float shadowDistanceIn)
{
    shadowDistanceIn = qMax(0.0f, shadowDistanceIn);
    if (shadowDistance_[0] != shadowDistanceIn) {
        shadowDistance_[0] = shadowDistanceIn;
        update();
        Q_EMIT shadowDistanceInChanged();
    }
}

/*!
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowAngleIn
 * \qmlproperty real Ubuntu.Components::UbuntuShape::shadowAngleOut
 *
 * These properties define the angles in degrees used to drop the shadow inside and outside the
 * UbuntuShape. Default value is \c 0.0 for both.
 */
void UbuntuShape::setShadowAngleIn(float shadowAngleIn)
{
    shadowAngleIn = qMax(0.0f, qMin(360.0f, shadowAngleIn));
    if (shadowAngle_[0] != shadowAngleIn) {
        shadowAngle_[0] = shadowAngleIn;
        update();
        Q_EMIT shadowAngleInChanged();
    }
}

void UbuntuShape::updateShadowLayout()
{
    float sine, cosine;
    sincosf(degreesToRadians(shadowAngle_[1]), &sine, &cosine);
    const float offset[2] = { cosine * shadowDistance_[1], -sine * shadowDistance_[1] };
    materialData_.shadowOffset[1] = QVector2D(
        offset[0] / geometry_.width(), offset[1] / geometry_.height());
    shadowBorder_ = ShadowBorder(
        qMax(0.0f, shadowSize_[1] + offset[0]), qMax(0.0f, shadowSize_[1] - offset[0]),
        qMax(0.0f, shadowSize_[1] + offset[1]), qMax(0.0f, shadowSize_[1] - offset[1]));
#if defined(HAS_QTDECLARATIVE_UPDATECLIPRECT)
    updateClipRect();
#endif
}

void UbuntuShape::setShadowSizeOut(float shadowSizeOut)
{
    shadowSizeOut = qMax(0.0f, shadowSizeOut);
    if (shadowSize_[1] != shadowSizeOut) {
        shadowSize_[1] = shadowSizeOut;
        if ((shadowSizeOut > 0.0f) || (shadowDistance_[1] > 0.0f)) {
            materialData_.hasShadowOut = true;
        } else {
            materialData_.hasShadowOut = false;
        }
        updateShadowLayout();
        update();
        Q_EMIT shadowSizeOutChanged();
    }
}

void UbuntuShape::setShadowColorOut(const QColor& color)
{
    if (shadowColor_[1] != color) {
        shadowColor_[1] = color;
        const float alpha = color.alphaF();
        materialData_.shadowColorPremultiplied[1] = QVector4D(
            color.redF() * alpha, color.greenF() * alpha, color.blueF() * alpha, alpha);
        update();
        Q_EMIT shadowColorOutChanged();
    }
}

void UbuntuShape::setShadowDistanceOut(float shadowDistanceOut)
{
    shadowDistanceOut = qMax(0.0f, shadowDistanceOut);
    if (shadowDistance_[1] != shadowDistanceOut) {
        shadowDistance_[1] = shadowDistanceOut;
        if ((shadowDistanceOut > 0.0f) || (shadowSize_[1] > 0.0f)) {
            materialData_.hasShadowOut = true;
        } else {
            materialData_.hasShadowOut = false;
        }
        updateShadowLayout();
        update();
        Q_EMIT shadowDistanceOutChanged();
    }
}

void UbuntuShape::setShadowAngleOut(float shadowAngleOut)
{
    shadowAngleOut = qMax(0.0f, qMin(360.0f, shadowAngleOut));
    if (shadowAngle_[1] != shadowAngleOut) {
        shadowAngle_[1] = shadowAngleOut;
        updateShadowLayout();
        update();
        Q_EMIT shadowAngleOutChanged();
    }
}

/*!
 * \qmlproperty variant Ubuntu.Components::UbuntuShape::source
 * \qmlproperty variant Ubuntu.Components::UbuntuShape::secondarySource
 *
 * These properties hold the primary and secondary source \l Image or \l ShaderEffectSource used by
 * the UbuntuShape. Default value is \c null for both.
 *
 * In the case of an \l {Image}-based \c source, the fill modes and alignments set on the \l Image
 * are not monitored, use the corresponding properties of the UbuntuShape instead. The only property
 * that is monitored on both \l Image and \l ShaderEffectSource sources is \c smooth.
 *
 * \qml
 *     UbuntuShape {
 *         source: Image { source: "ubuntu.png"; smooth: true }
 *         secondarySource: Image { source: "background.png"; smooth: true }
 *     }
 * \endqml
 */
void UbuntuShape::setSource(QVariant source)
{
    QQuickItem* item = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(source));
    if (item_[0] != item) {
        item_[0] = item;
        if (item && !item->parentItem()) {
            // Inlined images need a parent and must not be visible.
            item->setParentItem(this);
            item->setVisible(false);
        }
        update();
        Q_EMIT sourceChanged();
    }
}

/*!
 * \qmlproperty real Ubuntu.Components::UbuntuShape::sourceOpacity
 * \qmlproperty real Ubuntu.Components::UbuntuShape::secondarySourceOpacity
 *
 * These properties define the opacities of the primary and secondary sources in the range
 * [0.0, 1.0]. Default value is \c 1.0 for both.
 */
void UbuntuShape::setSourceOpacity(float sourceOpacity)
{
    sourceOpacity = qMax(0.0f, qMin(1.0f, sourceOpacity));
    if (sourceOpacity_[0] != sourceOpacity) {
        sourceOpacity_[0] = sourceOpacity;
        materialData_.sourceOpacity.setX(sourceOpacity);
        update();
        Q_EMIT sourceOpacityChanged();
    }
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::sourceFillMode
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::secondarySourceFillMode
 *
 * These properties define the fill modes used by the UbuntuShape to render the primary and
 * secondary sources. Default value is \c PreserveAspectCrop for both.
 *
 * \list
 * \li UbuntuShape.Stretch - the source is scaled non-uniformly to fit
 * \li UbuntuShape.PreserveAspectFit - the source is scaled uniformly to fit without cropping
 * \li UbuntuShape.PreserveAspectCrop - the source is scaled uniformly to fit with cropping
 * \li UbuntuShape.Pad - the source is not scaled
 * \endlist
 */
void UbuntuShape::setSourceFillMode(UbuntuShape::FillMode sourceFillMode)
{
    if (sourceFillMode_[0] != sourceFillMode) {
        sourceFillMode_[0] = sourceFillMode;
        update();
        Q_EMIT sourceFillModeChanged();
    }
}

void UbuntuShape::updateWrapMode(int index, int direction, UbuntuShape::WrapMode wrapMode)
{
    Q_ASSERT(index < 2);
    Q_ASSERT(direction < 2);

    sourceWrapMode_[index][direction] = wrapMode;
#if !defined(QT_OPENGL_ES_2) && defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    const QSGTexture::WrapMode wrapTable[3] =
        { QSGTexture::Repeat, QSGTexture::ClampToEdge, QSGTexture::ClampToBorder };
#else
    // On OpenGL ES 2, fall back to edge clamping with software border clamping in the shader.
    const QSGTexture::WrapMode wrapTable[3] =
        { QSGTexture::Repeat, QSGTexture::ClampToEdge, QSGTexture::ClampToEdge };
    const bool softwareClampToBorderTable[3] = { false, false, true };
    materialData_.hasSoftwareClampToBorder[index][direction] =
        softwareClampToBorderTable[static_cast<int>(wrapMode)];
#endif
    materialData_.wrap[index][direction] = wrapTable[static_cast<int>(wrapMode)];
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::sourceHorizontalWrapMode
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::sourceVerticalWrapMode
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::secondarySourceHorizontalWrapMode
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::secondarySourceVerticalWrapMode
 *
 * These properties define the wrap modes used by the UbuntuShape to render the primary and
 * secondary sources outside of their horizontal and vertical areas. Default value is
 * \c ClampToEdge for the four of them.
 *
 * \list
 * \li UbuntuShape.Repeat - the source is duplicated
 * \li UbuntuShape.ClampToEdge - the source is clamped to the last pixel color
 * \li UbuntuShape.ClampToBorder - the source is clamped to transparent black
 * \endlist
 *
 * \note \c Some OpenGL ES 2 implementations do not support \c Repeat with non-power-of-two sized
 * sources. OpenGL ES 2 doesn't support \c ClampToBorder, it is implemented in a fragment shader for
 * the UbuntuShape and it might be a bit slower than the other clamping modes. It is recommended to
 * use \c ClampToEdge and sources with transparent borders in order to workaround that limitation.
 */
void UbuntuShape::setSourceHorizontalWrapMode(UbuntuShape::WrapMode sourceHorizontalWrapMode)
{
    if (sourceWrapMode_[0][0] != sourceHorizontalWrapMode) {
        updateWrapMode(0, 0, sourceHorizontalWrapMode);
        update();
        Q_EMIT sourceHorizontalWrapModeChanged();
    }
}

void UbuntuShape::setSourceVerticalWrapMode(UbuntuShape::WrapMode sourceVerticalWrapMode)
{
    if (sourceWrapMode_[0][1] != sourceVerticalWrapMode) {
        updateWrapMode(0, 1, sourceVerticalWrapMode);
        update();
        Q_EMIT sourceVerticalWrapModeChanged();
    }
}

/*!
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::sourceHorizontalAlignment
 * \qmlproperty enumeration Ubuntu.Components::UbuntuShape::secondarySourceHorizontalAlignment
 *
 * These properties define the horizontal alignments used by the UbuntuShape to render the primary
 * and secondary sources. Default value is \c AlignLeft for both.
 *
 * \list
 * \li UbuntuShape.AlignLeft - the source is aligned to the left
 * \li UbuntuShape.AlignHCenter - the source is aligned to the horizontal center
 * \li UbuntuShape.AlignRight - the source is aligned to the right
 * \endlist
 */
void UbuntuShape::setSourceHorizontalAlignment(UbuntuShape::HAlignment sourceHorizontalAlignment)
{
    if (sourceHorizontalAlignment_[0] != sourceHorizontalAlignment) {
        sourceHorizontalAlignment_[0] = sourceHorizontalAlignment;
        update();
        Q_EMIT sourceHorizontalAlignmentChanged();
    }
}

/*!
 * \qmlproperty HAlignment Ubuntu.Components::UbuntuShape::sourceVerticalAlignment
 * \qmlproperty HAlignment Ubuntu.Components::UbuntuShape::secondarySourceVerticalAlignment
 *
 * These properties define the vertical alignments used by the UbuntuShape to render the primary
 * and secondary sources. Default value is \c AlignTop for both.
 *
 * \list
 * \li UbuntuShape.AlignTop - the source is aligned to the top
 * \li UbuntuShape.AlignVCenter - the source is aligned to the vertical center
 * \li UbuntuShape.AlignBottom - the source is aligned to the bottom
 * \endlist
 */
void UbuntuShape::setSourceVerticalAlignment(UbuntuShape::VAlignment sourceVerticalAlignment)
{
    if (sourceVerticalAlignment_[0] != sourceVerticalAlignment) {
        sourceVerticalAlignment_[0] = sourceVerticalAlignment;
        update();
        Q_EMIT sourceVerticalAlignmentChanged();
    }
}

/*!
 * \qmlproperty vector2d Ubuntu.Components::UbuntuShape::sourceTranslation
 * \qmlproperty vector2d Ubuntu.Components::UbuntuShape::secondarySourceTranslation
 *
 * This property holds the 2D translation in pixels used by the UbuntuShape to render the primary
 * source. Default value is \c {(0.0, 0.0)} for both.
 *
 * It can be used to animate the source. Interesting animations can be achieved using \c Repeat wrap
 * modes on seamless images like infinite scrolling background, progress bar, etc.
 */
void UbuntuShape::setSourceTranslation(const QVector2D& sourceTranslation)
{
    if (sourceTranslation_[0] != sourceTranslation) {
        sourceTranslation_[0] = sourceTranslation;
        update();
        Q_EMIT sourceTranslationChanged();
    }
}

/*!
 * \qmlproperty vector2d Ubuntu.Components::UbuntuShape::sourceScale
 * \qmlproperty vector2d Ubuntu.Components::UbuntuShape::secondarySourceScale
 *
 * This property holds the 2D scaling used by the UbuntuShape to render the primary source. Default
 * value is \c {(1.0, 1.0)} for both.
 *
 * It can be used to flip the source horizontally and/or vertically with -1.0, to achieve pulse
 * effects, etc.
 */
void UbuntuShape::setSourceScale(const QVector2D& sourceScale)
{
    if (sourceScale_[0] != sourceScale) {
        sourceScale_[0] = sourceScale;
        update();
        Q_EMIT sourceScaleChanged();
    }
}

void UbuntuShape::setSecondarySource(QVariant secondarySource)
{
    QQuickItem* item = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(secondarySource));
    if (item_[1] != item) {
        item_[1] = item;
        if (item && !item->parentItem()) {
            // Inlined images need a parent and must not be visible.
            item->setParentItem(this);
            item->setVisible(false);
        }
        update();
        Q_EMIT secondarySourceChanged();
    }
}

void UbuntuShape::setSecondarySourceOpacity(float secondarySourceOpacity)
{
    secondarySourceOpacity = qMax(0.0f, qMin(1.0f, secondarySourceOpacity));
    if (sourceOpacity_[1] != secondarySourceOpacity) {
        sourceOpacity_[1] = secondarySourceOpacity;
        materialData_.sourceOpacity.setY(secondarySourceOpacity);
        update();
        Q_EMIT secondarySourceOpacityChanged();
    }
}

void UbuntuShape::setSecondarySourceFillMode(UbuntuShape::FillMode secondarySourceFillMode)
{
    if (sourceFillMode_[1] != secondarySourceFillMode) {
        sourceFillMode_[1] = secondarySourceFillMode;
        update();
        Q_EMIT secondarySourceFillModeChanged();
    }
}

void UbuntuShape::setSecondarySourceHorizontalWrapMode(
    UbuntuShape::WrapMode secondarySourceHorizontalWrapMode)
{
    if (sourceWrapMode_[1][0] != secondarySourceHorizontalWrapMode) {
        updateWrapMode(1, 0, secondarySourceHorizontalWrapMode);
        update();
        Q_EMIT secondarySourceHorizontalWrapModeChanged();
    }
}

void UbuntuShape::setSecondarySourceVerticalWrapMode(
    UbuntuShape::WrapMode secondarySourceVerticalWrapMode)
{
    if (sourceWrapMode_[1][1] != secondarySourceVerticalWrapMode) {
        updateWrapMode(1, 1, secondarySourceVerticalWrapMode);
        update();
        Q_EMIT secondarySourceVerticalWrapModeChanged();
    }
}

void UbuntuShape::setSecondarySourceHorizontalAlignment(
    UbuntuShape::HAlignment secondarySourceHorizontalAlignment)
{
    if (sourceHorizontalAlignment_[1] != secondarySourceHorizontalAlignment) {
        sourceHorizontalAlignment_[1] = secondarySourceHorizontalAlignment;
        update();
        Q_EMIT secondarySourceHorizontalAlignmentChanged();
    }
}

void UbuntuShape::setSecondarySourceVerticalAlignment(
    UbuntuShape::VAlignment secondarySourceVerticalAlignment)
{
    if (sourceVerticalAlignment_[1] != secondarySourceVerticalAlignment) {
        sourceVerticalAlignment_[1] = secondarySourceVerticalAlignment;
        update();
        Q_EMIT secondarySourceVerticalAlignmentChanged();
    }
}

void UbuntuShape::setSecondarySourceTranslation(const QVector2D& secondarySourceTranslation)
{
    if (sourceTranslation_[1] != secondarySourceTranslation) {
        sourceTranslation_[1] = secondarySourceTranslation;
        update();
        Q_EMIT secondarySourceTranslationChanged();
    }
}

void UbuntuShape::setSecondarySourceScale(const QVector2D& secondarySourceScale)
{
    if (sourceScale_[1] != secondarySourceScale) {
        sourceScale_[1] = secondarySourceScale;
        update();
        Q_EMIT secondarySourceScaleChanged();
    }
}

QRectF UbuntuShape::clipRect() const
{
    return QRectF(
        -shadowBorder_.left, -shadowBorder_.top,
        shadowBorder_.left + geometry_.width() + shadowBorder_.right,
        shadowBorder_.top + geometry_.height() + shadowBorder_.bottom);
}

void UbuntuShape::gridUnitChanged()
{
    gridUnit_ = UCUnits::instance().gridUnit();
    setImplicitWidth(kImplicitGridUnitWidth * gridUnit_);
    setImplicitHeight(kImplicitGridUnitHeight * gridUnit_);
    update();
}

void UbuntuShape::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    geometry_ = newGeometry;
    update();
}

void UbuntuShape::updateFillMode(int index, QSGTexture* texture)
{
    Q_ASSERT(index <= 2);
    Q_ASSERT(texture != NULL);

    const QSize textureSize = texture->textureSize();
    const FillMode fillMode = sourceFillMode_[index];

    float fillSx, fillSy;
    if (fillMode == PreserveAspectFit) {
        const float textureRatio = static_cast<float>(textureSize.width()) / textureSize.height();
        const float itemRatio = geometry_.width() / geometry_.height();
        fillSx = (textureRatio < itemRatio) ? (itemRatio / textureRatio) : 1.0f;
        fillSy = (textureRatio < itemRatio) ? 1.0f : (textureRatio / itemRatio);
    } else if (fillMode == PreserveAspectCrop) {
        const float textureRatio = static_cast<float>(textureSize.width()) / textureSize.height();
        const float itemRatio = geometry_.width() / geometry_.height();
        fillSx = (textureRatio < itemRatio) ? 1.0f : (itemRatio / textureRatio);
        fillSy = (textureRatio < itemRatio) ? (textureRatio / itemRatio) : 1.0f;
    } else if (fillMode == Pad) {
        fillSx = geometry_.width() / textureSize.width();
        fillSy = geometry_.height() / textureSize.height();
    } else {
        fillSx = 1.0f;
        fillSy = 1.0f;
    }

    const float sourceSxInv = 1.0f / sourceScale_[index].x();
    const float sourceSyInv = 1.0f / sourceScale_[index].y();
    const float sourceTx = (sourceTranslation_[index].x() * sourceSxInv) / textureSize.width();
    const float sourceTy = (sourceTranslation_[index].y() * sourceSyInv) / textureSize.height();
    const float sx = fillSx * sourceSxInv;
    const float sy = fillSy * sourceSyInv;
    const float factorTable[3] = { 0.0f, 0.5f, 1.0f };
    const float hFactor = factorTable[static_cast<int>(sourceHorizontalAlignment_[index])];
    const float vFactor = factorTable[static_cast<int>(sourceVerticalAlignment_[index])];
    materialData_.sourceFill[index] = QVector4D(
        sx, sy, roundTextureCoord(hFactor * (1.0f - sx) - sourceTx, textureSize.width()),
        roundTextureCoord(vFactor * (1.0f - sy) - sourceTy, textureSize.height()));
}

QSGNode* UbuntuShape::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    if (geometry_.width() <= 0.0f || geometry_.height() <= 0.0f) {
        delete oldNode;
        return NULL;
    }

    // Get textures and listen for updates.
    QSGTexture* texture[3];
    for (int i = 0; i < 3; i++) {
        QSGTextureProvider* provider = item_[i] ? item_[i]->textureProvider() : NULL;
        texture[i] = provider ? provider->texture() : NULL;
        if (provider != materialData_.provider[i]) {
            if (materialData_.provider[i] != NULL)
                QObject::disconnect(
                    materialData_.provider[i], SIGNAL(textureChanged()), this, SLOT(update()));
            if (provider != NULL)
                QObject::connect(provider, SIGNAL(textureChanged()), this, SLOT(update()));
            materialData_.provider[i] = provider;
        }
    }

    // Store the source and secondary source texture coordinate scale and translate transformations.
    for (int i = 0; i < 2; i++) {
        if (texture[i]) {
            updateFillMode(i, texture[i]);
        }
    }

    // Adjust corner radius to item size. The corner radius first needs to be clamped to 0.5 in
    // order to prevent rendering issues.
    const float cornerRadius = cornerRadius_ >= 0.5f ? cornerRadius_ : 0.5f;
    const float minimalSide = qMin(geometry_.width(), geometry_.height());
    const float overlapSize = qMax(0.0f, cornerRadius - (0.5f * minimalSide));
    materialData_.adjustedCornerRadius = cornerRadius - overlapSize;

    // Store the corner scale and translate transformation.
    const float sx = (geometry_.width() * 0.5f) / materialData_.adjustedCornerRadius;
    const float sy = (geometry_.height() * 0.5f) / materialData_.adjustedCornerRadius;
    materialData_.cornerTransform = QVector4D(sx, sy, 1.0f - sx, 1.0f - sy);

    // Store shadow sizes.
    const float shadowSizeOut = shadowSize_[1] > 1.0f ? shadowSize_[1] : 1.0f;
    materialData_.shadowSize = QVector2D(0.0f, -materialData_.adjustedCornerRadius / shadowSizeOut);

    // Update node.
    UbuntuShapeNode* node = oldNode ?
        static_cast<UbuntuShapeNode*>(oldNode) : new UbuntuShapeNode(this);
    node->material()->setData(&materialData_);
    node->setVertices(geometry_, shadowBorder_);

    return node;
}

// --- Scene graph geometry node ---

UbuntuShapeNode::UbuntuShapeNode(UbuntuShape* item)
    : QSGGeometryNode()
    , item_(item)
    , geometry_(getAttributes(), kMesh.vertexCount, kMesh.indexCount, kMesh.indexType)
    , material_()
{
    memcpy(geometry_.indexData(), kMesh.indices, kMesh.indexCount * sizeOfType(kMesh.indexType));
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    setGeometry(&geometry_);
    setMaterial(&material_);
    setFlag(UsePreprocess, false);
}

void UbuntuShapeNode::setVertices(const QRectF& geometry, const ShadowBorder& shadowBorder)
{
    // 0-----1
    // |   / |
    // | /   |
    // 2-----3
    UbuntuShapeNode::Vertex* vertices =
        reinterpret_cast<UbuntuShapeNode::Vertex*>(geometry_.vertexData());
    vertices[0].position[0] = -shadowBorder.left;
    vertices[0].position[1] = -shadowBorder.top;
    vertices[0].textureCoord[0] = -shadowBorder.left / geometry.width();
    vertices[0].textureCoord[1] = -shadowBorder.top / geometry.height();
    vertices[1].position[0] = geometry.width() + shadowBorder.right;
    vertices[1].position[1] = -shadowBorder.top;
    vertices[1].textureCoord[0] = 1.0f + (shadowBorder.right / geometry.width());
    vertices[1].textureCoord[1] = - shadowBorder.top / geometry.height();
    vertices[2].position[0] = -shadowBorder.left;
    vertices[2].position[1] = geometry.height() + shadowBorder.bottom;
    vertices[2].textureCoord[0] = -shadowBorder.left / geometry.width();
    vertices[2].textureCoord[1] = 1.0f + (shadowBorder.bottom / geometry.height());
    vertices[3].position[0] = geometry.width() + shadowBorder.right;
    vertices[3].position[1] = geometry.height() + shadowBorder.bottom;
    vertices[3].textureCoord[0] = 1.0f + (shadowBorder.right / geometry.width());
    vertices[3].textureCoord[1] = 1.0f + (shadowBorder.bottom / geometry.height());
    markDirty(DirtyGeometry);
}

// --- Scene graph material ---

UbuntuShapeMaterial::UbuntuShapeMaterial()
    : data_()
{
    setFlag(Blending);
}

QSGMaterialType* UbuntuShapeMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UbuntuShapeMaterial::createShader() const
{
    return new UbuntuShapeShader();
}

void UbuntuShapeMaterial::setData(MaterialData* data)
{
    memcpy(&data_, data, sizeof(MaterialData));
}

// -- Scene graph material shader ---

const char *UbuntuShapeShader::vertexShader() const
{
    return kVertexShader;
}

const char* UbuntuShapeShader::fragmentShader() const
{
    return kFragmentShader;
}

char const* const* UbuntuShapeShader::attributeNames() const
{
    static char const* const attributes[] = { "positionAttrib", "textureCoordAttrib", 0 };
    return attributes;
}

void UbuntuShapeShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    openglFunctions_ = QOpenGLContext::currentContext()->functions();
    const int sources[2] = { 0, 1 };
    program()->setUniformValueArray("sampler", sources, 2);
    matrixId_ = program()->uniformLocation("matrix");
    qtOpacityId_ = program()->uniformLocation("qtOpacity");
    bgColorId_ = program()->uniformLocation("bgColor");
    overlayColorId_ = program()->uniformLocation("overlayColor");
    overlayStepsId_ = program()->uniformLocation("overlaySteps");
    shadowColorId_ = program()->uniformLocation("shadowColor");
    radiusId_ = program()->uniformLocation("radius");
    cornerTransformId_ = program()->uniformLocation("cornerTransform");
    shadowSizeId_ = program()->uniformLocation("shadowSize");
    shadowOffsetId_ = program()->uniformLocation("shadowOffset");
    bgGradientIndexId_ = program()->uniformLocation("bgGradientIndex");
    sourceOpacityId_ = program()->uniformLocation("sourceOpacity");
    sourceFillId_ = program()->uniformLocation("sourceFill");
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    borderId_ = program()->uniformLocation("border");
#endif
    featuresId_ = program()->uniformLocation("features");
}

// FIXME(loicm) Minimize uniform updates by checking against previous values.
// FIXME(loicm) Clean up.
void UbuntuShapeShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    MaterialData* data = static_cast<UbuntuShapeMaterial*>(newEffect)->data();
    int features[7];
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    float border[4];
#endif

    for (int i = 0; i < 2; i++) {
        QSGTextureProvider* provider = data->provider[i];
        QSGTexture* texture = provider ? provider->texture() : NULL;
        openglFunctions_->glActiveTexture(GL_TEXTURE0 + i);
        if (texture) {
            if (texture->horizontalWrapMode() != data->wrap[i][0])
                texture->setHorizontalWrapMode(data->wrap[i][0]);
            if (texture->verticalWrapMode() != data->wrap[i][1])
                texture->setVerticalWrapMode(data->wrap[i][1]);
            texture->bind();
            features[i*2] = 1;
#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
            features[i*2+1] |= data->hasSoftwareClampToBorder[i][0] ? 1 : 0;
            features[i*2+1] |= data->hasSoftwareClampToBorder[i][1] ? 1 : 0;
            border[i*2] = data->hasSoftwareClampToBorder[i][0] ? 0.0f : 1.0f;
            border[i*2+1] = data->hasSoftwareClampToBorder[i][1] ? 0.0f : 1.0f;
#endif
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
            features[i*2] = 0;
        }
    }

    if (state.isMatrixDirty()) {
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(qtOpacityId_, state.opacity());
    }

    features[4] = static_cast<int>(data->hasOverlay);
    features[5] = static_cast<int>(data->hasOverlayBlendingSourceOver);
    features[6] = static_cast<int>(data->hasShadowOut);
    program()->setUniformValueArray(featuresId_, features, 7);

#if defined(QT_OPENGL_ES_2) || !defined(HAS_QTDECLARATIVE_CLAMPTOBORDER)
    program()->setUniformValueArray(borderId_, &border[0], 4, 1);
#endif
    program()->setUniformValueArray(bgColorId_, data->backgroundColorPremultiplied, 2);
    program()->setUniformValue(overlayColorId_, data->overlayColorPremultiplied);
    program()->setUniformValue(overlayStepsId_, data->overlaySteps);
    program()->setUniformValueArray(shadowColorId_, data->shadowColorPremultiplied, 2);
    program()->setUniformValue(radiusId_, data->adjustedCornerRadius);
    program()->setUniformValue(shadowSizeId_, data->shadowSize);
    program()->setUniformValueArray(shadowOffsetId_, data->shadowOffset, 2);
    program()->setUniformValue(cornerTransformId_, data->cornerTransform);
    program()->setUniformValue(bgGradientIndexId_, data->backgroundGradientIndex);
    program()->setUniformValue(sourceOpacityId_, data->sourceOpacity);
    program()->setUniformValueArray(sourceFillId_, data->sourceFill, 2);
}
