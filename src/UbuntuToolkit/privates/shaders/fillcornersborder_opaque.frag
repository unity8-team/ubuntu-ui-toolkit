uniform sampler2D maskTexture;
uniform sampler2D borderTexture;

varying mediump vec2 maskCoord;
varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 borderColor;

void main()
{
    lowp float borderCoverage = 1.0 - texture2D(borderTexture, borderCoord).r;
    lowp vec4 border = borderColor * vec4(borderCoverage);
    lowp vec4 blend = border + (vec4(1.0 - border.a) * color);
    lowp float maskCoverage = texture2D(maskTexture, maskCoord).r;
    gl_FragColor = blend * vec4(maskCoverage);
}
