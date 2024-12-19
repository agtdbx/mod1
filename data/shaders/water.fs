#version 330 core

out vec4 FragColor;
in vec4 pointColor;

void main()
{
	FragColor = pointColor;
}
