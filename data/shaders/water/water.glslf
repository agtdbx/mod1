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
uniform float			waterRadius;
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
uniform int				terrainNbRectangles;
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
	float	dst_map_min;
	float	dst_map_max;
	float	dst_to_ground;
};

float	vecLength(vec3 vec)
{
	return (sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

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

	// Intersection with face normal
	denom = dot(rayDir, faceNormal);
	if (denom < 0.000001)
	{
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

	// Intersection with inverse face normal
	denom = -denom;
	if (denom < 0.000001)
	{
		faceNormal *= -1.0;
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

	return (-1.0);
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
	result.dst_map_min = cameraFar;
	result.dst_map_max = 0.0;
	result.dst_to_ground = cameraFar + 1.0;

	// Check if ray is in the cube
	if (facePoint_ldf.x <= rayPos.x && rayPos.x <= facePoint_rub.x
		&& facePoint_ldf.y <= rayPos.y && rayPos.y <= facePoint_rub.y
		&& facePoint_ldf.z <= rayPos.z && rayPos.z <= facePoint_rub.z)
	{
		result.inter_map = true;
		result.dst_map_min = 0.0;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalRight,
								facePoint_ruf, facePoint_rub,
								facePoint_rdf, facePoint_rdb);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalLeft,
								facePoint_lub, facePoint_luf,
								facePoint_ldb, facePoint_ldf);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalUp,
								facePoint_lub, facePoint_rub,
								facePoint_luf, facePoint_ruf);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalDown,
								facePoint_ldf, facePoint_rdf,
								facePoint_ldb, facePoint_rdb);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
		result.dst_to_ground = dst;
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalFront,
								facePoint_luf, facePoint_ruf,
								facePoint_ldf, facePoint_rdf);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
	}

	dst = intersectWithCubeFace(rayPos, rayDir, normalBack,
								facePoint_rub, facePoint_lub,
								facePoint_rdb, facePoint_ldb);
	if (dst != -1.0)
	{
		result.inter_map = true;
		result.dst_map_min = min(result.dst_map_min, dst);
		result.dst_map_max = max(result.dst_map_max, dst + 1.0);
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


bool	hitTriangle(
			vec3 rayPos, vec3 rayDir, vec3 p1, vec3 p2, vec3 p3)
{
	vec3	edge_1, edge_2, normal, ray_cross_e2, s, s_cross_e1;
	float	det, inv_det, u, v;

	edge_1 = p2 - p1;
	edge_2 = p3 - p1;

	normal = vec3((edge_1.y * edge_2.z) - (edge_1.z * edge_2.y),
					(edge_1.z * edge_2.x) - (edge_1.x * edge_2.z),
					(edge_1.x * edge_2.y) - (edge_1.y * edge_2.x));

	ray_cross_e2 = cross(rayDir, edge_2);
	det = dot(edge_1, ray_cross_e2);
	if (det > -0.000001)
		return (false);

	inv_det = 1.0 / det;
	s = rayPos - p1;
	u = inv_det * dot(s, ray_cross_e2);
	if (u < 0.0 || u > 1.0)
		return (false);

	s_cross_e1 = cross(s, edge_1);
	v = inv_det * dot(rayDir, s_cross_e1);
	if (v < 0.0 || v + u > 1.0)
		return (false);

	return (true);
}


// Ray marching
bool	checkIntersectionWithTerrain(
			vec3 rayPos, vec3 rayDir, int gx, int gy, int gz)
{
	int		gid, startId, endId, rectangleId;
	vec3	position, vecX, vecZ, vecXZ, normal, dirToRect,
			interPos, interLocalPos;
	vec3	pul, pur, pdl, pdr;
	float	denom, dist, interLocalx, interLocalz;

	if (gx < 0 || gx >= terrainGridW)
		return (false);
	if (gy < 0 || gy >= terrainGridH)
		return (false);
	if (gz < 0 || gz >= terrainGridD)
		return (false);

	gid = gx + gz * terrainGridW + gy * terrainIdHsize;
		startId = int(texelFetch(terrainOffsetsBuffer, gid).r);
	if (gid + 1 < terrainOffsetsSize)
		endId = int(texelFetch(terrainOffsetsBuffer, gid + 1).r);
	else
		endId = terrainGridSize;

	for (int i = startId; i < endId; i++)
	{
		rectangleId = int(texelFetch(terrainGridBuffer, i).r);

		position = texelFetch(terrainDataBuffer, rectangleId * 5).rgb;
		vecX = texelFetch(terrainDataBuffer, rectangleId * 5 + 1).rgb;
		vecZ = texelFetch(terrainDataBuffer, rectangleId * 5 + 2).rgb;
		vecXZ = texelFetch(terrainDataBuffer, rectangleId * 5 + 3).rgb;
		normal = texelFetch(terrainDataBuffer, rectangleId * 5 + 4).rgb;

		pul = position;
		pur = position + vecX;
		pdl = position + vecZ;
		pdr = position + vecXZ;

		// if (!hitTriangle(rayPos, rayDir, pul, pur, pdl))
		// if (!hitTriangle(rayPos, rayDir, pdl, pur, pdr))
		if (!hitTriangle(rayPos, rayDir, pul, pur, pdl)
			&& !hitTriangle(rayPos, rayDir, pdl, pur, pdr))
			continue;

		return (true);
	}


	return (false);
}


float interWithAllTriangles(vec3 rayPos, vec3 rayDir)
{
	float	dst = cameraFar + 2;
	vec3	position, vecX, vecZ, vecXZ;
	vec3	pul, pur, pdl, pdr;

	for (int i = 0; i < terrainGridSize; i++)
	{
		int rectangleId = int(texelFetch(terrainGridBuffer, i).r);

		position = texelFetch(terrainDataBuffer, rectangleId * 5).rgb;
		vecX = texelFetch(terrainDataBuffer, rectangleId * 5 + 1).rgb;
		vecZ = texelFetch(terrainDataBuffer, rectangleId * 5 + 2).rgb;
		vecXZ = texelFetch(terrainDataBuffer, rectangleId * 5 + 3).rgb;

		pul = position;
		pur = position + vecX;
		pdl = position + vecZ;
		pdr = position + vecXZ;

		// if (hitTriangle(rayPos, rayDir, pul, pur, pdl))
		// if (hitTriangle(rayPos, rayDir, pdl, pur, pdr))
		if (hitTriangle(rayPos, rayDir, pul, pur, pdl)
			|| hitTriangle(rayPos, rayDir, pdl, pur, pdr))
			return (0.0);
	}

	return (dst);
}


int	hitTerrain(vec3 rayPos, vec3 rayDir)
{
	s_inter_map_info	hitMapInfo;
	int	gx, gy, gz, ngx, ngy, ngz;
	float	dist, distGround, maxDist;

	hitMapInfo = hitMap(rayPos, rayDir);
	if (!hitMapInfo.inter_map)
		return (-1);

	dist = hitMapInfo.dst_map_min;
	rayPos += rayDir * dist;
	distGround = hitMapInfo.dst_to_ground;
	maxDist = hitMapInfo.dst_map_max;

	// float triDst = interWithAllTriangles(rayPos, rayDir);
	// if (triDst < maxDist)
	// 	return (2);

	gx = -1;
	gy = -1;
	gz = -1;

	while (dist <= maxDist)
	{
		// Check if ray collide with map bottom
		if (dist >= distGround)
			return (1);

		ngx = int(rayPos.x / terrainCellSize);
		ngy = int(rayPos.y / terrainCellSize);
		ngz = int(rayPos.z / terrainCellSize);
		if (ngx != gx || ngy != gy || ngz != gz)
		{
			gx = ngx;
			gy = ngy;
			gz = ngz;
			if (checkIntersectionWithTerrain(rayPos, rayDir, gx, gy, gz))
				return (2);
		}

		rayPos += rayDir * rayStep;
		dist += rayStep;
	}

	return (-2);
}


void main()
{
	vec3	rayPos = cameraPos - (cameraRight * planeWidth * pointPos.x) + (cameraUp * planeHeight * pointPos.y);
	vec3	fakeCameraPos = cameraPos - cameraFront * 2;
	vec3	rayDir = normalize(rayPos - fakeCameraPos);

	// The default color to transparent
	int res = hitTerrain(rayPos, rayDir);
	// Non hit case
	if (res == -1)
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else if (res == -2)
		FragColor = vec4(0.5, 0.0, 0.0, 1.0);

	// Hit case
	else if (res == 1)
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	else if (res == 2)
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);

	// WTF case
	else
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
