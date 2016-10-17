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

uniform highp mat4 matrix;

attribute highp vec4 positionAttrib;
attribute mediump vec2 shadowCoordAttrib;
attribute mediump vec2 midShadowCoordAttrib;
attribute mediump vec2 borderCoordAttrib;
attribute lowp vec4 colorAttrib;
attribute lowp vec4 shadowColorAttrib;
attribute lowp vec4 borderColorAttrib;

varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;
varying lowp vec4 borderColor;

void main()
{
    shadowCoord = shadowCoordAttrib;
    midShadowCoord = midShadowCoordAttrib;
    borderCoord = borderCoordAttrib;
    color = colorAttrib;
    shadowColor = shadowColorAttrib;
    borderColor = borderColorAttrib;
    gl_Position = matrix * positionAttrib;
}
