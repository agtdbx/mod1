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
uniform vec3			lightColor;
uniform float			waterDensity;

uniform int				mapBufferCellSize;
uniform float			invMapBufferCellSize;
uniform int				mapBufferW;
uniform int				mapBufferH;
uniform int				mapBufferD;
uniform int				mapBufferIdHsize;
uniform samplerBuffer	mapDensitiesBuffer;

uniform bool			holeEnable;
uniform float			holeRadius2;
uniform vec2			holePosition;
uniform vec3			clearColor;

uniform int				terrainCellSize;
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

vec3	p_luf = vec3(0.0, waterMaxY, 0.0);
vec3	p_ruf = vec3(waterMaxXZ, waterMaxY, 0.0);
vec3	p_ldf = vec3(0.0, 0.0, 0.0);
vec3	p_rdf = vec3(waterMaxXZ, 0.0, 0.0);
vec3	p_lub = vec3(0.0, waterMaxY, waterMaxXZ);
vec3	p_rub = vec3(waterMaxXZ, waterMaxY, waterMaxXZ);
vec3	p_ldb = vec3(0.0, 0.0, waterMaxXZ);
vec3	p_rdb = vec3(waterMaxXZ, 0.0, waterMaxXZ);

s_inter_map_info	hitMap(vec3 rayPos, vec3 rayDir)
{
	s_inter_map_info	result;
	return (intersectWithCube(
				rayPos, rayDir,
				p_luf, p_ruf, p_ldf, p_rdf,
				p_lub, p_rub, p_ldb, p_rdb));
}


float	hitTriangle(
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
	if (det > -0.000001 && det < 0.000001)
		return (-1.0);

	inv_det = 1.0 / det;
	s = rayPos - p1;
	u = inv_det * dot(s, ray_cross_e2);
	if (u < 0.0 || u > 1.0)
		return (-1.0);

	s_cross_e1 = cross(s, edge_1);
	v = inv_det * dot(rayDir, s_cross_e1);
	if (v < 0.0 || v + u > 1.0)
		return (-1.0);

	return (inv_det * dot(edge_2, s_cross_e1));
}


// Terrain distance
float	checkIntersectionWithTerrain(
			vec3 rayPos, vec3 rayDir, int gx, int gy, int gz)
{
	int		gid, startId, endId, rectangleId;
	vec3	position, vecX, vecZ, vecXZ, normal, dirToRect,
			interPos, interLocalPos;
	vec3	pul, pur, pdl, pdr;
	float	denom, dist1, dist2, interLocalx, interLocalz;

	if (gx < 0 || gx >= terrainGridW)
		return (-1.0);
	if (gy < 0 || gy >= terrainGridH)
		return (-1.0);
	if (gz < 0 || gz >= terrainGridD)
		return (-1.0);

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

		dist1 = hitTriangle(rayPos, rayDir, pul, pur, pdl);
		dist2 = hitTriangle(rayPos, rayDir, pdl, pur, pdr);
		if (dist1 == -1.0 && dist2 == -1.0)
			continue;

		if (dist1 == -1.0)
			return (dist2);
		if (dist2 == -1.0)
			return (dist1);
		if (dist1 < dist2)
			return (dist1);
		return (dist2);
	}

	return (-1.0);
}


