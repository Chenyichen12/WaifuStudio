#version 330 core
uniform sampler2D ourTexture;
in vec2 o_UV;
void main() {
    gl_FragColor = vec4(o_UV,0.0,1.0);
}