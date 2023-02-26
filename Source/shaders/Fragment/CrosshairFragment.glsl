#version 330 core

in vec3 fragPos;
in vec2 text;

uniform sampler2D aTexture;

out vec4 fragColor;


void main()
{
	fragColor = texture(aTexture, text);
}