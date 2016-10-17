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
