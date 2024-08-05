#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

out vec2 o_TexCoord;
void main()
{
    o_TexCoord = texCoord;
    gl_Position = position;
}