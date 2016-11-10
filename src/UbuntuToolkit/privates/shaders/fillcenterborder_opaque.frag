uniform sampler2D borderTexture;

varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 borderColor;

void main()
{
    lowp float borderCoverage = 1.0 - texture2D(borderTexture, borderCoord).r;
    lowp vec4 border = borderColor * vec4(borderCoverage);
    gl_FragColor = border + (vec4(1.0 - border.a) * color);
}
