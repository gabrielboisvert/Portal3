#version 330 core

in vec3 fragPos;
in vec3 norm;
in vec2 text;

uniform sampler2D aTexture;
uniform int hasTexture;

out vec4 fragColor;


void main()
{
	if (hasTexture == 1)
		fragColor = texture(aTexture, text);
	else
		fragColor = vec4(0.8, 0.8, 0.8, 0.3);
}