uniform sampler2D maskTexture;

varying mediump vec2 maskCoord;
varying lowp vec4 color;

void main()
{
    gl_FragColor = color * vec4(texture2D(maskTexture, maskCoord).r);
}
