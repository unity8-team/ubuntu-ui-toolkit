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
 */

import QtQuick 2.4

Item {
    // Public properties.
    property Item scene: null
    property MouseArea area: null

    id: root
    visible: scene != null
    property real __scaling: 1.0
    property variant __translation: Qt.point(0.0, 0.0)

    // The FBO abstraction handling our first offscreen pass.
    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: scene
        hideSource: scene != null
        visible: false
        smooth: false  // Nearest neighbour texture filtering.
    }

    // The shader abstraction handling our second pass with the
    // translation and scaling in the vertex shader and the simple
    // texturing from the FBO in the fragment shader.
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
                varying vec2 texCoord;
                void main() {
                    texCoord =
                        qt_MultiTexCoord0 * vec2(scaling)
                        + translation;
                    gl_Position = qt_Matrix * qt_Vertex;
                }"
            fragmentShader: "
                uniform sampler2D texture;
                uniform lowp float qt_Opacity;
                varying mediump vec2 texCoord;
                void main() {
                    gl_FragColor =
                        texture2D(texture, texCoord) * qt_Opacity;
                }"
    }

    // Mouse handling.
    Connections {
        target: scene != null ? area : null
    }
}
