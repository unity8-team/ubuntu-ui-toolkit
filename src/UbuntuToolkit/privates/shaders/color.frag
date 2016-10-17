uniform lowp float opacity;
varying lowp vec4 color;

void main()
{
    gl_FragColor = vec4(opacity) * color;
}
