#include <engine/render/Mesh.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mesh::Mesh(void)
{
	this->vertices = NULL;
	this->indices = NULL;
	this->nbVertices = 0;
	this->nbIndices = 0;
}


Mesh::Mesh(std::vector<Point> &vertices, std::vector<t_tri_id> &indices)
{
	this->nbVertices = vertices.size() * 8;
	this->vertices = new float[this->nbVertices];

	if (this->vertices == NULL)
		throw new std::invalid_argument("Mesh vertrices alloc failed");

	int	id;
	for (int i = 0; i < this->nbVertices; i += 8)
	{
		id = i / 8;
		this->vertices[i] = vertices[id].pos.x;
		this->vertices[i + 1] = vertices[id].pos.y;
		this->vertices[i + 2] = vertices[id].pos.z;
		this->vertices[i + 3] = vertices[id].r;
		this->vertices[i + 4] = vertices[id].g;
		this->vertices[i + 5] = vertices[id].b;
		this->vertices[i + 6] = vertices[id].imgx;
		this->vertices[i + 7] = vertices[id].imgy;
	}

	this->nbIndices = indices.size() * 3;
	this->indices = new unsigned int[this->nbIndices];

	if (this->indices == NULL)
	{
		delete [] this->vertices;
		throw new std::invalid_argument("Mesh indices alloc failed");
	}

	for (int i = 0; i < this->nbIndices; i += 3)
	{
		this->indices[i] = indices[i / 3].p1;
		this->indices[i + 1] = indices[i / 3].p2;
		this->indices[i + 2] = indices[i / 3].p3;
	}
}


Mesh::Mesh(const Mesh &obj)
{
	this->nbVertices = obj.nbVertices;
	this->vertices = new float[this->nbVertices];

	if (this->vertices == NULL)
		throw new std::invalid_argument("Mesh vertrices alloc failed");

	for (int i = 0; i < this->nbVertices; i++)
		this->vertices[i] = obj.vertices[i];

	this->nbIndices = obj.nbIndices;
	this->indices = new unsigned int[this->nbIndices];

	if (this->indices == NULL)
	{
		delete [] this->vertices;
		throw new std::invalid_argument("Mesh indices alloc failed");
	}

	for (int i = 0; i < this->nbIndices; i++)
		this->indices[i] = obj.indices[i];
}

//---- Destructor --------------------------------------------------------------

Mesh::~Mesh()
{
	if (this->vertices != NULL)
		delete [] this->vertices;
	this->vertices = NULL;
	if (this->indices != NULL)
		delete [] this->indices;
	this->indices = NULL;
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Mesh	&Mesh::operator=(const Mesh &obj)
{
	if (this == &obj)
		return (*this);

	if (this->vertices != NULL)
		delete [] this->vertices;
	this->vertices = NULL;
	if (this->indices != NULL)
		delete [] this->indices;
	this->indices = NULL;

	this->nbVertices = obj.nbVertices;
	this->vertices = new float[this->nbVertices];

	if (this->vertices == NULL)
		throw new std::invalid_argument("Mesh vertrices alloc failed");

	for (int i = 0; i < this->nbVertices; i++)
		this->vertices[i] = obj.vertices[i];

	this->nbIndices = obj.nbIndices;
	this->indices = new unsigned int[this->nbIndices];

	if (this->indices == NULL)
	{
		delete [] this->vertices;
		throw new std::invalid_argument("Mesh indices alloc failed");
	}

	for (int i = 0; i < this->nbIndices; i++)
		this->indices[i] = obj.indices[i];

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Mesh::draw(Shader *shader)
{
	if (this->vertices == NULL || this ->indices == NULL)
		return ;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->nbVertices, this->vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->nbIndices, this->indices, GL_STATIC_DRAW);

	shader->use();

	glDrawElements(GL_TRIANGLES, this->nbIndices, GL_UNSIGNED_INT, 0);
}


void	Mesh::drawWithTexture(Shader *shader, TextureManager *textureManager, std::string textureName)
{
	unsigned int	texture;

	if (this->vertices == NULL || this ->indices == NULL)
		return ;

	try
	{
		texture = textureManager->getTexture(textureName);
	}
	catch (std::exception &e)
	{
		return ;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->nbVertices, this->vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->nbIndices, this->indices, GL_STATIC_DRAW);

	shader->use();

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(shader->getVAOId());
	glDrawElements(GL_TRIANGLES, this->nbIndices, GL_UNSIGNED_INT, 0);
}


void	Mesh::print(void)
{
	printf("MESH\n");
	printf("nb vertices : %i, nb indices %i\n", this->nbVertices, this->nbIndices);
	printf("vertices\n");
	for (int i = 0; i < this->nbVertices; i += 3)
		printf("  %f, %f, %f\n", this->vertices[i], this->vertices[i + 1], this->vertices[i + 2]);
	printf("indices\n");
	for (int i = 0; i < this->nbIndices; i += 3)
		printf("  %u, %u, %u\n", this->indices[i], this->indices[i + 1], this->indices[i + 2]);
}

//**** PRIVATE METHODS *********************************************************
