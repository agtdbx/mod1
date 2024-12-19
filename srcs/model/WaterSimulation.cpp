#include <model/WaterSimulation.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	this->mesh = obj.mesh;
	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->velocities = obj.velocities;
}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

int	WaterSimulation::getNbParticules(void)
{
	return (this->nbParticules);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

WaterSimulation	&WaterSimulation::operator=(const WaterSimulation &obj)
{
	if (this == &obj)
		return (*this);

	this->mesh = obj.mesh;
	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->velocities = obj.velocities;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(Vec3 position)
{
	this->positions.push_back(position);
	this->velocities.push_back(Vec3(0, 0, 0));
	this->nbParticules++;
}


void	WaterSimulation::tick(double delta)
{
	for (int i = 0; i < this->nbParticules; i++)
	{
		// Apply gravity
		// this->velocities[i] += Vec3(0, -0.1, 0) * delta;

		// Update particule position
		this->positions[i] += this->velocities[i] * delta;

		// Check if particule is out of the map on x axis
		if (this->positions[i].x < 0.0)
		{
			this->positions[i].x = 0.0;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].x >= MAP_SIZE)
		{
			this->positions[i].x = MAP_SIZE;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		// Check if particule is out of the map on y axis
		if (this->positions[i].y < 0.0)
		{
			this->positions[i].y = 0.0;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].y >= WATER_MAX_HEIGHT)
		{
			this->positions[i].y = MAP_SIZE;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		// Check if particule is out of the map on z axis
		if (this->positions[i].z < 0.0)
		{
			this->positions[i].z = 0.0;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].z >= MAP_SIZE)
		{
			this->positions[i].z = MAP_SIZE;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
	}

	this->createMesh();
}


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager)
{
	this->mesh.draw(camera, shaderManager->getShader("water"),
					shaderManager->getVAOId());
}

//**** PRIVATE METHODS *********************************************************

static inline int	getHashId(int x, int y, int z)
{
	return (x | (y << 8) | (z << 16));
}

static bool	isPoint(
				std::unordered_map<int, bool> &waterInfluenceMap,
				int posId)
{
	return (waterInfluenceMap.find(posId) != waterInfluenceMap.end());
}

static bool	isPoint(
				std::unordered_map<int, t_water_grid_pos> &waterInfluenceMap,
				int x, int y, int z)
{
	return (waterInfluenceMap.find(getHashId(x, y, z)) != waterInfluenceMap.end());
}

#include <ctime> // TODO: REMOVE
typedef struct s_timetest
{
	int				nbCall;
	int				totalTime;
	std::clock_t	start;
}	t_timetest;

void	printTimeTest(const char *str, t_timetest &timetest)
{
	float	timePerCall = 0.0f;

	if (timetest.totalTime > 0)
		timePerCall = (float)timetest.totalTime / (float)timetest.nbCall;

	printf("%s :\n", str);
	printf(" - nb call : %i ms\n", timetest.nbCall);
	printf(" - total time : %i ms\n", timetest.totalTime);
	printf(" - time per call : %.3f ms\n", timePerCall);
	printf("\n");
}

void	computeTimeTook(t_timetest &timetest)
{
	timetest.totalTime += ((double)(std::clock() - timetest.start) / CLOCKS_PER_SEC) * 1000000;
	timetest.nbCall++;
}

void	WaterSimulation::createMesh(void)
{
	std::unordered_map<int, t_water_grid_pos>	waterInfluenceMap;
	std::unordered_map<int, bool>	pointAlreadyCreate;
	int						wx, wy, wz, nbPoints, id, posId;
	unsigned int			p1, p2, p3;
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;
	Vec3					normal, A, B;
	// TODO: REMOVE
	// t_timetest	tCreateWmap, tCreateTriangles, tCreateMesh,
	// 			tCheckDuplicatePoint, tCheckCubeCorner,
	// 			tAddVertices, tAddIndices;

	// tCreateWmap.nbCall = 0; tCreateWmap.totalTime = 0;
	// tCreateTriangles.nbCall = 0; tCreateTriangles.totalTime = 0;
	// tCreateMesh.nbCall = 0; tCreateMesh.totalTime = 0;
	// tCheckDuplicatePoint.nbCall = 0; tCheckDuplicatePoint.totalTime = 0;
	// tCheckCubeCorner.nbCall = 0; tCheckCubeCorner.totalTime = 0;
	// tAddVertices.nbCall = 0; tAddVertices.totalTime = 0;
	// tAddIndices.nbCall = 0; tAddIndices.totalTime = 0;

	// printf("\n\n%i water particles\n\n", this->nbParticules);

	// Update water influence by water pos
	for (int i = 0; i < this->nbParticules; i++)
	{
		// tCreateWmap.start = std::clock();
		wx = this->positions[i].x + 0.5;
		wy = this->positions[i].y + 0.5;
		wz = this->positions[i].z + 0.5;

		waterInfluenceMap[getHashId(wx, wy, wz)] = (t_water_grid_pos){wx, wy, wz};
		// computeTimeTook(tCreateWmap);
	}
	// printTimeTest("Create water influence map", tCreateWmap);

	// Create points and triangles
	nbPoints = 0;

	std::unordered_map<int, t_water_grid_pos>::const_iterator it;

	it = waterInfluenceMap.begin();

	const int	lookAround[8][3] = {
		{ 0,  0,  0},
		{ 0,  0, -1},
		{ 0, -1,  0},
		{ 0, -1, -1},
		{-1,  0,  0},
		{-1,  0, -1},
		{-1, -1,  0},
		{-1, -1, -1}
	};

	int x, y, z;
	while (it != waterInfluenceMap.end())
	{
		// tCreateTriangles.start = std::clock();
		wx = it->second.x;
		wy = it->second.y;
		wz = it->second.z;

		for (int j = 0; j < 8; j++)
		{
			x = wx + lookAround[j][0];
			y = wy + lookAround[j][1];
			z = wz + lookAround[j][2];
			id = 0;

			// tCheckDuplicatePoint.start = std::clock();
			posId = getHashId(x, y, z);
			bool test = isPoint(pointAlreadyCreate, posId);
			// computeTimeTook(tCheckDuplicatePoint);
			if (test)
				continue;
			pointAlreadyCreate[posId] = true;

			// tCheckCubeCorner.start = std::clock();
			if (isPoint(waterInfluenceMap, x, y + 1, z + 1))
				id++; // 6
			id <<= 1;
			if (isPoint(waterInfluenceMap, x + 1, y + 1, z + 1))
				id++; // 7
			id <<= 1;
			if (isPoint(waterInfluenceMap, x + 1, y, z + 1))
				id++; // 5
			id <<= 1;
			if (isPoint(waterInfluenceMap, x, y, z + 1))
				id++; // 4
			id <<= 1;
			if (isPoint(waterInfluenceMap, x, y + 1, z))
				id++; // 2
			id <<= 1;
			if (isPoint(waterInfluenceMap, x + 1, y + 1, z))
				id++; // 3
			id <<= 1;
			if (isPoint(waterInfluenceMap, x + 1, y, z))
				id++; // 1
			id <<= 1;
			if (isPoint(waterInfluenceMap, x, y, z))
				id++; // 0
			// computeTimeTook(tCheckCubeCorner);

			if (id == 0 || id == 0b11111111)
				continue;

			// tAddVertices.start = std::clock();
			vertices.push_back(Point(Vec3(x + 0.5, y, z), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 1, y + 0.5, z), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 0.5, y + 1, z), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x, y + 0.5, z), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 0.5, y, z + 1), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 1, y + 0.5, z + 1), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 0.5, y + 1, z + 1), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x, y + 0.5, z + 1), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x, y, z + 0.5), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 1, y, z + 0.5), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x + 1, y + 1, z + 0.5), 0, 0, 0.8));
			vertices.push_back(Point(Vec3(x, y + 1, z + 0.5), 0, 0, 0.8));
			// computeTimeTook(tAddVertices);

			// tAddIndices.start = std::clock();
			for (int i = 0; i < 16 && this->trianglePoints[id][i] != -1; i += 3)
			{
				p1 = nbPoints + this->trianglePoints[id][i];
				p2 = nbPoints + this->trianglePoints[id][i + 1];
				p3 = nbPoints + this->trianglePoints[id][i + 2];

				A = vertices[p2].pos - vertices[p1].pos;
				B = vertices[p3].pos - vertices[p1].pos;
				normal = A.cross(B);
				normal.normalize();

				vertices[p1].normal = normal;
				vertices[p2].normal = normal;
				vertices[p3].normal = normal;

				indices.push_back((t_tri_id){p1, p2, p3});
			}
			// computeTimeTook(tAddIndices);
			nbPoints += 12;
		}
		it++;
		// computeTimeTook(tCreateTriangles);
	}
	// printTimeTest("Check point in map for marching cube", tCreateTriangles);
	// printTimeTest("Check duplicate point", tCheckDuplicatePoint);
	// printTimeTest("Check cube corner", tCheckCubeCorner);
	// printTimeTest("Add vertices", tAddVertices);
	// printTimeTest("Add indices", tAddIndices);

	// tCreateMesh.start = std::clock();
	this->mesh.setMesh(vertices, indices);
	// computeTimeTook(tCreateMesh);
	// printTimeTest("Create mesh", tCreateMesh);
}
