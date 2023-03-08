#version 300 es

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Texture;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out highp vec2 FragTexture;

void main() {
    gl_Position = Projection * View * Model * vec4(Position.xyz, 1.0f);
    FragTexture = Texture;
}
