uniform highp mat4 matrix;

attribute highp vec4 positionAttrib;
attribute mediump vec2 borderCoordAttrib;
attribute lowp vec4 colorAttrib;
attribute lowp vec4 borderColorAttrib;

varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 borderColor;

void main()
{
    borderCoord = borderCoordAttrib;
    color = colorAttrib;
    borderColor = borderColorAttrib;
    gl_Position = matrix * positionAttrib;
}
