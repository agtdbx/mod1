#version 430 core

out vec4	FragColor;
in vec2		pointPos;

uniform vec3			lightPos;
uniform vec3			cameraPos;
uniform vec3			cameraFront;
uniform vec3			cameraRight;
uniform vec3			cameraUp;
uniform float			cameraFar;
uniform float			planeWidth;
uniform float			planeHeight;
uniform float			rayStep;
uniform float			waterMaxXZ;
uniform float			waterMaxY;
uniform vec3			waterColor;

uniform int				gridW;
uniform int				gridH;
uniform int				gridD;
uniform int				idHsize;
uniform int				gridSize;
uniform int				offsetsSize;
uniform samplerBuffer	gridBuffer;
uniform samplerBuffer	offsetsBuffer;
uniform samplerBuffer	positionsBuffer;
uniform samplerBuffer	densitiesBuffer;

uniform float			terrainCellSize;
uniform int				terrainGridW;
uniform int				terrainGridH;
uniform int				terrainGridD;
uniform int				terrainIdHsize;
uniform int				terrainGridSize;
uniform int				terrainOffsetsSize;
uniform samplerBuffer	terrainDataBuffer;
uniform samplerBuffer	terrainGridBuffer;
uniform samplerBuffer	terrainOffsetsBuffer;


// RAY INTERSION FUNCTIONS
bool	isSameSign(float a, float b)
{
	return ((a >= 0.0 && b >= 0.0) || (a < 0.0 && b < 0.0));
}

bool	intersectWithCubeFace(vec3 rayPos, vec3 rayDir, vec3 faceNormal,
								vec3 facePoint_lu, vec3 facePoint_ru,
								vec3 facePoint_ld, vec3 facePoint_rd)
{
	float	denom, dist, o1, o2, o3, o4;
	vec3	q, p1_q, p2_q, p3_q, p4_q;

	denom = dot(rayDir, faceNormal);
	if (denom >= 0.000001)
		return (false);

	dist = dot(facePoint_lu - rayPos, faceNormal) / denom;
	if (dist < 0.0)
		return (false);

	q = rayPos + rayDir * dist;

	p1_q = facePoint_lu - q;
	p2_q = facePoint_ld - q;

	o1 = dot(cross(p1_q, p2_q), faceNormal);
	if (o1 == 0.0)
		return (false);

	p3_q = facePoint_rd - q;
	o2 = dot(cross(p2_q, p3_q), faceNormal);
	if (o2 == 0.0 || !isSameSign(o1, o2))
		return (false);

	p4_q = facePoint_ru - q;
	o3 = dot(cross(p3_q, p4_q), faceNormal);
	if (o3 == 0.0 || !isSameSign(o2, o3))
		return (false);

	o4 = dot(cross(p4_q, p1_q), faceNormal);
	if (o4 == 0.0 || !isSameSign(o3, o4))
		return (false);

	return (true);
}


bool	intersectWithCube(vec3 rayPos, vec3 rayDir,
							vec3 facePoint_luf, vec3 facePoint_ruf,
							vec3 facePoint_ldf, vec3 facePoint_rdf,
							vec3 facePoint_lub, vec3 facePoint_rub,
							vec3 facePoint_ldb, vec3 facePoint_rdb)
{
	// Check if ary is in the cube
	if (facePoint_ldf.x <= rayPos.x && rayPos.x <= facePoint_rub.x
		&& facePoint_ldf.y <= rayPos.y && rayPos.y <= facePoint_rub.y
		&& facePoint_ldf.z <= rayPos.z && rayPos.z <= facePoint_rub.z)
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalRight,
								facePoint_ruf, facePoint_rub,
								facePoint_rdf, facePoint_rdb))
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalLeft,
								facePoint_lub, facePoint_luf,
								facePoint_ldb, facePoint_ldf))
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalUp,
								facePoint_lub, facePoint_rub,
								facePoint_luf, facePoint_ruf))
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalDown,
								facePoint_ldf, facePoint_rdf,
								facePoint_ldb, facePoint_rdb))
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalFront,
								facePoint_luf, facePoint_ruf,
								facePoint_ldf, facePoint_rdf))
		return (true);

	if (intersectWithCubeFace(rayPos, rayDir, normalBack,
								facePoint_rub, facePoint_lub,
								facePoint_rdb, facePoint_ldb))
		return (true);

	return (false);
}

// HIT TERRAIN
bool	hitTerrain(vec3 rayPos, vec3 rayDir)
{
	float	dist = 0.0;

	while (dist <= cameraFar)
	{
		// Check if ray collide with map bottom
		if (rayPos.x >= 0.0 && rayPos.x < waterMaxXZ
				&& rayPos.z >= 0.0 && rayPos.z < waterMaxXZ
				&& abs(rayPos.y) < 0.1)
			return (true);

		rayPos += rayDir * rayStep;
		dist += rayStep;
	}

	return (false);
}


void main()
{
	vec3	rayPos = cameraPos - (cameraRight * planeWidth * pointPos.x) + (cameraUp * planeHeight * pointPos.y);
	vec3	fakeCameraPos = cameraPos - cameraFront * 2;
	vec3	rayDir = normalize(rayPos - fakeCameraPos);

	// The default color to transparent
	if (hitTerrain(rayPos, rayDir))
		FragColor = vec4(waterColor, 1.0);
	else
		FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
