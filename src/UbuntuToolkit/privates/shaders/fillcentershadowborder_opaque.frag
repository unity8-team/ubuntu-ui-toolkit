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
uniform sampler2D borderTexture;

varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;
varying lowp vec4 borderColor;

void main(void)
{
    mediump vec2 shadowTextureCoord = midShadowCoord - abs(shadowCoord - midShadowCoord);
    lowp float shadowCoverage = 1.0 - texture2D(shadowTexture, shadowTextureCoord).r;
    lowp vec4 shadow = shadowColor * vec4(shadowCoverage);
    lowp vec4 blend = shadow + (vec4(1.0 - shadow.a) * color);
    lowp float borderCoverage = 1.0 - texture2D(borderTexture, borderCoord).r;
    lowp vec4 border = borderColor * vec4(borderCoverage);
    gl_FragColor = border + (vec4(1.0 - border.a) * blend);

    //gl_FragColor = vec4(borderCoord.s, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
