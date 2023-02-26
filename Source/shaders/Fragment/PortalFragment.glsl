#version 330 core

in vec3 fragPos;
in vec2 text;

uniform sampler2D aTexture;
out vec4 fragColor;


void main()
{
    vec4 texColor = texture(aTexture, text);
    if (texColor.a < 0.1)
        discard;

    fragColor = texColor;
}