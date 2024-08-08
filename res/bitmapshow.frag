#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D ourTexture;
in vec2 o_UV;
void main() {
    color = texture(ourTexture,o_UV);
}