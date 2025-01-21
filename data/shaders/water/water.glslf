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
vec3	normalRight =	vec3( 1.0,  0.0,  0.0);
vec3	normalLeft =	vec3(-1.0,  0.0,  0.0);
vec3	normalUp =		vec3( 0.0,  1.0,  0.0);
vec3	normalDown =	vec3( 0.0, -1.0,  0.0);
vec3	normalFront =	vec3( 0.0,  0.0, -1.0);
vec3	normalBack =	vec3( 0.0,  0.0,  1.0);

struct s_inter_map_info
{
	bool	inter_map;
	bool	inter_ground;
	float	dst_map_min;
	float	dst_map_max;
	float	dst_to_ground;
};

bool	isSameSign(float a, float b)
{
	return ((a >= 0.0 && b >= 0.0) || (a < 0.0 && b < 0.0));
}

float	intersectWithCubeFace(
			vec3 rayPos, vec3 rayDir, vec3 faceNormal,
			vec3 facePoint_lu, vec3 facePoint_ru,
			vec3 facePoint_ld, vec3 facePoint_rd)
{
	float	denom, dist, o1, o2, o3, o4;
	vec3	q, p1_q, p2_q, p3_q, p4_q;


	denom = dot(rayDir, faceNormal);
	if (denom >= 0.000001)
		return (-1.0);

	dist = dot(facePoint_lu - rayPos, faceNormal) / denom;
	if (dist < 0.0)
		return (-1.0);

	q = rayPos + rayDir * dist;

	p1_q = facePoint_lu - q;
	p2_q = facePoint_ld - q;

	o1 = dot(cross(p1_q, p2_q), faceNormal);
	if (o1 == 0.0)
		return (-1.0);

	p3_q = facePoint_rd - q;
	o2 = dot(cross(p2_q, p3_q), faceNormal);
	if (o2 == 0.0 || !isSameSign(o1, o2))
		return (-1.0);

	p4_q = facePoint_ru - q;
	o3 = dot(cross(p3_q, p4_q), faceNormal);
	if (o3 == 0.0 || !isSameSign(o2, o3))
		return (-1.0);

	o4 = dot(cross(p4_q, p1_q), faceNormal);
	if (o4 == 0.0 || !isSameSign(o3, o4))
		return (-1.0);

	return (dist);
}


s_inter_map_info	intersectWithCube(vec3 rayPos, vec3 rayDir,
							vec3 facePoint_luf, vec3 facePoint_ruf,
							vec3 facePoint_ldf, vec3 facePoint_rdf,
							vec3 facePoint_lub, vec3 facePoint_rub,
							vec3 facePoint_ldb, vec3 facePoint_rdb)
{
	s_inter_map_info	result;
	float				dst;

	result.inter_map = false;
	result.inter_ground = false;
	result.dst_map_min = cameraFar;
	result.dst_map_max = 0.0;
	result.dst_to_ground = 0.0;

	// Check if ray is in the cube
	if (facePoint_ldf.x <= rayPos.x && rayPos.x <= facePoint_rub.x
		&& facePoint_ldf.y <= rayPos.y && rayPos.y <= facePoint_rub.y
		&& facePoint_ldf.z <= rayPos.z && rayPos.z <= facePoint_rub.z)
	{
		result.inter_map = true;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalRight,
								facePoint_ruf, facePoint_rub,
								facePoint_rdf, facePoint_rdb);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalLeft,
								facePoint_lub, facePoint_luf,
								facePoint_ldb, facePoint_ldf);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalUp,
								facePoint_lub, facePoint_rub,
								facePoint_luf, facePoint_ruf);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalDown,
								facePoint_ldf, facePoint_rdf,
								facePoint_ldb, facePoint_rdb);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		result.inter_ground = true;
		result.dst_to_ground = dst;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalFront,
								facePoint_luf, facePoint_ruf,
								facePoint_ldf, facePoint_rdf);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalBack,
								facePoint_rub, facePoint_lub,
								facePoint_rdb, facePoint_ldb);
	if (dst != -1.0f)
	{
		result.inter_map = true;
		if (result.dst_map_min > dst)
			result.dst_map_min = dst;
		if (result.dst_map_max < dst)
			result.dst_map_max = dst;
	}

	return (result);
}


s_inter_map_info	hitMap(vec3 rayPos, vec3 rayDir)
{
	s_inter_map_info	result;
	vec3				p_luf, p_ruf, p_ldf, p_rdf,
						p_lub, p_rub, p_ldb, p_rdb;

	p_luf = vec3(0.0, waterMaxY, 0.0);
	p_ruf = vec3(waterMaxXZ, waterMaxY, 0.0);
	p_ldf = vec3(0.0, 0.0, 0.0);
	p_rdf = vec3(waterMaxXZ, 0.0, 0.0);
	p_lub = vec3(0.0, waterMaxY, waterMaxXZ);
	p_rub = vec3(waterMaxXZ, waterMaxY, waterMaxXZ);
	p_ldb = vec3(0.0, 0.0, waterMaxXZ);
	p_rdb = vec3(waterMaxXZ, 0.0, waterMaxXZ);
	return (intersectWithCube(
				rayPos, rayDir,
				p_luf, p_ruf, p_ldf, p_rdf,
				p_lub, p_rub, p_ldb, p_rdb));
}


// Ray marching
bool	hitTerrain(vec3 rayPos, vec3 rayDir)
{
	s_inter_map_info	hitMapInfo;
	float	dist, distGround, maxDist;

	hitMapInfo = hitMap(rayPos, rayDir);
	if (!hitMapInfo.inter_map)
		return (false);

	dist = hitMapInfo.dst_map_min;
	if (hitMapInfo.inter_ground)
		return (true);

	maxDist = hitMapInfo.dst_map_max;

	while (dist <= maxDist)
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