float hitTriangleTerrain(vec3 rayPos, vec3 rayDir, float startDist, float maxDist)
{
	int		gx, gy, gz;
	float	gpx, gpy, gpz, dx, dy, dz, dist, distX, distY, distZ, totalDist;

	float gridMaxX = terrainGridW * terrainCellSize;
	float gridMaxY = terrainGridH * terrainCellSize;
	float gridMaxZ = terrainGridD * terrainCellSize;
	float precisionLimit = 0.1;

	rayPos += rayDir;

	totalDist = startDist;
	while (totalDist < maxDist)
	{
		// Calculate grid pos
		gx = int(rayPos.x / terrainCellSize);
		gy = int(rayPos.y / terrainCellSize);
		gz = int(rayPos.z / terrainCellSize);

		// Check intersection with triangles
		dist = checkIntersectionWithTerrain(rayPos, rayDir, gx, gy, gz);
		if (dist != -1.0)
			return (totalDist + dist);

		// Get real world coordonnate of grid
		gpx = gx * terrainCellSize;
		gpy = gy * terrainCellSize;
		gpz = gz * terrainCellSize;

		// Compute axis differences
		dx = 1000.0;
		if (rayDir.x < 0.0)
			dx = rayPos.x - gpx;
		else if (rayDir.x > 0.0)
			dx = -(rayPos.x - (gpx + terrainCellSize)) - precisionLimit;

		dy = 1000.0;
		if (rayDir.y < 0.0)
			dy = rayPos.y - gpy;
		else if (rayDir.y > 0.0)
			dy = -(rayPos.y - (gpy + terrainCellSize)) - precisionLimit;

		dz = 1000.0;
		if (rayDir.z < 0.0)
			dz = rayPos.z - gpz;
		else if (rayDir.z > 0.0)
			dz = -(rayPos.z - (gpz + terrainCellSize)) - precisionLimit;

		// Compute distances for each axis
		distX = 1000.0;
		if (dx != 1000.0)
			distX = dx / abs(rayDir.x);

		distY = 1000.0;
		if (dy != 1000.0)
			distY = dy / abs(rayDir.y);

		distZ = 1000.0;
		if (dz != 1000.0)
			distZ = dz / abs(rayDir.z);

		// Get closer dist
		dist = min(distX, min(distY, distZ));
		if (dist < precisionLimit)
			dist = precisionLimit;

		// Advance of a new step
		rayPos += rayDir * dist;
		totalDist += dist;
	}

	return (cameraFar);
}


// Get density
float	getDensityAtMapPoint(int x, int y, int z)
{
	if (x >= mapBufferW || y >= mapBufferH || z >= mapBufferD)
		return (0.0);

	int	tid = x + z * mapBufferW + y * mapBufferIdHsize;
	float density = texelFetch(mapDensitiesBuffer, tid).r;
	return (density);
}


float	lerp(float start, float end, float ratio)
{
	return (start + (end - start) * ratio);
}


float	getDensityAtPos(vec3 pos, float densityValue)
{
	int		gx, gy, gz, ngx, ngy, ngz;
	float	densityFUL, densityFUR, densityFDL, densityFDR,
			densityBUL, densityBUR, densityBDL, densityBDR,
			densityFU, densityFD, densityBU, densityBD,
			densityF, densityB,
			density,
			dx, dy, dz;

	// Get grid coordonates
	gx = int(pos.x * invMapBufferCellSize);
	gy = int(pos.y * invMapBufferCellSize);
	gz = int(pos.z * invMapBufferCellSize);
	ngx = gx + 1;
	ngy = gy + 1;
	ngz = gz + 1;

	// Get ratio for each axis
	dx = (pos.x - (gx * mapBufferCellSize)) * invMapBufferCellSize;
	dy = (pos.y - (gy * mapBufferCellSize)) * invMapBufferCellSize;
	dz = (pos.z - (gz * mapBufferCellSize)) * invMapBufferCellSize;

	// Get density for 8 cell around pos
	densityFUL = getDensityAtMapPoint(gx,  gy,  gz);
	densityFUR = getDensityAtMapPoint(ngx, gy,  gz);
	densityFDL = getDensityAtMapPoint(gx,  ngy, gz);
	densityFDR = getDensityAtMapPoint(ngx, ngy, gz);
	densityBUL = getDensityAtMapPoint(gx,  gy,  ngz);
	densityBUR = getDensityAtMapPoint(ngx, gy,  ngz);
	densityBDL = getDensityAtMapPoint(gx,  ngy, ngz);
	densityBDR = getDensityAtMapPoint(ngx, ngy, ngz);

	// Merge density on x axis
	densityFU = lerp(densityFUL, densityFUR, dx);
	densityFD = lerp(densityFDL, densityFDR, dx);
	densityBU = lerp(densityBUL, densityBUR, dx);
	densityBD = lerp(densityBDL, densityBDR, dx);

	// Merge density on y axis
	densityF = lerp(densityFU, densityFD, dy);
	densityB = lerp(densityBU, densityBD, dy);

	// Merge density on z axis
	density = lerp(densityF, densityB, dz);
	if (density > waterDensity)
		return (densityValue);
	return (0.0);
}

float	maxDensity = 10.0;
float	invMaxDensity = 1.0 / maxDensity;
float	quickRaySize = 50.0;
float	quickRayStep = rayStep * quickRaySize;

