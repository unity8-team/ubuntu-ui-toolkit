uniform sampler2D texture[2];
uniform lowp float opacity;

varying mediump vec2 texCoord1;
varying mediump vec2 texCoord2;
varying lowp vec4 color;

void main()
{
    lowp float outerShape = texture2D(texture[0], texCoord1).r;
    lowp float innerShape = texture2D(texture[1], texCoord2).r;
    // Fused multiply-add friendly version of (outerShape * (1.0 - innerShape))
    lowp float shape = (outerShape * -innerShape) + outerShape;
    gl_FragColor = vec4(shape * opacity) * color;
}
