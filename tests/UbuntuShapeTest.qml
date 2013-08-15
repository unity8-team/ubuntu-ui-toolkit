import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: root
    width: 700
    height: 700
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
    property real cornerRadius: 20.0
    property color backgroundColor: Qt.rgba(0.6, 0.6, 1.0, 1.0)
    property color secondaryBackgroundColor: Qt.rgba(0.3, 0.3, 0.5, 1.0)
    property variant backgroundMode: UbuntuShape.BackgroundColor
    property variant overlayGeometry: Qt.rect(0.0, 0.0, 0.0, 0.0)
    property color overlayColor: Qt.rgba(0.0, 0.0, 0.0, 0.75)
    property variant overlayBlending: UbuntuShape.SourceOver
    property color shadowColorIn: Qt.rgba(0.0, 0.0, 0.0, 1.0)
    property real shadowSizeIn: 25.0
    property real shadowAngleIn: 0.0
    property real shadowDistanceIn: 0.0
    property color shadowColorOut: Qt.rgba(0.0, 0.0, 0.0, 0.9)
    property real shadowSizeOut: 50.0
    property real shadowAngleOut: 0.0
    property real shadowDistanceOut: 0.0
    property real sourceOpacity: 1.0
    property variant sourceFillMode: UbuntuShape.PreserveAspectCrop
    property variant sourceHorizontalWrapMode: UbuntuShape.ClampToEdge
    property variant sourceVerticalWrapMode: UbuntuShape.ClampToEdge
    property variant sourceHorizontalAlignment: UbuntuShape.AlignHCenter
    property variant sourceVerticalAlignment: UbuntuShape.AlignVCenter
    property real secondarySourceOpacity: 1.0
    property variant secondarySourceFillMode: UbuntuShape.Pad
    property variant secondarySourceHorizontalWrapMode: UbuntuShape.Repeat
    property variant secondarySourceVerticalWrapMode: UbuntuShape.Repeat
    property variant secondarySourceHorizontalAlignment: UbuntuShape.AlignHCenter
    property variant secondarySourceVerticalAlignment: UbuntuShape.AlignVCenter
    property Image defaultImage: Image { source: "ubuntu.jpg" }
    property Image image: defaultImage

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
            anchors.margins: 200
            cornerRadius: root.cornerRadius
            backgroundColor: root.backgroundColor
            secondaryBackgroundColor: root.secondaryBackgroundColor
            backgroundMode: root.backgroundMode
            overlayGeometry: root.overlayGeometry
            overlayColor: root.overlayColor
            overlayBlending: root.overlayBlending
            shadowColorIn: root.shadowColorIn
            shadowSizeIn: root.shadowSizeIn
            shadowAngleIn: root.shadowAngleIn
            shadowDistanceIn: root.shadowDistanceIn
            shadowColorOut: root.shadowColorOut
            shadowSizeOut: root.shadowSizeOut
            shadowAngleOut: root.shadowAngleOut
            shadowDistanceOut: root.shadowDistanceOut
            source: root.image
            sourceOpacity: root.sourceOpacity
            sourceFillMode: root.sourceFillMode
            sourceHorizontalWrapMode: root.sourceHorizontalWrapMode
            sourceVerticalWrapMode: root.sourceVerticalWrapMode
            sourceHorizontalAlignment: root.sourceHorizontalAlignment
            sourceVerticalAlignment: root.sourceVerticalAlignment
            secondarySource: Image { source: "paper.jpg" }
            secondarySourceOpacity: root.secondarySourceOpacity
            secondarySourceFillMode: root.secondarySourceFillMode
            secondarySourceHorizontalWrapMode: root.secondarySourceHorizontalWrapMode
            secondarySourceVerticalWrapMode: root.secondarySourceVerticalWrapMode
            secondarySourceHorizontalAlignment: root.secondarySourceHorizontalAlignment
            secondarySourceVerticalAlignment: root.secondarySourceVerticalAlignment
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
        text: "Zoom:                x " + root.scaleFactor.toFixed(1) + "\n\n" +
              "Corner radius:       " + root.cornerRadius + "\n\n" +
              "Background colors:   " + root.backgroundColor + ", " + root.secondaryBackgroundColor + "\n" +
              "Background mode:     " + root.backgroundModeTable[root.backgroundMode] + "\n\n" +
              "Overlay color:       " + root.overlayColor + "\n" +
              "Overlay geometry:    " + root.overlayGeometry.x.toFixed(2) + ", " + root.overlayGeometry.y.toFixed(2) + ", " + root.overlayGeometry.width.toFixed(2) + ", " + root.overlayGeometry.height.toFixed(2) + "\n" +
              "Overlay blending:    " + root.blendingTable[root.overlayBlending] + "\n\n" +
              "Shadow color in:     " + root.shadowColorIn + "\n" +
              "Shadow size in:      " + root.shadowSizeIn.toFixed(1) + "\n" +
              "Shadow angle in:     " + root.shadowAngleIn.toFixed(1) + "°\n" +
              "Shadow distance in:  " + root.shadowDistanceIn.toFixed(1) + "\n" +
              "Shadow color out:    " + root.shadowColorOut + "\n" +
              "Shadow size out:     " + root.shadowSizeOut.toFixed(1) + "\n" +
              "Shadow angle out:    " + root.shadowAngleOut.toFixed(1) + "°\n" +
              "Shadow distance out: " + root.shadowDistanceOut.toFixed(1) + "\n\n" +
              "Source 1:            " + root.image + "\n" +
              "Source 1 opacity:    " + root.sourceOpacity.toFixed(2) + "\n" +
              "Source 1 fill:       " + root.fillModeTable[root.sourceFillMode] + "\n" +
              "Source 1 hwrap:      " + root.wrapModeTable[root.sourceHorizontalWrapMode] + "\n" +
              "Source 1 vwrap:      " + root.wrapModeTable[root.sourceVerticalWrapMode] + "\n" +
              "Source 1 halign:     " + root.hAlignmentTable[root.sourceHorizontalAlignment] + "\n" +
              "Source 1 valign:     " + root.vAlignmentTable[root.sourceVerticalAlignment] + "\n" +
              ""
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

        // Corner.
        if (event.key == Qt.Key_A) {
            // FIXME(loicm) Assign feature here.
        } else if (event.key == Qt.Key_Z) {
            root.cornerRadius = Math.max(0.0, Math.min(
                500.0, root.cornerRadius + ((event.modifiers & shift) ? 1.0 : -1.0)));

        // Background.
        } else if (event.key == Qt.Key_E) {
            root.backgroundColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_R) {
            root.secondaryBackgroundColor =
                Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_T) {
            root.backgroundMode = (root.backgroundMode + 1) % 3;

        // Overlay.
        } else if (event.key == Qt.Key_Y) {
            root.overlayColor = Qt.rgba(Math.random(), Math.random(), Math.random(), Math.random());
        } else if (event.key == Qt.Key_U) {
            var x = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.x + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                x, root.overlayGeometry.y, root.overlayGeometry.width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_I) {
            var y = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.y + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, y, root.overlayGeometry.width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_O) {
            var width = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.width + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, root.overlayGeometry.y, width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_P) {
            var height = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.height + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, root.overlayGeometry.y, root.overlayGeometry.width, height);
        } else if (event.key == Qt.Key_Q) {
            root.overlayBlending = (root.overlayBlending + 1) % 2;

        // Shadows.
        } else if (event.key == Qt.Key_S) {
            root.shadowColorIn = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.9);
        } else if (event.key == Qt.Key_D) {
            root.shadowSizeIn = Math.max(0.0, Math.min(400.0,
                root.shadowSizeIn + ((event.modifiers & shift) ? 1.0 : -1.0)));
        } else if (event.key == Qt.Key_F) {
            if (!(event.modifiers & Qt.ShiftModifier)) {
                root.shadowAngleIn = (root.shadowAngleIn > 0.1) ?
                    (root.shadowAngleIn - 2.5) : 360.0 - 2.5;
            } else {
                root.shadowAngleIn = (root.shadowAngleIn < 359.9) ?
                    (root.shadowAngleIn + 2.5) : 2.5;
            }
        } else if (event.key == Qt.Key_G) {
            root.shadowDistanceIn = Math.max(0.0, Math.min(200.0,
                root.shadowDistanceIn + ((event.modifiers & shift) ? 1.0 : -1.0)));
        } else if (event.key == Qt.Key_H) {
            root.shadowColorOut = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.9);
        } else if (event.key == Qt.Key_J) {
            root.shadowSizeOut = Math.max(0.0, Math.min(400.0,
                root.shadowSizeOut + ((event.modifiers & shift) ? 1.0 : -1.0)));
        } else if (event.key == Qt.Key_K) {
            if (!(event.modifiers & Qt.ShiftModifier)) {
                root.shadowAngleOut = (root.shadowAngleOut > 0.1) ?
                    (root.shadowAngleOut - 2.5) : 360.0 - 2.5;
            } else {
                root.shadowAngleOut = (root.shadowAngleOut < 359.9) ?
                    (root.shadowAngleOut + 2.5) : 2.5;
            }
            // shape.sourceScale = Qt.vector2d(shape.sourceScale.x,
            //     shape.sourceScale.y + ((event.modifiers & shift) ? 0.05 : -0.05));
            // print (shape.sourceScale);
        } else if (event.key == Qt.Key_L) {
            root.shadowDistanceOut = Math.max(0.0, Math.min(200.0,
                root.shadowDistanceOut + ((event.modifiers & shift) ? 1.0 : -1.0)));
            // shape.sourceTranslation = Qt.vector2d(shape.sourceTranslation.x,
            //     shape.sourceTranslation.y + ((event.modifiers & shift) ? 0.05 : -0.05));
            // print (shape.sourceTranslation);

        // Source.
        } else if (event.key == Qt.Key_M) {
            if (root.image == null) {
                root.image = root.defaultImage;
            } else {
                root.image = null;
            }
            // shape.sourceScale = Qt.vector2d(
            //     shape.sourceScale.x + ((event.modifiers & shift) ? 1.0 : -1.0), shape.sourceScale.y);
            // print (shape.sourceScale);
        } else if (event.key == Qt.Key_W) {
            // shape.sourceTranslation = Qt.vector2d(
            //     shape.sourceTranslation.x + ((event.modifiers & shift) ? 0.283 : -0.283), shape.sourceTranslation.y);
            // print (shape.sourceTranslation);
            root.sourceOpacity = Math.max(0.0, Math.min(1.0,
                root.sourceOpacity + ((event.modifiers & shift) ? 0.01 : -0.01)));
        } else if (event.key == Qt.Key_X) {
            root.sourceFillMode = (root.sourceFillMode + 1) % 4;
        } else if (event.key == Qt.Key_C) {
            root.sourceHorizontalWrapMode = (root.sourceHorizontalWrapMode + 1) % 3;
        } else if (event.key == Qt.Key_V) {
            root.sourceVerticalWrapMode = (root.sourceVerticalWrapMode + 1) % 3;
        } else if (event.key == Qt.Key_B) {
            root.sourceHorizontalAlignment = (root.sourceHorizontalAlignment + 1) % 3;
        } else if (event.key == Qt.Key_N) {
            root.sourceVerticalAlignment = (root.sourceVerticalAlignment + 1) % 3;

        // Secondary source.
        // } else if (event.key == Qt.Key_M) {
        //     if (root.image == null) {
        //         root.image = root.defaultImage;
        //     } else {
        //         root.image = null;
        //     }
        } else if (event.key == Qt.Key_1) {
            root.secondarySourceOpacity = Math.max(0.0, Math.min(1.0,
                root.secondarySourceOpacity + ((event.modifiers & shift) ? 0.01 : -0.01)));
        // } else if (event.key == Qt.Key_X) {
        //     root.sourceFillMode = (root.sourceFillMode + 1) % 4;
        // } else if (event.key == Qt.Key_C) {
        //     root.sourceWrapMode = (root.sourceWrapMode + 1) % 4;
        // } else if (event.key == Qt.Key_V) {
        //     root.sourceHorizontalAlignment = (root.sourceHorizontalAlignment + 1) % 3;
        // } else if (event.key == Qt.Key_B) {
        //     root.sourceVerticalAlignment = (root.sourceVerticalAlignment + 1) % 3;
        }
    }
}
