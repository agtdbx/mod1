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

bool checkIsInDequekSave(std::deque<std::pair<Point &, Point &>> & save, Point & point)
{
	for (std::pair<Point &, Point &>  pair : save)
	{
		if (point.pos.x >= pair.first.pos.x and point.pos.x <= pair.second.pos.x and
			point.pos.z >= pair.first.pos.z and point.pos.z <= pair.second.pos.z)
			return true;
	}
	return false;
}

bool needToDestroyTriangle(Point & p1,Point & p2,Point & p3,Point & p4)
{
	if (p1.normal == Vec3(0, -1, 0) and
		p2.normal == Vec3(0, -1, 0) and
		p3.normal == Vec3(0, -1, 0) and
		p4.normal == Vec3(0, -1, 0))
		return true;
	return false;
}


void	Terrain::createMesh(void)
{
	std::vector<Point>		vertices;
	std::vector<t_tri_id>	indices;

	unsigned int	id_tl, id_tr, id_dl, id_dr;
	double			r, g, b, height;
	Vec3			p1, p2, p3, A, B, normal;
	std::deque<std::pair<Point &, Point &>>		triangleSave;

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
			if (needToDestroyTriangle(vertices[id_tl], vertices[id_tr], vertices[id_dl], vertices[id_dr])
				or checkIsInDequekSave(triangleSave, vertices[y * MAP_SIZE + x]))
				continue;
			int len1 = MAP_SIZE + 42;
			int len2 = MAP_SIZE + 42;
			std::pair<Point *, Point *> pair1= std::pair<Point *, Point *>(NULL,NULL);
			std::pair<Point *, Point *> pair2= std::pair<Point *, Point *>(NULL,NULL);
			std::cout << "create new :" << std::endl;
			Vec3	testVect = vertices[y * MAP_SIZE + x].normal;
			
			for (int y_n = y; y_n < MAP_SIZE - 1; y_n++)
			{
				for (int x_n = x; x_n < MAP_SIZE - 1; x_n++)
				{
					if (vertices[y_n * MAP_SIZE + x_n].normal != testVect or vertices[y_n * MAP_SIZE + (x_n + 1)].normal != testVect or
						vertices[(y_n + 1) * MAP_SIZE + x_n].normal != testVect or vertices[(y_n + 1) * MAP_SIZE + (x_n + 1)].normal != testVect)
					{
						std::cout << "test" << std::endl;
						if (len1 == MAP_SIZE + 42)
						{
							std::cout << "T1" << std::endl;

							len1 = x_n - x;
						}
						else
						{
							if (x_n - x != len1)
							{
						std::cout << "T2" << std::endl;

								if (len1 == 0)
									len1 = 1;
								pair1.first = &vertices[y * MAP_SIZE + x];
								pair1.second = &vertices[y_n * MAP_SIZE + x + len1];
							}
						}
						break;
					}
					else if (y_n == x_n && x_n == MAP_SIZE - 2)
					{
						if (len1 == 0)
							len1 = 1;
						if (len1 == MAP_SIZE + 42)
						{
							len1 = x_n - x;
						}
						pair1.first = &vertices[y * MAP_SIZE + x];
						pair1.second = &vertices[y_n * MAP_SIZE + x + len1];	
					}
				}
				if (pair1.first)
					break;
				if (len1 == MAP_SIZE + 42)
				{
					len1 = MAP_SIZE - 2 - x;
				}
			}

			//test Y axe
			for (int x_n = x; x_n < MAP_SIZE - 1; x_n++) //
			{
				for (int y_n = y; y_n < MAP_SIZE - 1; y_n++)
				{
					if (vertices[y_n * MAP_SIZE + x_n].normal != testVect or vertices[(y_n + 1) * MAP_SIZE + x_n].normal != testVect or
						vertices[y_n * MAP_SIZE + (x_n + 1)].normal != testVect or vertices[(y_n + 1) * MAP_SIZE + (x_n + 1)].normal != testVect )
					{
						if (len2 == MAP_SIZE + 42)
						{
							len2 = y_n - y;
						}
						else
						{
							if (y_n - y != len2)
							{
								if (len2 == 0)
									len2 = 1;
								pair2.first = &vertices[y * MAP_SIZE + x];
								pair2.second = &vertices[(y + len2) * MAP_SIZE + x_n];
							}
						}
						break;
					}
					else if (y_n == x_n && x_n == MAP_SIZE - 2)
					{
						if (len2 == 0)
							len2 = 1;
						if (len2 == MAP_SIZE + 42)
						{
							len2 = y_n - y;
						}
						pair2.first = &vertices[y * MAP_SIZE + x];
						pair2.second = &vertices[(y + len2) * MAP_SIZE + x_n];		
					}
				}
				if (pair2.first)
					break;
				if (len2 == MAP_SIZE + 42)
					len2 = MAP_SIZE - 2 - y;
			}
			// std::cout << pair1.first << " | " << pair1.second << std::endl;
			// std::cout << pair2.first << " | " << pair2.second << std::endl;
			if ((pair1.first->pos.x + pair1.second->pos.x) * (pair1.first->pos.z + pair1.second->pos.z)  >
				(pair2.first->pos.x + pair2.second->pos.x) * (pair2.first->pos.z + pair2.second->pos.z) )
			{
				id_tl = pair1.first->pos.z * MAP_SIZE + pair1.first->pos.x;
				id_tr = pair1.first->pos.z * MAP_SIZE + pair1.second->pos.x;
				id_dl = pair1.second->pos.z * MAP_SIZE + pair1.first->pos.x;
				id_dr = pair1.second->pos.z * MAP_SIZE + pair1.second->pos.x;
				triangleSave.push_back(std::pair<Point &, Point&>(*pair1.first, *pair1.second));
				std::cout << "adding : " << pair1.first->pos.x << " | " << pair1.first->pos.z << std::endl;
				std::cout << "adding : " << pair1.second->pos.x << " | " << pair1.second->pos.z << std::endl;
			}
			else
			{
				id_tl = pair2.first->pos.z * MAP_SIZE + pair2.first->pos.x;
				id_tr = pair2.first->pos.z * MAP_SIZE + pair2.second->pos.x;
				id_dl = pair2.second->pos.z * MAP_SIZE + pair2.first->pos.x;
				id_dr = pair2.second->pos.z * MAP_SIZE + pair2.second->pos.x;
				triangleSave.push_back(std::pair<Point &, Point&>(*pair2.first, *pair2.second));
				std::cout << "adding : " << pair2.first->pos.x << " | " << pair2.first->pos.z << std::endl;
				std::cout << "adding : " << pair2.second->pos.x << " | " << pair2.second->pos.z << std::endl;
			}
			indices.push_back((t_tri_id){id_tl, id_tr, id_dr});
			indices.push_back((t_tri_id){id_tl, id_dr, id_dl});
		}
	}
	this->mesh = Mesh(vertices, indices);
}
