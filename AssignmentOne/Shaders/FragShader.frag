#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"

in vec3 fragColor;

// Output data
out vec4 color;

void main()
{
	color = vec4(fragColor.x, fragColor.y, fragColor.z, 1.0f);
}
