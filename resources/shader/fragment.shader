#version 330 core

in vec2 textureCord;
out vec4 outcolor;

uniform sampler2D ourTexture;

void main()
{
	outcolor = texture(ourTexture, textureCord);
};
