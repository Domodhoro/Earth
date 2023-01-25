#version 300 es

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture;

uniform mat4 model, view, projection;

out highp vec2 Texture;

void main() {
    gl_Position = projection * view * model * vec4(position.xyz, 1.0f);

    Texture = texture;
}
