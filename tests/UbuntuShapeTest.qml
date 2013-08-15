import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: root
    width: 900
    height: 600
    focus: true

    // Enum to string tables.
    property variant backgroundModeTable: [
        "BackgroundColor", "HorizontalGradient", "VerticalGradient"
    ]
    property variant blendingTable: [
        "SourceOver", "Multiply"
    ]
    property variant hAlignmentTable: [
        "AlignLeft", "AlignHCenter", "AlignRight"
    ]
    property variant vAlignmentTable: [
        "AlignTop", "AlignVCenter", "AlignBottom"
    ]
    property variant fillModeTable: [
        "Stretch", "PreserveAspectFit", "PreserveAspectCrop", "Pad"
    ]
    property variant wrapModeTable: [
        "Repeat", "ClampToEdge", "ClampToBorder"
    ]

    // Zoom properties.
    property variant translation: Qt.point(0.0, 0.0)
    property real scaleBase: 1.1
    property real scaleExponent: 0.0
    property real minScaleExponent: 0.0
    property real maxScaleExponent:  48.317715856  // Logarithm of 100 to base 1.1 (scaleBase).
    property real scaleFactor: 1.0
    property real scale: 1.0

    // Shape properties.
    property Image image: Image { source: "ubuntu.jpg" }
    property Image secondaryImage: Image { source: "texture.jpg" }

    // Overlay text properties.
    property int optionPage: 1
    property string textOverlayString1:
        "Zoom            (scroll):   x " + root.scaleFactor.toFixed(1) + "\n\n" +
        "Option page          (a):   " + root.optionPage + "\n\n" +
        "Corner radius        (z):   " + shape.cornerRadius + "\n\n" +
        "Background colors    (e/r): " + shape.backgroundColor + ", " + shape.secondaryBackgroundColor + "\n" +
        "Background mode      (t):   " + root.backgroundModeTable[shape.backgroundMode] + "\n\n" +
        "Overlay color        (y):   " + shape.overlayColor + "\n" +
        "Overlay geometry (u/i/o/p): " + shape.overlayGeometry.x.toFixed(2) + ", " + shape.overlayGeometry.y.toFixed(2) + ", " + shape.overlayGeometry.width.toFixed(2) + ", " + shape.overlayGeometry.height.toFixed(2) + "\n" +
        "Overlay blending     (q):   " + root.blendingTable[shape.overlayBlending] + "\n\n" +
        "Shadow color in      (s):   " + shape.shadowColorIn + "\n" +
        "Shadow size in       (d):   " + shape.shadowSizeIn.toFixed(1) + "\n" +
        "Shadow angle in      (f):   " + shape.shadowAngleIn.toFixed(1) + "°\n" +
        "Shadow distance in   (g):   " + shape.shadowDistanceIn.toFixed(1) + "\n\n" +
        "Shadow color out     (h):   " + shape.shadowColorOut + "\n" +
        "Shadow size out      (j):   " + shape.shadowSizeOut.toFixed(1) + "\n" +
        "Shadow angle out     (k):   " + shape.shadowAngleOut.toFixed(1) + "°\n" +
        "Shadow distance out  (l):   " + shape.shadowDistanceOut.toFixed(1)
    property string textOverlayString2:
        "Zoom            (scroll):   x " + root.scaleFactor.toFixed(1) + "\n\n" +
        "Option page          (a):   " + root.optionPage + "\n\n" +
        "Source 1             (z):   " + shape.source + "\n" +
        "Source 1 opacity     (e):   " + shape.sourceOpacity.toFixed(2) + "\n" +
        "Source 1 fill        (r):   " + root.fillModeTable[shape.sourceFillMode] + "\n" +
        "Source 1 hwrap       (t):   " + root.wrapModeTable[shape.sourceHorizontalWrapMode] + "\n" +
        "Source 1 vwrap       (y):   " + root.wrapModeTable[shape.sourceVerticalWrapMode] + "\n" +
        "Source 1 halign      (u):   " + root.hAlignmentTable[shape.sourceHorizontalAlignment] + "\n" +
        "Source 1 valign      (i):   " + root.vAlignmentTable[shape.sourceVerticalAlignment] + "\n" +
        "Source 1 translation (o/p): " + shape.sourceTranslation.x.toFixed(2) + ", " + shape.sourceTranslation.y.toFixed(2) + "\n" +
        "Source 1 scale       (q/s): " + shape.sourceScale.x.toFixed(2) + ", " + shape.sourceScale.y.toFixed(2) + "\n\n" +
        "Source 2             (d):   " + shape.secondarySource + "\n" +
        "Source 2 opacity     (f):   " + shape.secondarySourceOpacity.toFixed(2) + "\n" +
        "Source 2 fill        (g):   " + root.fillModeTable[shape.secondarySourceFillMode] + "\n" +
        "Source 2 hwrap       (h):   " + root.wrapModeTable[shape.secondarySourceHorizontalWrapMode] + "\n" +
        "Source 2 vwrap       (j):   " + root.wrapModeTable[shape.secondarySourceVerticalWrapMode] + "\n" +
        "Source 2 halign      (k):   " + root.hAlignmentTable[shape.secondarySourceHorizontalAlignment] + "\n" +
        "Source 2 valign      (l):   " + root.vAlignmentTable[shape.secondarySourceVerticalAlignment] + "\n" +
        "Source 2 translation (m/w): " + shape.secondarySourceTranslation.x.toFixed(2) + ", " + shape.secondarySourceTranslation.y.toFixed(2) + "\n" +
        "Source 2 scale       (x/c): " + shape.secondarySourceScale.x.toFixed(2) + ", " + shape.secondarySourceScale.y.toFixed(2)

    // Main scene.
    Item {
        id: scene
        anchors.fill: parent
        Rectangle {
            id: background
            anchors.fill: parent
            color: "#7f7f7f"
        }

        UbuntuShape {
            id: shape
            anchors.fill: parent
            anchors.leftMargin: 400
            anchors.rightMargin: 200
            anchors.topMargin: 200
            anchors.bottomMargin: 200
            cornerRadius: 20.0
            backgroundColor: Qt.rgba(0.6, 0.6, 1.0, 1.0)
            secondaryBackgroundColor: Qt.rgba(0.3, 0.3, 0.5, 1.0)
            shadowSizeOut: 50.0
            source: root.image
            secondarySource: root.secondaryImage
            secondarySourceFillMode: UbuntuShape.Pad
            secondarySourceHorizontalWrapMode: UbuntuShape.Repeat
            secondarySourceVerticalWrapMode: UbuntuShape.Repeat
        }
    }

    // Zoom support.
    ShaderEffectSource {
        id: shaderEffectSource
        anchors.fill: scene
        sourceItem: scene
        hideSource: true
        visible: false
        smooth: false
    }
    ShaderEffect {
        anchors.fill: scene
        property variant tex: shaderEffectSource
        property variant translation: root.translation
        property real scale: root.scale
        vertexShader: "
            uniform mat4 qt_Matrix;
            uniform float scale;
            uniform vec2 translation;
            attribute vec4 qt_Vertex;
            attribute vec2 qt_MultiTexCoord0;
            varying vec2 texCoord;
            void main() {
                texCoord = vec2(scale) * qt_MultiTexCoord0 + translation;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            uniform sampler2D tex;
            uniform float qt_Opacity;
            varying vec2 texCoord;
            void main() {
                gl_FragColor = texture2D(tex, texCoord) * qt_Opacity;
            }"
    }

    // Text overlay.
    Text {
        id: textOverlay
        width:200
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 14
        font.weight: Font.Bold
        //style: Text.Outline
        //styleColor: "black"
        color: "black"
        text: optionPage == 1 ? textOverlayString1 : textOverlayString2
    }

    // Mouse handling.
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true

        property real lastX: 0.0
        property real lastY: 0.0

        onPressed: {
            if (pressedButtons & Qt.LeftButton) {
                lastX = mouseX;
                lastY = mouseY;
            }
        }
        onPositionChanged: {
            if (pressedButtons & Qt.LeftButton) {
                var tx = root.translation.x;
                var ty = root.translation.y;
                var sx = root.scale / root.width;
                var sy = root.scale / root.height;
                var x = mouseX - lastX;
                var y = mouseY - lastY;
                root.translation = Qt.point(Math.max(0.0, Math.min(1.0 - root.scale, tx - sx * x)),
                                            Math.max(0.0, Math.min(1.0 - root.scale, ty - sy * y)));
                lastX = mouseX;
                lastY = mouseY;
            }
        }
        onWheel: {
            root.scaleExponent = Math.max(minScaleExponent, Math.min(maxScaleExponent,
                root.scaleExponent + (wheel.angleDelta.y < 0.0 ? -1.0 : 1.0)));
            root.scaleFactor = Math.pow(root.scaleBase, root.scaleExponent);
            var oldScale = root.scale;
            root.scale = 1.0 / root.scaleFactor;
            var s = oldScale - root.scale;
            var tx = root.translation.x;
            var ty = root.translation.y;
            var x = mouseX / root.width;
            var y = mouseY / root.height;
            root.translation = Qt.point(Math.max(0.0, Math.min(1.0 - root.scale, tx + s * x)),
                                        Math.max(0.0, Math.min(1.0 - root.scale, ty + s * y)));
        }
    }

    // Keyboard handling.
    Keys.onPressed: {
        var shift = Qt.ShiftModifier;

        if (event.key == Qt.Key_A) {
            root.optionPage = (root.optionPage == 1) ? 2 : 1;

        } else {
            if (root.optionPage == 1) {
                // Corner.
                if (event.key == Qt.Key_Z) {
                    shape.cornerRadius = Math.max(0.0, Math.min(
                    500.0, shape.cornerRadius + ((event.modifiers & shift) ? 1.0 : -1.0)));

                // Background.
                } else if (event.key == Qt.Key_E) {
                    shape.backgroundColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
                } else if (event.key == Qt.Key_R) {
                    shape.secondaryBackgroundColor =
                    Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
                } else if (event.key == Qt.Key_T) {
                    shape.backgroundMode = (shape.backgroundMode + 1) % 3;

                // Overlay.
                } else if (event.key == Qt.Key_Y) {
                    shape.overlayColor = Qt.rgba(Math.random(), Math.random(), Math.random(), Math.random());
                } else if (event.key == Qt.Key_U) {
                    var x = Math.max(0.0, Math.min(1.0,
                    shape.overlayGeometry.x + ((event.modifiers & shift) ? 0.01 : -0.01)));
                    shape.overlayGeometry = Qt.rect(
                        x, shape.overlayGeometry.y, shape.overlayGeometry.width, shape.overlayGeometry.height);
                } else if (event.key == Qt.Key_I) {
                    var y = Math.max(0.0, Math.min(1.0,
                    shape.overlayGeometry.y + ((event.modifiers & shift) ? 0.01 : -0.01)));
                    shape.overlayGeometry = Qt.rect(
                        shape.overlayGeometry.x, y, shape.overlayGeometry.width, shape.overlayGeometry.height);
                } else if (event.key == Qt.Key_O) {
                    var width = Math.max(0.0, Math.min(1.0,
                    shape.overlayGeometry.width + ((event.modifiers & shift) ? 0.01 : -0.01)));
                    shape.overlayGeometry = Qt.rect(
                        shape.overlayGeometry.x, shape.overlayGeometry.y, width, shape.overlayGeometry.height);
                } else if (event.key == Qt.Key_P) {
                    var height = Math.max(0.0, Math.min(1.0,
                    shape.overlayGeometry.height + ((event.modifiers & shift) ? 0.01 : -0.01)));
                    shape.overlayGeometry = Qt.rect(
                        shape.overlayGeometry.x, shape.overlayGeometry.y, shape.overlayGeometry.width, height);
                } else if (event.key == Qt.Key_Q) {
                    shape.overlayBlending = (shape.overlayBlending + 1) % 2;

                // Shadows.
                } else if (event.key == Qt.Key_S) {
                    shape.shadowColorIn = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.9);
                } else if (event.key == Qt.Key_D) {
                    shape.shadowSizeIn = Math.max(0.0, Math.min(400.0,
                        shape.shadowSizeIn + ((event.modifiers & shift) ? 1.0 : -1.0)));
                } else if (event.key == Qt.Key_F) {
                    if (!(event.modifiers & Qt.ShiftModifier)) {
                        shape.shadowAngleIn = (shape.shadowAngleIn > 0.1) ?
                            (shape.shadowAngleIn - 2.5) : 360.0 - 2.5;
                    } else {
                        shape.shadowAngleIn = (shape.shadowAngleIn < 359.9) ?
                            (shape.shadowAngleIn + 2.5) : 2.5;
                    }
                } else if (event.key == Qt.Key_G) {
                    shape.shadowDistanceIn = Math.max(0.0, Math.min(200.0,
                        shape.shadowDistanceIn + ((event.modifiers & shift) ? 1.0 : -1.0)));
                } else if (event.key == Qt.Key_H) {
                    shape.shadowColorOut = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.9);
                } else if (event.key == Qt.Key_J) {
                    shape.shadowSizeOut = Math.max(0.0, Math.min(400.0,
                        shape.shadowSizeOut + ((event.modifiers & shift) ? 1.0 : -1.0)));
                } else if (event.key == Qt.Key_K) {
                    if (!(event.modifiers & Qt.ShiftModifier)) {
                        shape.shadowAngleOut = (shape.shadowAngleOut > 0.1) ?
                            (shape.shadowAngleOut - 2.5) : 360.0 - 2.5;
                    } else {
                        shape.shadowAngleOut = (shape.shadowAngleOut < 359.9) ?
                            (shape.shadowAngleOut + 2.5) : 2.5;
                    }
                } else if (event.key == Qt.Key_L) {
                    shape.shadowDistanceOut = Math.max(0.0, Math.min(200.0,
                        shape.shadowDistanceOut + ((event.modifiers & shift) ? 1.0 : -1.0)));
                }

            } else if (optionPage == 2) {
                // Primary source.
                if (event.key == Qt.Key_Z) {
                    if (shape.source == null) {
                        shape.source = root.image;
                    } else {
                        shape.source = null;
                    }
                } else if (event.key == Qt.Key_E) {
                    shape.sourceOpacity = Math.max(0.0, Math.min(1.0,
                    shape.sourceOpacity + ((event.modifiers & shift) ? 0.01 : -0.01)));
                } else if (event.key == Qt.Key_R) {
                    shape.sourceFillMode = (shape.sourceFillMode + 1) % 4;
                } else if (event.key == Qt.Key_T) {
                    shape.sourceHorizontalWrapMode = (shape.sourceHorizontalWrapMode + 1) % 3;
                } else if (event.key == Qt.Key_Y) {
                    shape.sourceVerticalWrapMode = (shape.sourceVerticalWrapMode + 1) % 3;
                } else if (event.key == Qt.Key_U) {
                    shape.sourceHorizontalAlignment = (shape.sourceHorizontalAlignment + 1) % 3;
                } else if (event.key == Qt.Key_I) {
                    shape.sourceVerticalAlignment = (shape.sourceVerticalAlignment + 1) % 3;
                } else if (event.key == Qt.Key_O) {
                    shape.sourceTranslation = Qt.vector2d(
                        shape.sourceTranslation.x + ((event.modifiers & shift) ? 1.0 : -1.0), shape.sourceTranslation.y);
                } else if (event.key == Qt.Key_P) {
                    shape.sourceTranslation = Qt.vector2d(
                        shape.sourceTranslation.x, shape.sourceTranslation.y + ((event.modifiers & shift) ? 1.0 : -1.0));
                } else if (event.key == Qt.Key_Q) {
                    shape.sourceScale = Qt.vector2d(
                        shape.sourceScale.x + ((event.modifiers & shift) ? 0.05 : -0.05), shape.sourceScale.y);
                } else if (event.key == Qt.Key_S) {
                    shape.sourceScale = Qt.vector2d(
                        shape.sourceScale.x, shape.sourceScale.y + ((event.modifiers & shift) ? 0.05 : -0.05));

                // Secondary source.
                } else if (event.key == Qt.Key_D) {
                    if (shape.secondarySource == null) {
                        shape.secondarySource = root.secondaryImage;
                    } else {
                        shape.secondarySource = null;
                    }
                } else if (event.key == Qt.Key_F) {
                    shape.secondarySourceOpacity = Math.max(0.0, Math.min(1.0,
                    shape.secondarySourceOpacity + ((event.modifiers & shift) ? 0.01 : -0.01)));
                } else if (event.key == Qt.Key_G) {
                    shape.secondarySourceFillMode = (shape.secondarySourceFillMode + 1) % 4;
                } else if (event.key == Qt.Key_H) {
                    shape.secondarySourceHorizontalWrapMode = (shape.secondarySourceHorizontalWrapMode + 1) % 3;
                } else if (event.key == Qt.Key_J) {
                    shape.secondarySourceVerticalWrapMode = (shape.secondarySourceVerticalWrapMode + 1) % 3;
                } else if (event.key == Qt.Key_K) {
                    shape.secondarySourceHorizontalAlignment = (shape.secondarySourceHorizontalAlignment + 1) % 3;
                } else if (event.key == Qt.Key_L) {
                    shape.secondarySourceVerticalAlignment = (shape.secondarySourceVerticalAlignment + 1) % 3;
                } else if (event.key == Qt.Key_M) {
                    shape.secondarySourceTranslation = Qt.vector2d(
                        shape.secondarySourceTranslation.x + ((event.modifiers & shift) ? 1.0 : -1.0), shape.secondarySourceTranslation.y);
                } else if (event.key == Qt.Key_W) {
                    shape.secondarySourceTranslation = Qt.vector2d(
                        shape.secondarySourceTranslation.x, shape.secondarySourceTranslation.y + ((event.modifiers & shift) ? 1.0 : -1.0));
                } else if (event.key == Qt.Key_X) {
                    shape.secondarySourceScale = Qt.vector2d(
                        shape.secondarySourceScale.x + ((event.modifiers & shift) ? 0.05 : -0.05), shape.secondarySourceScale.y);
                } else if (event.key == Qt.Key_C) {
                    shape.secondarySourceScale = Qt.vector2d(
                        shape.secondarySourceScale.x, shape.secondarySourceScale.y + ((event.modifiers & shift) ? 0.05 : -0.05));
                }
            }
        }
    }
}
