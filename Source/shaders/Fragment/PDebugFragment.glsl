#version 330 core
in vec3 aColor;
out vec4 color;

void main()
{
    color = vec4(aColor, 1.0);
}