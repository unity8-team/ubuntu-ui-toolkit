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
