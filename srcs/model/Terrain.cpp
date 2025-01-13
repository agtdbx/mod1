#include <model/Terrain.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Terrain::Terrain(void)
{
	glGenBuffers(1, &this->textureBufferTerrainGridFlat);
	glGenTextures(1, &this->textureTerrainGridFlat);
	glGenBuffers(1, &this->textureBufferTerrainGridOffsets);
	glGenTextures(1, &this->textureTerrainGridOffsets);

	this->terrainGridW = 0;
	this->terrainGridH = 0;
	this->terrainGridD = 0;
	this->terrainIdHsize = 0;

	this->initEmptyMap();
}


Terrain::Terrain(const Terrain &obj)
{
	glGenBuffers(1, &this->textureBufferTerrainGridFlat);
	glGenTextures(1, &this->textureTerrainGridFlat);
	glGenBuffers(1, &this->textureBufferTerrainGridOffsets);
	glGenTextures(1, &this->textureTerrainGridOffsets);

	this->parameterPoints = obj.parameterPoints;
	this->heightmap = obj.heightmap;
	this->mesh = obj.mesh;
	this->rectangles = obj.rectangles;

	this->generateGridTextures();
}

//---- Destructor --------------------------------------------------------------

Terrain::~Terrain()
{
	glDeleteBuffers(1, &this->textureBufferTerrainGridFlat);
	glDeleteTextures(1, &this->textureTerrainGridFlat);
	glDeleteBuffers(1, &this->textureBufferTerrainGridOffsets);
	glDeleteTextures(1, &this->textureTerrainGridOffsets);
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

GLuint	Terrain::getTextureBufferTerrainGridFlat(void)
{
	return (this->textureBufferTerrainGridFlat);
}


GLuint	Terrain::getTextureTerrainGridFlat(void)
{
	return (this->textureTerrainGridFlat);
}


GLuint	Terrain::getTextureBufferTerrainGridOffsets(void)
{
	return (this->textureBufferTerrainGridOffsets);
}


GLuint	Terrain::getTextureTerrainGridOffsets(void)
{
	return (this->textureTerrainGridOffsets);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Terrain	&Terrain::operator=(const Terrain &obj)
{
	if (this == &obj)
		return (*this);

	this->parameterPoints = obj.parameterPoints;
	this->heightmap = obj.heightmap;
	this->mesh = obj.mesh;
	this->rectangles = obj.rectangles;

	this->generateGridTextures();

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Terrain::loadFromFile(char *filePath)
{
	this->parameterPoints = parse(filePath);
	this->interpolate();
	this->createMesh();
}


void	Terrain::renderMesh(Camera *camera, ShaderManager *shaderManager)
{
	this->mesh.draw(camera, shaderManager->getShader("terrain"), shaderManager->getVAOId());
}

//**** PRIVATE METHODS *********************************************************

void	Terrain::initEmptyMap(void)
{
	this->heightmap.clear();
	for (int y = 0; y < MAP_SIZE; y++)
	{
		std::vector<double> line;
		for (int x = 0; x < MAP_SIZE; x++)
			line.push_back(0.0);
		this->heightmap.push_back(line);
	}
}


void Terrain::interpolate(void)
{
	double	px, py, pz, dx, dy, dist, val;
	std::vector<double> possible_heights;

	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			possible_heights.clear();
			for (Vec3 point: this->parameterPoints)
			{
				px = point.x;
				py = point.y;
				pz = point.z;

				dx = px - x;
				dy = py - y;
				dist = (dx * dx) + (dy * dy);
				if (dist == 0)
				{
					possible_heights.push_back(pz);
					continue;
				}

				dist = std::sqrt(dist);
				val = std::max(0.0, pz*pz - dist*dist);
				if (val == 0)
					continue;

				val /= pz*pz;
				val = val*val*val;
				val = val * pz;
				if (val <= TERRAIN_PRECISION)
					continue;

				possible_heights.push_back(val);

			}
			if (possible_heights.size() == 0)
					continue;

			val = 0.0;
			for (double testVal : possible_heights)
			{
				if (testVal > val)
					val = testVal;
			}
			this->heightmap[y][x] = val;
		}
	}
}


void	Terrain::createMesh(void)
{
	std::vector<t_tri_id>	indices;
	std::vector<bool>		isInRectangle;
	double					r, g, b, height;
	Vec3					p1, p2, p3, A, B, normal;
	int						yId, nyId, nx, area, maxArea,
							maxAreaX, maxAreaY, minLineX;
	unsigned int			vIds[4];

	this->vertices.clear();
	// Vertices creation
	for (double y = 0; y < MAP_SIZE; y++)
	{
		for (double x = 0; x < MAP_SIZE; x++)
		{
			height = this->heightmap[y][x];

			// Compute basic color
			r = height / (double)MAX_HEIGHT;
			g = 0.8;
			b = r;

			// Compute normal
			if (y == MAP_SIZE -1 || x == MAP_SIZE - 1)
			{
				normal = Vec3(0, -1, 0);
			}
			else
			{
				p1 = Vec3(x, height, y);
				p2 = Vec3(x + 1, this->heightmap[y][x + 1], y);
				p3 = Vec3(x, this->heightmap[y + 1][x], y + 1);

				A = p2 - p1;
				B = p3 - p1;

				normal = A.cross(B);
				normal.normalize();
			}

			this->vertices.push_back(Point(Vec3(x, height, y), normal, r, g, b));
		}
	}


	for (int y = 0; y < MAP_SIZE; y++)
		for (int x = 0; x < MAP_SIZE; x++)
			isInRectangle.push_back(false);


	for (int y = 0; y < MAP_SIZE - 1; y++)
	{
		yId = y * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE - 1; x++)
		{

			if (isInRectangle[yId + x])
				continue;

			Vec3	&testNormal = this->vertices[yId + x].normal;

			maxArea = 1;
			maxAreaX = x + 1;
			maxAreaY = y + 1;
			minLineX = MAP_SIZE - 1;
			for (int ny = y; ny < MAP_SIZE; ny++)
			{
				nyId = ny * MAP_SIZE;
				nx = x;
				while (nx < MAP_SIZE)
				{
					if (this->vertices[nyId + nx].normal != testNormal)
						break;
					if (isInRectangle[nyId + nx])
						break;
					nx++;
				}

				if (minLineX > nx)
					minLineX = nx;

				area = (ny - y) * (minLineX - x);
				if (area > maxArea)
				{
					maxArea = area;
					maxAreaX = minLineX;
					maxAreaY = ny;
				}
			}

			nyId = maxAreaY * MAP_SIZE;

			vIds[0] = yId + x;
			vIds[1] = yId + maxAreaX;
			vIds[2] = nyId + maxAreaX;
			vIds[3] = nyId + x;

			indices.push_back((t_tri_id){vIds[0], vIds[1], vIds[2]});
			indices.push_back((t_tri_id){vIds[0], vIds[2], vIds[3]});

			for (int ty = y; ty < maxAreaY; ty++)
				for (int tx = x; tx < maxAreaX; tx++)
					isInRectangle[ty * MAP_SIZE + tx] = true;

			this->rectangles.push_back((t_rectangle){x, y, maxAreaX - x, maxAreaY - y});
		}
	}

	// Create mesh
	this->mesh = Mesh(this->vertices, indices);
	this->generateGridTextures();
}


