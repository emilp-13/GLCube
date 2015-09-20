#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 mtxWorldViewProj;

out vec4 outColor;

void main()
{
    gl_Position = mtxWorldViewProj * vec4(position, 1.0f);
    outColor = color;
}
