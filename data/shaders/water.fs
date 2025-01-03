#version 420 core

out vec4		FragColor;
in vec2			pointPos;

uniform vec3	lightPos;
uniform vec3	cameraPos;
uniform vec3	cameraFront;
uniform vec3	cameraRight;
uniform vec3	cameraUp;
uniform float	cameraFar;
uniform float	planeWidth;
uniform float	planeHeight;
uniform float	waterRadius2;
uniform vec3	waterColor;
uniform int		nbPositions;
uniform samplerBuffer positionsBuffer;
// uniform samplerBuffer gridBuffer;
// uniform samplerBuffer offsetsBuffer;


struct s_intersection_info
{
	float	dst;
	int		id;
};


s_intersection_info	getIntersectionPointWithWater(vec3 rayPos, vec3 rayDir)
{
	s_intersection_info	intersectionInfo;
	vec3	vec, pos;
	float	dotRes;
	float	nabla;
	float	dst;

	intersectionInfo.dst = cameraFar;
	intersectionInfo.id = -1;

	for (int i = 0; i < nbPositions; i++)
	{
		pos = texelFetch(positionsBuffer, i).rgb;
		vec = rayPos - pos;
		dotRes = dot(rayDir, vec);
		nabla = (dotRes * dotRes) - dot(vec, vec) + waterRadius2;

		if (nabla < 0.0)
			continue;

		nabla = sqrt(nabla);
		dst = -dotRes - nabla;

		if (dst < 0.0)
			dst = -dotRes + nabla;

		if (dst < 0.0)
			continue;

		if (dst < intersectionInfo.dst)
		{
			intersectionInfo.dst = dst;
			intersectionInfo.id = i;
		}
	}
	return (intersectionInfo);
}

void main()
{
	vec3	rayPos = cameraPos - (cameraRight * planeWidth * pointPos.x) + (cameraUp * planeHeight * pointPos.y);
	vec3	fakeCameraPos = cameraPos - cameraFront * 2;
	vec3	rayDir = normalize(rayPos - fakeCameraPos);

	float totalDist = 0.0;
	s_intersection_info intersectionInfo;
	int id = 0;

	intersectionInfo = getIntersectionPointWithWater(rayPos, rayDir);

	if (intersectionInfo.id == -1)
	{
		// The default color to transparent
		FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
	else
	{
		vec3 hitPoint = rayPos + rayDir * intersectionInfo.dst;
		vec3 pos = texelFetch(positionsBuffer, intersectionInfo.id).rgb;
		vec3 normal = normalize(hitPoint - pos);
		vec3 lightDir = normalize(lightPos - pos);
		float colorRatio = dot(normal, lightDir);

		FragColor = vec4(waterColor * colorRatio, 1.0);
	}
}
