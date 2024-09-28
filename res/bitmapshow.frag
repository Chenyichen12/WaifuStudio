#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D ourTexture;
in vec2 o_UV;
void main() {
    if(o_UV.x < 0.0 || o_UV.x > 1.0 || o_UV.y < 0.0 || o_UV.y > 1.0){
        color = vec4(0.0,0.0,0.0,0.0);
    }else{
        color = texture(ourTexture,o_UV);
    }

}