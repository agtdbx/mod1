#version 330 core
layout (location = 0) in vec2 point;

out vec4 pointColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform int nbPositions;
uniform vec3 positions[1000];

const float waterRadius = 50;
const float mapW = 200.0;
const float mapH = 60.0;
const float mapD = 200.0;
const float marchingCubePrecision = 0.1;
const float maxDistToCamera = 500.0;

float	getShortestDistToWater(vec3 currentPos)
{
	float	shortestDist = maxDistToCamera;

	for (int i = 0; i < nbPositions; i++)
	{
		vec3 dir = positions[i] - currentPos;
		float dst = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z) - waterRadius;

		if (dst < marchingCubePrecision)
			return (dst);

		if (shortestDist > dst)
			shortestDist = dst;
	}
	return (shortestDist);
}

void main()
{
	gl_Position = vec4(point, 0.0, 1.0);

	vec3 rayPos = cameraPos + cameraRight * 16 * point.x + cameraUp * 9 * point.y;
	vec3 fakeCameraPos = cameraPos - cameraFront * 10;
	vec3 rayDir = normalize(rayPos - fakeCameraPos);

	float totalDist = 0.0;
	float dst = 0.0;
	// The default color to transparent
	pointColor = vec4(0.0, 0.0, 0.0, 0.0);

	while (true)
	{
		// Get the shortest distance to every sphere
		dst = getShortestDistToWater(rayPos);

		// If we hit a water sphere, draw it
		if (dst <= marchingCubePrecision)
		{
			pointColor = vec4(0.0, 0.0, 0.8, 1.0);
			break;
		}

		// If we are to far of the camera, stop the loop
		totalDist += dst;
		if (totalDist >= maxDistToCamera)
			break;

		// Update ray pos
		rayPos += rayDir * dst;
	}

}
