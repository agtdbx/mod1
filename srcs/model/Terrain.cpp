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


void	Terrain::renderMesh(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName)
{
	this->mesh.draw(camera, shader, textureManager, textureName);
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

	// for (double y = 0; y < MAP_SIZE; y++)
	// {
	// 	for (double x = 0; x < MAP_SIZE; x++)
	// 	{
	// 		double red = double(x/ MAP_SIZE);
	// 		double green = double(y / MAP_SIZE);
	// 		double blue = 0;
	// 		// double red = 0.5;
	// 		// double green = 0.5;
	// 		// double blue = 0.5;
	// 		if (int(y) % 2 == 0)
	// 		{
	// 			if (int(x) % 2 == 0)
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 1.0));
	// 			}
	// 			else
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 1.0));
	// 			}

	// 		}
	// 		else
	// 		{
	// 			if (int(x) % 2 == 0)
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue,  1.0, 0.0));
	// 			}
	// 			else
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 0.0));
	// 			}
	// 		}
	// 		// vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), double(x/ MAP_SIZE), double(y / MAP_SIZE), 0.0, 1.0, 0.0));
	// 		// std::cout << " New point : " << Vec3((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10)) << std::endl;
	// 	}
	// }
	// for (double y = 0; y < MAP_SIZE; y++)
	// {
	// 	for (double x = 0; x < MAP_SIZE; x++)
	// 	{
	// 		double red = double(x/ MAP_SIZE);
	// 		double green = double(y / MAP_SIZE);
	// 		double blue = 0;
	// 		// double red = 0.5;
	// 		// double green = 0.5;
	// 		// double blue = 0.5;
	// 		if (int(y) % 2 == 0)
	// 		{
	// 			if (int(x) % 2 == 0)
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 0.0));
	// 			}
	// 			else
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 0.0));
	// 			}

	// 		}
	// 		else
	// 		{
	// 			if (int(x) % 2 == 0)
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 1.0, 1.0));
	// 			}
	// 			else
	// 			{
	// 				vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), red, green, blue, 0.0, 1.0));
	// 			}
	// 		}
	// 		// vertices.push_back(Point((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10), double(x/ MAP_SIZE), double(y / MAP_SIZE), 0.0, 1.0, 0.0));
	// 		// std::cout << " New point : " << Vec3((x - (MAP_SIZE / 2)) / MAP_SIZE,  (y - (MAP_SIZE / 2)) / MAP_SIZE, (this->heightmap[y][x] - ((MAX_HEIGHT * 10) / 2)) / (MAX_HEIGHT * 10)) << std::endl;
	// 	}
	// }

	// for (unsigned y = 0; y < MAP_SIZE - 1; y++)
	// {
	// 	for (unsigned x = 0; x < MAP_SIZE - 1; x++)
	// 	{
	// 		if (y % 2 == 0)
	// 		{
	// 		indices.push_back((t_tri_id){y * MAP_SIZE + x, y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x});
	// 		indices.push_back((t_tri_id){y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x, (y + 1) * MAP_SIZE + x + 1});
	// 		}
	// 		else
	// 		{
	// 		indices.push_back((t_tri_id){(MAP_SIZE * MAP_SIZE) + y * MAP_SIZE + x,(MAP_SIZE * MAP_SIZE) + y * MAP_SIZE + x + 1,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x});
	// 		indices.push_back((t_tri_id){(MAP_SIZE * MAP_SIZE) +y * MAP_SIZE + x + 1,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x,(MAP_SIZE * MAP_SIZE) + (y + 1) * MAP_SIZE + x + 1});
	// 		}
	// 		// indices.push_back((t_tri_id){y * MAP_SIZE + x, y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x});
	// 		// indices.push_back((t_tri_id){y * MAP_SIZE + x + 1, (y + 1) * MAP_SIZE + x, (y + 1) * MAP_SIZE + x + 1});
	// 		// std::cout << " create triangle with point " << y * MAP_SIZE + x << ", "  <<  y * MAP_SIZE + x + 1 << ", "  << (y + 1) * MAP_SIZE + x
	// 		// << " and  " << y * MAP_SIZE + x + 1 << ", "  << (y + 1) * MAP_SIZE + x << ", "  << (y + 1) * MAP_SIZE + x + 1 << std::endl;
	// 	}
	// }

	unsigned int	id_tl, id_tr, id_dl, id_dr;
	double			r, g, b, height;
	Vec3			A, B;

	/*
	So for a triangle p1, p2, p3, if the vector A = p2 - p1 and the vector B = p3 - p1 then the normal N = A x B and can be calculated by:

	Nx = Ay * Bz - Az * By
	Ny = Az * Bx - Ax * Bz
	Nz = Ax * By - Ay * Bx
	*/

	for (double y = 0; y < MAP_SIZE; y++)
	{
		for (double x = 0; x < MAP_SIZE; x++)
		{
			height = this->heightmap[y][x];
			r = x / (double)MAP_SIZE;
			g = height / (double)MAX_HEIGHT;
			b = y / (double)MAP_SIZE;
			vertices.push_back(Point(x, height, y, r, g, b));
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
