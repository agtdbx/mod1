#version 420 core
layout (location = 0) in vec2 position;
// layout (location = 1) in vec3 color;

out vec3 pointColor;

void main()
{
	// note that we read the multiplication from right to left
	gl_Position = vec4(position, 0.0, 1.0);
	// pointColor = color;
	pointColor = vec3(1.0, 0.0, 0.0);
}
