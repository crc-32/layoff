#version 460

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outColor;

void main()
{
    //outColor = inColor;
    outColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}