import QtQuick 2.4

Item {
    // Public properties.
    property Item scene: null
    property MouseArea area: null

    id: root
    visible: scene != null
    property real __scaling: 1.0
    property variant __translation: Qt.point(0.0, 0.0)

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: scene
        hideSource: scene != null
        visible: false
        smooth: false
    }

    ShaderEffect {
        id: effect
        anchors.fill: parent
        property real scaling: __scaling
        property variant translation: __translation
        property variant texture: effectSource

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            uniform mediump float scaling;
            uniform mediump vec2 translation;
            attribute highp vec4 qt_Vertex;
            attribute mediump vec2 qt_MultiTexCoord0;
            varying mediump vec2 texCoord;
            void main() {
                texCoord = qt_MultiTexCoord0 * vec2(scaling) + translation;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            uniform sampler2D texture;
            uniform lowp float qt_Opacity;
            varying mediump vec2 texCoord;
            void main() {
                gl_FragColor = texture2D(texture, texCoord) * qt_Opacity;
            }"
    }

    // Mouse handling.
    Connections {
        target: scene != null ? area : null
        property real lastX: 0.0
        property real lastY: 0.0
        property real scalingLevel: 0.0
        property real maxScalingLevel: 48.317715856  // Logarithm of 100 to base 1.1 (scalingFactor).
        property real scalingFactor: 1.1

        onPressed: {
            if (area.pressedButtons & Qt.LeftButton) {
                lastX = area.mouseX;
                lastY = area.mouseY;
            }
        }

        onPositionChanged: {
            if (area.pressedButtons & Qt.LeftButton) {
                var tx = __translation.x;
                var ty = __translation.y;
                var sx = __scaling / width;
                var sy = __scaling / height;
                var x = area.mouseX - lastX;
                var y = area.mouseY - lastY;
                __translation = Qt.point(Math.max(0.0, Math.min(1.0 - __scaling, tx - sx * x)),
                                         Math.max(0.0, Math.min(1.0 - __scaling, ty - sy * y)));
                lastX = area.mouseX;
                lastY = area.mouseY;
            }
        }

        onWheel: {
            scalingLevel = Math.max(0.0, Math.min(maxScalingLevel,
                scalingLevel + (wheel.angleDelta.y < 0.0 ? -1.0 : 1.0)));
            var oldScaling = __scaling;
            __scaling = Math.pow(scalingFactor, -scalingLevel);
            var s = oldScaling - __scaling;
            var tx = __translation.x;
            var ty = __translation.y;
            var x = area.mouseX / width;
            var y = area.mouseY / height;
            __translation = Qt.point(Math.max(0.0, Math.min(1.0 - __scaling, tx + s * x)),
                                     Math.max(0.0, Math.min(1.0 - __scaling, ty + s * y)));
        }
    }
}
