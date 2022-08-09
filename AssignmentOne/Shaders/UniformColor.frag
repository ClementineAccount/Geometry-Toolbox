#version 410 core

in vec4 fragColor;
in float fragAlpha;

// Output data
out vec4 color;

void main()
{
	color = fragColor;
}
