#version 300 es

out highp vec4 FragColor;

uniform sampler2D TextureSampler;

in highp vec2 FragTexture;

void main() {
    highp vec2 aux   = FragTexture;
    aux.x            = 1.0f - aux.x;
    highp vec4 color = texture(TextureSampler, aux);

    if (color.r == 1.0f && color.g != 0.0f && color.z == 1.0f) discard;

    FragColor = color;
}
