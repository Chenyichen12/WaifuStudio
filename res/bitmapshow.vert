#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 UV;

uniform mat3 uProjectionMatrix;

out vec2 o_UV;
void main()
{
    o_UV = UV;
    gl_Position = vec4((uProjectionMatrix * vec3(position, 1.0)).xy, 0.0, 1.0);
}