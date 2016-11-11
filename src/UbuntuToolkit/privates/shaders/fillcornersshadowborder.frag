uniform sampler2D shadowTexture;
uniform sampler2D borderTexture;
uniform lowp float opacity;

varying mediump vec2 maskCoord;
varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying mediump vec2 borderCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;
varying lowp vec4 borderColor;

void main()
{
    lowp float maskCoverage = texture2D(shadowTexture, maskCoord).a;
    mediump vec2 shadowTextureCoord = midShadowCoord - abs(shadowCoord - midShadowCoord);
    lowp float shadowCoverage = 1.0 - texture2D(shadowTexture, shadowTextureCoord).r;
    lowp vec4 shadow = shadowColor * vec4(shadowCoverage);
    lowp vec4 blend = shadow + (vec4(1.0 - shadow.a) * color);
    lowp float borderCoverage = 1.0 - texture2D(borderTexture, borderCoord).r;
    lowp vec4 border = borderColor * vec4(borderCoverage);
    blend = border + (vec4(1.0 - border.a) * blend);
    gl_FragColor = blend * vec4(maskCoverage * opacity);
}