void	Terrain::generateGridTextures(void)
{
	// Init terrain grid size
	int	terrain_cell_size = TERRAIN_CELL_SIZE;
	this->terrainGridW = MAP_SIZE / terrain_cell_size;
	if (MAP_SIZE > terrain_cell_size && MAP_SIZE % terrain_cell_size != 0)
		this->terrainGridW++;

	this->terrainGridH = MAP_MAX_HEIGHT / terrain_cell_size;
	if (MAP_MAX_HEIGHT > terrain_cell_size
		&& (int)MAP_MAX_HEIGHT % terrain_cell_size != 0)
		this->terrainGridH++;

	this->terrainGridD = this->terrainGridW;
	this->terrainIdHsize = this->terrainGridW * this->terrainGridD;

	// Init terrain grid
	int terrainGridSize = this->terrainGridW * this->terrainGridH * this->terrainGridD;
	std::vector<std::vector<int>>	terrainGrid;

	for (int i = 0; i < terrainGridSize; i++)
	{
		std::vector<int>	terrainGridContent;
		terrainGrid.push_back(terrainGridContent);
	}

	float gStartX, gEndX, gStartY, gEndY, gStartZ, gEndZ;
	int	vid, gx, gy, gz, gid;
	std::unordered_map<int, bool>	idsIn;
	// 3 vec3 per rectangle (pos, size, normal)
	std::vector<glm::vec3>	terrainData;

	for (uint i = 0; i < this->rectangles.size(); i++)
	{
		t_rectangle &rectangle = this->rectangles[i];

		vid = rectangle.y * MAP_SIZE + rectangle.x;
		Vec3	pos = this->vertices[vid].pos;
		Vec3	normal = this->vertices[vid].normal;
		vid = (rectangle.y + rectangle.height) * MAP_SIZE
				+ (rectangle.x + rectangle.width);
		Vec3	endPos = this->vertices[vid].pos;
		Vec3	size = endPos - pos;

		// Put data in terrainData vector
		terrainData.push_back(glm::vec3(pos.x, pos.y, pos.z));
		terrainData.push_back(glm::vec3(size.x, size.y, size.z));
		terrainData.push_back(glm::vec3(normal.x, normal.y, normal.z));

		if (pos.x <= endPos.x)
		{
			gStartX = pos.x;
			gEndX = endPos.x;
		}
		else
		{
			gStartX = endPos.x;
			gEndX = pos.x;
		}

		if (pos.y <= endPos.y)
		{
			gStartY = pos.y;
			gEndY = endPos.y;
		}
		else
		{
			gStartY = endPos.y;
			gEndY = pos.y;
		}

		if (pos.z <= endPos.z)
		{
			gStartZ = pos.z;
			gEndZ = endPos.z;
		}
		else
		{
			gStartZ = endPos.z;
			gEndZ = pos.z;
		}

		idsIn.clear();
		float gridStep = 1.0f;
		for (float x = gStartX; x < gEndX; x += gridStep)
		{
			gx = x / TERRAIN_CELL_SIZE;
			for (float y = gStartY; y < gEndY; y += gridStep)
			{
				gy = y / TERRAIN_CELL_SIZE;
				for (float z = gStartZ; z < gEndZ; z += gridStep)
				{
					gz = z / TERRAIN_CELL_SIZE;
					gid = gx + gz * this->terrainGridW + gy * this->terrainIdHsize;
					// If the id in already in grid, skip it
					if (idsIn.find(gid) != idsIn.end())
						continue;

					terrainGrid[gid].push_back(i);
					idsIn[gid] = true;
				}
			}
		}
	}
	// TODO : Put texture for terrain vertices
}
