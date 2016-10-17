uniform highp mat4 matrix;

attribute highp vec4 positionAttrib;
attribute mediump vec2 texCoord1Attrib;
attribute mediump vec2 texCoord2Attrib;
attribute lowp vec4 colorAttrib;

varying mediump vec2 texCoord1;
varying mediump vec2 texCoord2;
varying lowp vec4 color;

void main()
{
    texCoord1 = texCoord1Attrib;
    texCoord2 = texCoord2Attrib;
    color = colorAttrib;
    gl_Position = matrix * positionAttrib;
}
