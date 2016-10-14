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
attribute lowp vec4 colorAttrib;
attribute lowp vec4 shadowColorAttrib;
varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;

void main()
{
    shadowCoord = shadowCoordAttrib;
    midShadowCoord = midShadowCoordAttrib;
    color = colorAttrib;
    shadowColor = shadowColorAttrib;
    gl_Position = matrix * positionAttrib;
}
