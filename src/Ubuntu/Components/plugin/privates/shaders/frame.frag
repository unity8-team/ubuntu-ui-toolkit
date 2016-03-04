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

uniform sampler2D texture[2];
uniform lowp float opacity;
varying mediump vec2 outerCoord;
varying mediump vec2 innerCoord;
varying lowp vec4 color;

void main(void)
{
    lowp float outerShape = texture2D(texture[0], outerCoord).r;
    lowp float innerShape = texture2D(texture[1], innerCoord).r;
    // Fused multiply-add friendly version of (outerShape * (1.0 - innerShape))
    lowp float shape = (outerShape * -innerShape) + outerShape;
    // Squared to make thinner corners (particularly visible at stroke 1).
    gl_FragColor = vec4(shape * shape * opacity) * color;
}
