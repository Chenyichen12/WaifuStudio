#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D ourTexture;
in vec2 o_UV;
void main() {
    color = vec4(o_UV,0.0,1.0);
}