float	getDensityToLight(vec3 rayPos)
{
	float	dstToLight, dst, maxDst, densityAlongRay;
	vec3	rayToLight, rayDir;
	int		fullResRay = 5;

	// Get new ray direction
	rayToLight = lightPos - rayPos;
	dstToLight = vecLength(rayToLight);
	if (dstToLight == 0.0)
		return (0.0);
	rayDir = rayToLight / dstToLight;

	// Get distance to top bbox face
	dst = intersectWithCubeFace(rayPos, rayDir, normalUp,
								p_lub, p_rub,
								p_luf, p_ruf);
	if (dst == -1.0)
		return (0.0);

	maxDst = max(dst, dstToLight);
	dst = 0;
	densityAlongRay = 0;
	fullResRay = 5;
	// Full resolution ray
	while (dst < maxDst && fullResRay > 0)
	{
		fullResRay--;
		densityAlongRay += getDensityAtPos(rayPos, rayStep);
		if (densityAlongRay >= maxDensity)
			return (maxDensity);
		rayPos += rayDir * rayStep;
		dst += rayStep;

	}
	// Quick ray
	while (dst < maxDst)
	{
		densityAlongRay += getDensityAtPos(rayPos, quickRayStep);
		if (densityAlongRay >= maxDensity)
			return (maxDensity);
		rayPos += rayDir * quickRayStep;
		dst += quickRayStep;
	}

	return (densityAlongRay);
}


vec4	getPixelColor(vec3 rayPos, vec3 rayDir)
{
	s_inter_map_info	hitMapInfo;
	float	dist, distGround, distTriangle, distTerrain, maxDist,
			densityAlongRay;

	hitMapInfo = hitMap(rayPos, rayDir);
	if (!hitMapInfo.inter_map)
		return (vec4(0.0, 0.0, 0.0, 0.0));

	dist = hitMapInfo.dst_map_min;
	rayPos += rayDir * dist;
	distGround = hitMapInfo.dst_to_ground;
	maxDist = hitMapInfo.dst_map_max;
	distTriangle = hitTriangleTerrain(rayPos, rayDir, dist,
										min(distGround, maxDist));

	distTerrain = min(distGround, distTriangle);

	densityAlongRay = 0.0;
	float	density, densityToLight, dstToHole2, ratio;
	vec2	posForHole, dirToHole;
	vec3	color;
	vec3	colorOnLight = lightColor;
	vec3	colorDiff = waterColor - colorOnLight;
	vec4	pixelColor = vec4(0, 0, 0, 0);
	vec4	holeColor = vec4(clearColor, 1.0);

	while (dist <= maxDist)
	{
		density = getDensityAtPos(rayPos, rayStep);
		if (density > 0)
		{
			densityToLight = getDensityToLight(rayPos);

			if (densityToLight != 0.0)
			{
				densityToLight *= invMaxDensity;
				color = colorOnLight + colorDiff * densityToLight;
			}
			else
				color = waterColor;

			pixelColor += vec4(color, 0.8) * (density * invMaxDensity);
		}

		densityAlongRay += density;
		if (densityAlongRay > maxDensity)
		{
			densityAlongRay = maxDensity;
			break;
		}

		// Check if ray collide with terrain
		if (dist >= distTerrain)
			break;

		rayPos += rayDir * rayStep;
		dist += rayStep;
	}

	// if hole enable and the ray hit the ground
	if (holeEnable && rayPos.y <= 0.1 && densityAlongRay != maxDensity)
	{
		posForHole = vec2(rayPos.x, rayPos.z);
		dirToHole = holePosition - posForHole;
		dstToHole2 = dirToHole.x * dirToHole.x + dirToHole.y * dirToHole.y;
		if (dstToHole2 <= holeRadius2)
		{
			ratio = pixelColor.a / 0.8;
			pixelColor = holeColor + (pixelColor - holeColor) * ratio;
		}
	}

	return (pixelColor);
}


void main()
{
	vec3	rayPos = cameraPos - (cameraRight * planeWidth * pointPos.x)
								+ (cameraUp * planeHeight * pointPos.y);
	vec3	fakeCameraPos = cameraPos - cameraFront * 2;
	vec3	rayDir = normalize(rayPos - fakeCameraPos);

	FragColor = getPixelColor(rayPos, rayDir);
}
