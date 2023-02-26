#version 330 core

in vec3 fragPos;
in vec2 text;

uniform sampler2D aTexture;
uniform int hasTexture;
uniform vec4 objColor;
uniform float alpha;
uniform int isSelected;

out vec4 fragColor;


void main()
{
	if (hasTexture == 1)
		fragColor = objColor * vec4(texture(aTexture, text).xyz, alpha);
	else
		fragColor = objColor * vec4(0.8, 0.8, 0.8, alpha);

	if (isSelected == 1)
		fragColor = fragColor * vec4(0, 0.5f, 0, 1);
}