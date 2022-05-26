#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"

in vec3 fragColor;

// Output data
out vec3 color;

void main()
{
	color = fragColor;
}
