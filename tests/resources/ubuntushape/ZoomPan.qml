import QtQuick 2.4

Item {
    // --- Public API ---

    // The zoomed and panned item.
    property Item scene: null

    // The mouse area covering the scene item.
    property MouseArea area: null

    // The background color.
    property color backgroundColor: Qt.rgba(1.0, 1.0, 1.0, 1.0)

    // Render a grid on the pixel boundaries when zoomed.
    property bool zoomGrid: true

    // The grid color.
    property color zoomGridColor: Qt.rgba(0.0, 0.0, 0.0, 1.0)

    // The zoom factor.
    readonly property real zoomFactor: 1.0 / __scaling

    // --- Public API ---

    id: root
    visible: scene != null
    property real __scaling: 1.0
    property variant __translation: Qt.point(0.0, 0.0)

    Rectangle {
        id: background
        anchors.fill: parent
        color: root.backgroundColor
        visible: root.zoomFactor <= 1.0
    }

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: scene
        hideSource: root.zoomFactor > 1.0
        visible: false
        smooth: false
    }

    ShaderEffect {
        id: effectNoGrid
        anchors.fill: parent
        visible: (root.zoomFactor > 1.0) && !zoomGrid
        property real scaling: __scaling
        property variant translation: __translation
        property variant texture: effectSource
        property color backgroundColor: root.backgroundColor

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
            uniform lowp vec4 backgroundColor;
            uniform lowp float qt_Opacity;
            varying mediump vec2 texCoord;
            void main() {
                lowp vec4 scene = texture2D(texture, texCoord);
                lowp vec4 blend = scene + vec4(1.0 - scene.a) * backgroundColor;
                gl_FragColor = blend * vec4(qt_Opacity);
            }"
    }

    ShaderEffect {
        id: effectGrid
        anchors.fill: parent
        visible: (root.zoomFactor > 1.0) && zoomGrid
        property real scaling: __scaling
        property variant translation: __translation
        property variant texture: effectSource
        property color backgroundColor: root.backgroundColor
        property variant size: Qt.point(Math.ceil(width), Math.ceil(height))
        property color gridColor: zoomGridColor
        property real gridOpacity: 0.0
        property real zoomFactor: -root.zoomFactor

        onScalingChanged: {
            var fromStart = 3.0;
            var fromEnd = 15.0;
            var toStart = 0.0;
            var toEnd = 0.75;
            var x = root.zoomFactor;
            var remap = toStart + (((x - fromStart) * (toEnd - toStart)) / (fromEnd - fromStart));
            gridOpacity = Math.max(toStart, Math.min(remap, toEnd));
        }

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            uniform mediump float scaling;
            uniform mediump vec2 translation;
            uniform mediump vec2 size;
            attribute highp vec4 qt_Vertex;
            attribute mediump vec2 qt_MultiTexCoord0;
            varying mediump vec2 texCoord;
            varying mediump vec2 sizeCoord;
            void main() {
                texCoord = qt_MultiTexCoord0 * vec2(scaling) + translation;
                sizeCoord = texCoord * size;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            uniform sampler2D texture;
            uniform mediump float zoomFactor;
            uniform lowp vec4 backgroundColor;
            uniform lowp vec4 gridColor;
            uniform lowp float gridOpacity;
            uniform lowp float qt_Opacity;
            varying mediump vec2 sizeCoord;
            varying mediump vec2 texCoord;
            void main() {
                lowp vec4 scene = texture2D(texture, texCoord);
                mediump vec2 frac = fract(sizeCoord - vec2(0.5)) - vec2(0.5);
                mediump vec2 boundary = clamp((abs(frac) * vec2(zoomFactor)) + vec2(1.0), vec2(0.0), vec2(1.0));
                lowp vec4 grid = gridColor * vec4(max(boundary.x, boundary.y) * gridOpacity);
                lowp vec4 blend = (vec4(1.0 - scene.a) * backgroundColor) + scene;
                blend = (vec4(1.0 - grid.a) * blend) + grid;
                gl_FragColor = blend * vec4(qt_Opacity);
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
