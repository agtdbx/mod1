#version 330 core

out vec4 FragColor;
in vec3 pointColor;

void main()
{
	FragColor = vec4(pointColor, 0.5);
}
