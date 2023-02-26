#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 aColor;

uniform mat4 vp;

void main()
{
    gl_Position = vp * vec4(position, 1.0f);
    aColor = color;
}