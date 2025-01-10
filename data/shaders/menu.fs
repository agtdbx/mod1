#version 420 core

out vec4 FragColor;

in vec3 pointColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	vec4 textureColor = texture(texture1, TexCoord).rgba;

	if ( textureColor.a == 0.0)
		textureColor = vec4(pointColor, 1.0);
	FragColor = textureColor;
}
