#version 430 core

out vec4		FragColor;
in vec2			pointPos;

uniform vec3			lightPos;
uniform vec3			cameraPos;
uniform vec3			cameraFront;
uniform vec3			cameraRight;
uniform vec3			cameraUp;
uniform float			cameraFar;
uniform float			planeWidth;
uniform float			planeHeight;
uniform float			waterRadius2;
uniform vec3			waterColor;
uniform float			renderCellSize;
uniform int				nbPositions;
uniform samplerBuffer	positionsBuffer;
uniform int				mapSize;
uniform int				mapHeight;
uniform int				renderGridW;
uniform int				renderGridH;
uniform int				renderGridD;
uniform int				renderGridSize;
uniform int				renderOffsetsSize;
uniform samplerBuffer	renderGridBuffer;
uniform samplerBuffer	renderOffsetsBuffer;

vec3	normalRight =	vec3( 1.0,  0.0,  0.0);
vec3	normalLeft =	vec3(-1.0,  0.0,  0.0);
vec3	normalUp =		vec3( 0.0,  1.0,  0.0);
vec3	normalDown =	vec3( 0.0, -1.0,  0.0);
vec3	normalFront =	vec3( 0.0,  0.0, -1.0);
vec3	normalBack =	vec3( 0.0,  0.0,  1.0);


struct s_intersection_info
{
	float	dst;
	int		id;
};


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


s_intersection_info	getIntersectionPointWithWater(vec3 rayPos, vec3 rayDir)
{
	s_intersection_info	intersectionInfo;
	vec3	vec, pos;
	vec3	p_luf, p_ruf, p_ldf, p_rdf, p_lub, p_rub, p_ldb, p_rdb;
	float	dotRes, nabla, dst;
	int		offsetId, startId, endId, waterId;
	int		Hsize = renderGridW * renderGridD;

	intersectionInfo.dst = cameraFar;
	intersectionInfo.id = -1;

	p_luf = vec3(0.0, mapHeight, 0.0);
	p_ruf = vec3(mapSize, mapHeight, 0.0);
	p_ldf = vec3(0.0, 0.0, 0.0);
	p_rdf = vec3(mapSize, 0.0, 0.0);
	p_lub = vec3(0.0, mapHeight, mapSize);
	p_rub = vec3(mapSize, mapHeight, mapSize);
	p_ldb = vec3(0.0, 0.0, mapSize);
	p_rdb = vec3(mapSize, 0.0, mapSize);
	if (!intersectWithCube(rayPos, rayDir,
							p_luf, p_ruf, p_ldf, p_rdf,
							p_lub, p_rub, p_ldb, p_rdb))
		return (intersectionInfo);

	for (int cx = 0; cx < renderGridW; cx++)
	{
		for (int cy = 0; cy < renderGridH; cy++)
		{
			for (int cz = 0; cz < renderGridD; cz++)
			{
				p_luf = vec3(cx,     cy + 1, cz    ) * renderCellSize;
				p_ruf = vec3(cx + 1, cy + 1, cz    ) * renderCellSize;
				p_ldf = vec3(cx,     cy,     cz    ) * renderCellSize;
				p_rdf = vec3(cx + 1, cy,     cz    ) * renderCellSize;
				p_lub = vec3(cx,     cy + 1, cz + 1) * renderCellSize;
				p_rub = vec3(cx + 1, cy + 1, cz + 1) * renderCellSize;
				p_ldb = vec3(cx,     cy,     cz + 1) * renderCellSize;
				p_rdb = vec3(cx + 1, cy,     cz + 1) * renderCellSize;

				if (!intersectWithCube(rayPos, rayDir,
							p_luf, p_ruf, p_ldf, p_rdf,
							p_lub, p_rub, p_ldb, p_rdb))
					continue;

				offsetId = cx + cz * renderGridW + cy * Hsize;

				startId = int(texelFetch(renderOffsetsBuffer, offsetId).r);
				if (offsetId + 1 < renderOffsetsSize)
					endId = int(texelFetch(renderOffsetsBuffer, offsetId + 1).r);
				else
					endId = renderGridSize;

				for (int i = startId; i < endId; i++)
				{
					waterId = int(texelFetch(renderGridBuffer, i).r);
					pos = texelFetch(positionsBuffer, waterId).rgb;
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
						intersectionInfo.id = waterId;
					}
				}
			}
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
