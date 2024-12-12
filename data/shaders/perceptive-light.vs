#version 330 core
layout (location = 0) in vec3 posPoint;
layout (location = 1) in vec3 normal;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 cameraPos;

void main()
{
	// note that we read the multiplication from right to left
	gl_Position = proj * view * model * vec4(posPoint, 1.0);
	vec3 cameraDir = normalize(posPoint - cameraPos);
	float nb = dot(cameraDir, normal);
	if (nb > 0)
	{
		float c = posPoint.y / 210.0;
		ourColor = vec3(c, 1.0, c) * nb;
	}
	else
		ourColor = vec3(0.0, 0.0, 0.0);
}
