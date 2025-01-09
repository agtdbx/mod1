#version 430 core
layout (location = 0) in vec2 point;

out vec2 pointPos;

void main()
{
	gl_Position = vec4(point, 0.0, 1.0);
	pointPos = point;
}
