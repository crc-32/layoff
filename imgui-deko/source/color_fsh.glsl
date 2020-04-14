#version 460

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outColor;

void main()
{
    //outColor = vec4(inColor, 1.0);
    outColor = vec4(1.0,0.0,0.0,1.0);
}