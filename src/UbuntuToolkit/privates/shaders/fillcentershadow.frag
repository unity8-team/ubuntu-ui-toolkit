uniform sampler2D shadowTexture;
uniform lowp float opacity;

varying mediump vec2 shadowCoord;
varying mediump vec2 midShadowCoord;
varying lowp vec4 color;
varying lowp vec4 shadowColor;

void main()
{
    mediump vec2 shadowTextureCoord = midShadowCoord - abs(shadowCoord - midShadowCoord);
    lowp float shadowCoverage = 1.0 - texture2D(shadowTexture, shadowTextureCoord).r;
    lowp vec4 shadow = shadowColor * vec4(shadowCoverage);
    lowp vec4 blend = shadow + (vec4(1.0 - shadow.a) * color);
    gl_FragColor = blend * vec4(opacity);
}
