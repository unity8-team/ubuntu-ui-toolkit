import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: root
    width: 700
    height: 700
    focus: true

    // Enum to string tables.
    property variant backgroundFillModeTable: [
        "BackgroundColor", "HorizontalGradient", "VerticalGradient"
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
    property real cornerRadius: 100.0
    property real cornerSquareness: 0.0
    property color backgroundColor: Qt.rgba(0.6, 0.6, 1.0, 1.0)
    property color secondaryBackgroundColor: Qt.rgba(0.3, 0.3, 0.5, 1.0)
    property variant backgroundFillMode: Shape2.VerticalGradient
    property color shadowColorOut: Qt.rgba(0.0, 0.0, 0.0, 0.9)
    property variant shadowRadiusOut: Qt.vector4d(150.0, 50.0, 100.0, 100.0)
    property variant overlayGeometry: Qt.rect(0.0, 0.0, 0.0, 0.0)
    property color overlayColor: Qt.rgba(0.0, 0.0, 0.0, 0.75)
    property Image defaultImage: Image { source: "map_icon.png" }
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
        Shape2 {
            id: shape
            anchors.fill: parent
            anchors.margins: 25
            backgroundColor: root.backgroundColor
            secondaryBackgroundColor: root.secondaryBackgroundColor
            backgroundFillMode: root.backgroundFillMode
            shadowRadiusOut: root.shadowRadiusOut
            shadowColorOut: root.shadowColorOut
            cornerRadius: root.cornerRadius
            cornerSquareness: root.cornerSquareness
            overlayGeometry: root.overlayGeometry
            overlayColor: root.overlayColor
            source: root.image
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
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 18
        font.weight: Font.Bold
        style: Text.Outline
        styleColor: "black"
        color: "white"
        text: "Zoom:              x" + root.scaleFactor.toFixed(1) + "\n" +
              "Corner radius:     " + root.cornerRadius + "\n" +
              "Corner squareness: " + root.cornerSquareness.toFixed(2) + " (TBD)\n" +
              "Background colors: " + root.backgroundColor + "," + root.secondaryBackgroundColor + "\n" +
              "Background mode:   " + root.backgroundFillModeTable[root.backgroundFillMode] + "\n" +
              "Shadow color:      " + root.shadowColorOut + "\n" +
              "Shadow radius out: " + root.shadowRadiusOut.x + "," + root.shadowRadiusOut.y + "," + root.shadowRadiusOut.z + "," + root.shadowRadiusOut.w + "\n" +
              "Overlay color:     " + root.overlayColor + "\n" +
              "Overlay geometry:  " + root.overlayGeometry.x.toFixed(2) + "," + root.overlayGeometry.y.toFixed(2) + "," + root.overlayGeometry.width.toFixed(2) + "," + root.overlayGeometry.height.toFixed(2) + "\n" +
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
        // Corner radius.
        if (event.key == Qt.Key_R) {
            root.cornerRadius = Math.max(1.0, Math.min(
                500.0, root.cornerRadius + ((event.modifiers & shift) ? 1.0 : -1.0)));
        // Corner squareness.
        } else if (event.key == Qt.Key_S) {
            root.cornerSquareness = Math.max(0.0, Math.min(
                1.0, root.cornerSquareness + ((event.modifiers & shift) ? 0.01 : -0.01)));
        // Background.
        } else if (event.key == Qt.Key_B) {
            root.backgroundColor = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_V) {
            root.secondaryBackgroundColor =
                Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_C) {
            root.backgroundFillMode = (root.backgroundFillMode + 1) % 3;
        // Source image.
        } else if (event.key == Qt.Key_Z) {
            if (root.image == null) {
                root.image = root.defaultImage;
            } else {
                root.image = null;
            }
        // Shadow color out.
        } else if (event.key == Qt.Key_X) {
            root.shadowColorOut = Qt.rgba(Math.random(), Math.random(), Math.random(), 0.9);
        // Shadow radius out.
        } else if (event.key == Qt.Key_U) {
            var x = Math.max(0.0, Math.min(400.0,
                root.shadowRadiusOut.x + ((event.modifiers & shift) ? 1.0 : -1.0)));
            root.shadowRadiusOut = Qt.vector4d(
                x, root.shadowRadiusOut.y, root.shadowRadiusOut.z, root.shadowRadiusOut.w);
        } else if (event.key == Qt.Key_I) {
            var y = Math.max(0.0, Math.min(400.0,
                root.shadowRadiusOut.y + ((event.modifiers & shift) ? 1.0 : -1.0)));
            root.shadowRadiusOut = Qt.vector4d(
                root.shadowRadiusOut.x, y, root.shadowRadiusOut.z, root.shadowRadiusOut.w);
        } else if (event.key == Qt.Key_O) {
            var z = Math.max(0.0, Math.min(400.0,
                root.shadowRadiusOut.z + ((event.modifiers & shift) ? 1.0 : -1.0)));
            root.shadowRadiusOut = Qt.vector4d(
                root.shadowRadiusOut.x, root.shadowRadiusOut.y, z, root.shadowRadiusOut.w);
        } else if (event.key == Qt.Key_P) {
            var w = Math.max(0.0, Math.min(400.0,
                root.shadowRadiusOut.w + ((event.modifiers & shift) ? 1.0 : -1.0)));
            root.shadowRadiusOut = Qt.vector4d(
                root.shadowRadiusOut.x, root.shadowRadiusOut.y, root.shadowRadiusOut.z, w);
        // Overlay color.
        } else if (event.key == Qt.Key_G) {
            root.overlayColor = Qt.rgba(Math.random(), Math.random(), Math.random(), Math.random());
        // Overlay geometry.
        } else if (event.key == Qt.Key_J) {
            var x = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.x + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                x, root.overlayGeometry.y, root.overlayGeometry.width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_K) {
            var y = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.y + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, y, root.overlayGeometry.width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_L) {
            var width = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.width + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, root.overlayGeometry.y, width, root.overlayGeometry.height);
        } else if (event.key == Qt.Key_M) {
            var height = Math.max(0.0, Math.min(1.0,
                root.overlayGeometry.height + ((event.modifiers & shift) ? 0.01 : -0.01)));
            root.overlayGeometry = Qt.rect(
                root.overlayGeometry.x, root.overlayGeometry.y, root.overlayGeometry.width, height);
        }
    }
}
