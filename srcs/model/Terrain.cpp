#include <model/Terrain.hpp>

//**** STATIC FUNCTIONS ********************************************************

static float	min(float a, float b)
{
	if (a < b)
		return (a);
	return (b);
}

static float	max(float a, float b)
{
	if (a > b)
		return (a);
	return (b);
}

static float	min4(float a, float b, float c, float d)
{
	float	mini = a;

	mini = min(mini, b);
	mini = min(mini, c);
	mini = min(mini, d);
	return (mini);
}

static float	max4(float a, float b, float c, float d)
{
	float	maxi = a;

	maxi = max(maxi, b);
	maxi = max(maxi, c);
	maxi = max(maxi, d);
	return (maxi);
}


static Vec3	lerp(Vec3 a, Vec3 b, float r)
{
	return (a + (b - a) * r);
}

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Terrain::Terrain(void)
{
	glGenBuffers(1, &this->textureBufferTerrainGridData);
	glGenTextures(1, &this->textureTerrainGridData);
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
	glGenBuffers(1, &this->textureBufferTerrainGridData);
	glGenTextures(1, &this->textureTerrainGridData);
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
	glDeleteBuffers(1, &this->textureBufferTerrainGridData);
	glDeleteTextures(1, &this->textureTerrainGridData);
	glDeleteBuffers(1, &this->textureBufferTerrainGridFlat);
	glDeleteTextures(1, &this->textureTerrainGridFlat);
	glDeleteBuffers(1, &this->textureBufferTerrainGridOffsets);
	glDeleteTextures(1, &this->textureTerrainGridOffsets);
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

GLuint	Terrain::getTextureBufferTerrainGridData(void)
{
	return (this->textureBufferTerrainGridData);
}


GLuint	Terrain::getTextureTerrainGridData(void)
{
	return (this->textureTerrainGridData);
}


int	Terrain::getDataNbRectangles(void)
{
	return (this->dataNbRectangles);
}


GLuint	Terrain::getTextureBufferTerrainGridFlat(void)
{
	return (this->textureBufferTerrainGridFlat);
}


GLuint	Terrain::getTextureTerrainGridFlat(void)
{
	return (this->textureTerrainGridFlat);
}


int	Terrain::getSizeTerrainGridFlat(void)
{
	return (this->flatTerrainGridSize);
}


GLuint	Terrain::getTextureBufferTerrainGridOffsets(void)
{
	return (this->textureBufferTerrainGridOffsets);
}


GLuint	Terrain::getTextureTerrainGridOffsets(void)
{
	return (this->textureTerrainGridOffsets);
}


int	Terrain::getSizeTerrainGridOffsets(void)
{
	return (this->offsetsTerrainGridSize);
}


void	Terrain::getGridSize(int sizes[4])
{
	sizes[0] = this->terrainGridW;
	sizes[1] = this->terrainGridH;
	sizes[2] = this->terrainGridD;
	sizes[3] = this->terrainIdHsize;
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

void	Terrain::loadFromParse(std::vector<Vec3> & pointsList, std::vector<std::vector<double>> heightmap)
{
	this->parameterPoints = pointsList;
	this->heightmap = heightmap;
	// this->interpolate();
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


void	Terrain::createMesh(void)
{
	std::vector<t_tri_id>	indices;
	std::vector<bool>		isInRectangle;
	double					r, g, b, height, brownRatio;
	Vec3					p1, p2, p3, A, B, top, normal, color, brown;
	int						yId, nyId, nx, area, maxArea,
							maxAreaX, maxAreaY, minLineX;
	unsigned int			vIds[4];

	top = Vec3(0, -1, 0);
	brown = Vec3(0.345, 0.223, 0.152);

	this->vertices.clear();
	// Vertices creation
	for (double y = 0; y < MAP_SIZE; y++)
	{
		for (double x = 0; x < MAP_SIZE; x++)
		{
			height = this->heightmap[y][x];

			// Compute basic color
			r = height / (double)MAX_HEIGHT;
			g = max(0.8, r);
			b = r;
			color = Vec3(r, g, b);

			// Compute normal
			if (y == MAP_SIZE -1 || x == MAP_SIZE - 1)
				normal = top;
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

			brownRatio = 1.0 - top.dot(normal);
			color = lerp(color, brown, brownRatio);

			this->vertices.push_back(Point(Vec3(x, height, y), normal, color));
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
	this->terrainGridW = MAP_SIZE / TERRAIN_CELL_SIZE;
	if (MAP_SIZE > TERRAIN_CELL_SIZE && MAP_SIZE % TERRAIN_CELL_SIZE != 0)
		this->terrainGridW++;

	this->terrainGridH = MAP_MAX_HEIGHT / TERRAIN_CELL_SIZE;
	if (MAP_MAX_HEIGHT > TERRAIN_CELL_SIZE
		&& (int)MAP_MAX_HEIGHT % TERRAIN_CELL_SIZE != 0)
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

	// Fill terrain grid
	float	gStartX, gEndX, gStartY, gEndY, gStartZ, gEndZ;
	int		vid, gx, gy, gz, gid;
	int		rectangleId = 0;
	std::unordered_map<int, bool>	idsIn;
	// 5 vec3 per rectangle (pos, vecX, vecZ, vecXZ, normal)
	std::vector<glm::vec3>	terrainData;

	for (t_rectangle &rectangle : this->rectangles)
	{
		vid = rectangle.y * MAP_SIZE + rectangle.x;
		Vec3	topLeft = this->vertices[vid].pos;
		Vec3	normal = this->vertices[vid].normal;

		vid = rectangle.y * MAP_SIZE + (rectangle.x + rectangle.width);
		Vec3	topRight = this->vertices[vid].pos;
		Vec3	vecX = topRight - topLeft;

		vid = (rectangle.y + rectangle.height) * MAP_SIZE + rectangle.x;
		Vec3	botLeft = this->vertices[vid].pos;
		Vec3	vecZ = botLeft - topLeft;

		vid = (rectangle.y + rectangle.height) * MAP_SIZE
				+ (rectangle.x + rectangle.width);
		Vec3	botRight = this->vertices[vid].pos;
		Vec3	vecXZ = botRight - topLeft;

		// If the rectangle is on the ground, ignore it
		if	(topLeft.y == 0.0 && topRight.y == 0.0 && botLeft.y == 0.0 && botRight.y == 0.0)
			continue;

		// Put data in terrainData vector
		terrainData.push_back(glm::vec3(topLeft.x, topLeft.y, topLeft.z));
		terrainData.push_back(glm::vec3(vecX.x, vecX.y, vecX.z));
		terrainData.push_back(glm::vec3(vecZ.x, vecZ.y, vecZ.z));
		terrainData.push_back(glm::vec3(vecXZ.x, vecXZ.y, vecXZ.z));
		terrainData.push_back(glm::vec3(normal.x, normal.y, normal.z));

		gStartX = min4(topLeft.x, topRight.x, botLeft.x, botRight.x);
		gEndX = max4(topLeft.x, topRight.x, botLeft.x, botRight.x);
		gStartY = min4(topLeft.y, topRight.y, botLeft.y, botRight.y);
		gEndY = max4(topLeft.y, topRight.y, botLeft.y, botRight.y);
		gStartZ = min4(topLeft.z, topRight.z, botLeft.z, botRight.z);
		gEndZ = max4(topLeft.z, topRight.z, botLeft.z, botRight.z);

		idsIn.clear();
		float gridStep = 0.2f;
		for (float x = gStartX; x <= gEndX; x += gridStep)
		{
			gx = x / TERRAIN_CELL_SIZE;
			for (float y = gStartY; y <= gEndY; y += gridStep)
			{
				gy = y / TERRAIN_CELL_SIZE;
				for (float z = gStartZ; z <= gEndZ; z += gridStep)
				{
					gz = z / TERRAIN_CELL_SIZE;
					gid = gx + gz * this->terrainGridW + gy * this->terrainIdHsize;
					// If the id in already in grid, skip it
					if (idsIn.find(gid) != idsIn.end())
						continue;

					terrainGrid[gid].push_back(rectangleId);
					idsIn[gid] = true;
				}
			}
		}
		rectangleId++;
	}
	this->dataNbRectangles = rectangleId;

	// Create flat version of the grid
	std::vector<float>	flatTerrainGrid;
	std::vector<float>	offsetsTerrainGrid;
	this->flatTerrainGridSize = 0;
	this->offsetsTerrainGridSize = 0;
	int	terrainCellSize;

	for (std::vector<int> &terrainCell : terrainGrid)
	{
		terrainCellSize = terrainCell.size();

		for (int i = 0; i < terrainCellSize; i++)
			flatTerrainGrid.push_back(terrainCell[i]);
		offsetsTerrainGrid.push_back(this->flatTerrainGridSize);
		this->flatTerrainGridSize += terrainCellSize;
		this->offsetsTerrainGridSize++;
	}

	// Fill textures from flat grids
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferTerrainGridData);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * terrainData.size(),
					terrainData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferTerrainGridFlat);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->flatTerrainGridSize,
					flatTerrainGrid.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferTerrainGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->offsetsTerrainGridSize,
					offsetsTerrainGrid.data(), GL_DYNAMIC_DRAW);
}
