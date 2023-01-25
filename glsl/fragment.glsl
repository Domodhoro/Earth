#version 300 es

out highp vec4 FragColor;

uniform sampler2D TextureSampler;

in highp vec2 Texture;

void main() {
    highp vec2 textureInput = Texture;

    textureInput.x = 1.0f - textureInput.x;

    highp vec4 FragTexture = texture(TextureSampler, textureInput);

    if (FragTexture.x == 1.0f && FragTexture.z == 1.0f) {
        discard;
    }

    FragColor = FragTexture;
}
