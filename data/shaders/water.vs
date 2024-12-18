#version 330 core
layout (location = 0) in vec3 point;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 pointColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 lightPos;

void main()
{
	// note that we read the multiplication from right to left
	gl_Position = proj * view * model * vec4(point, 1.0);
	// vec3 lightDir = normalize(point - lightPos);
	// float nb = max(0, dot(lightDir, normal));
	// pointColor = color * nb;
	pointColor = color;
}
