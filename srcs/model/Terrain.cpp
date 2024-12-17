#include <model/Terrain.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Terrain::Terrain(void)
{
	this->initEmptyMap();
}


Terrain::Terrain(const Terrain &obj)
{
	this->parameterPoints = obj.parameterPoints;
	this->heightmap = obj.heightmap;
	this->mesh = obj.mesh;
}

//---- Destructor --------------------------------------------------------------

Terrain::~Terrain()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Terrain	&Terrain::operator=(const Terrain &obj)
{
	if (this == &obj)
		return (*this);

	this->parameterPoints = obj.parameterPoints;
	this->heightmap = obj.heightmap;
	this->mesh = obj.mesh;

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
	this->mesh.draw(camera, shaderManager->getTerrainShader(), shaderManager->getVAOId());
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
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;

	unsigned int	id_tl, id_tr, id_dl, id_dr;
	double			r, g, b, height;
	Vec3			p1, p2, p3, A, B, normal;

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
				normal = Vec3(0, 1, 0);
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

			vertices.push_back(Point(Vec3(x, height, y), normal, r, g, b));
		}
	}

	for (int y = 0; y < MAP_SIZE - 1; y++)
	{
		for (int x = 0; x < MAP_SIZE - 1; x++)
		{
			id_tl = y * MAP_SIZE + x;
			id_tr = y * MAP_SIZE + (x + 1);
			id_dl = (y + 1) * MAP_SIZE + x;
			id_dr = (y + 1) * MAP_SIZE + (x + 1);

			indices.push_back((t_tri_id){id_tl, id_tr, id_dr});
			indices.push_back((t_tri_id){id_tl, id_dr, id_dl});
		}
	}

	this->mesh = Mesh(vertices, indices);
}
