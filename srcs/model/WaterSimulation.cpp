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
		this->velocities[i] += Vec3(0, -0.1, 0) * delta;

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

static int	getHashId(int x, int y, int z)
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


void	WaterSimulation::createMesh(void)
{
	std::unordered_map<int, t_water_grid_pos>	waterInfluenceMap;
	std::unordered_map<int, bool>	pointAlreadyCreate;
	int						wx, wy, wz, nbPoints, id, posId;
	unsigned int			p1, p2, p3;
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;
	Vec3					normal, A, B;

	// Update water influence by water pos
	for (int i = 0; i < this->nbParticules; i++)
	{
		wx = this->positions[i].x + 0.5;
		wy = this->positions[i].y + 0.5;
		wz = this->positions[i].z + 0.5;

		waterInfluenceMap[getHashId(wx, wy, wz)] = (t_water_grid_pos){wx, wy, wz};
	}

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
		wx = it->second.x;
		wy = it->second.y;
		wz = it->second.z;

		for (int j = 0; j < 8; j++)
		{
			x = wx + lookAround[j][0];
			y = wy + lookAround[j][1];
			z = wz + lookAround[j][2];
			id = 0;

			posId = getHashId(x, y, z);
			if (isPoint(pointAlreadyCreate, posId))
				continue;
			pointAlreadyCreate[posId] = true;

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

			if (id == 0 || id == 0b11111111)
				continue;

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
			nbPoints += 12;
		}


		it++;
	}

	this->mesh.setMesh(vertices, indices);
}
