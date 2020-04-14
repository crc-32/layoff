#version 460

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inCol;

layout (location = 0) out vec4 outCol;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0f, 1.0f);
    outCol = inCol;
}