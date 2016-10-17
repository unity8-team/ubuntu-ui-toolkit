uniform highp mat4 matrix;

attribute highp vec4 positionAttrib;
attribute mediump vec2 maskCoordAttrib;
attribute lowp vec4 colorAttrib;

varying mediump vec2 maskCoord;
varying lowp vec4 color;

void main()
{
    maskCoord = maskCoordAttrib;
    color = colorAttrib;
    gl_Position = matrix * positionAttrib;
}
