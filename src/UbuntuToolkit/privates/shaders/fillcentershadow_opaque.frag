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

uniform sampler2D shadowTexture;
varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;

void main(void)
{
    mediump vec2 shadowTextureCoord = midShadowCoord - abs(shadowCoord - midShadowCoord);
    lowp float shadowCoverage = 1.0 - texture2D(shadowTexture, shadowTextureCoord).r;
    lowp vec4 shadow = shadowColor * vec4(shadowCoverage);
    gl_FragColor = shadow + (vec4(1.0 - shadow.a) * color);

    // lowp float luma = texture2D(shadowTexture, shadowCoord).r;
    // lowp float a = 1.0 - luma;
    // gl_FragColor = vec4(luma) * color + vec4(a);
}
