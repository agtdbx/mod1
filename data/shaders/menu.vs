#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 aTexCoord;

out vec3 pointColor;
out vec2 TexCoord;

void main()
{
	// note that we read the multiplication from right to left
	gl_Position = vec4(position, 0.0, 1.0);
	pointColor = color;
    TexCoord = aTexCoord;
}
