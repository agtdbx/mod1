#include <engine/render/Mesh.hpp>

#include <stdexcept>
#include <cmath>

#include <define.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mesh::Mesh(void)
{
	this->vertices = NULL;
	this->indices = NULL;
	this->nbVertices = 0;
	this->nbIndices = 0;
	this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->degrees_x = 0.0f;
	this->degrees_y = 0.0f;
	this->degrees_z = 0.0f;
	this->scaleFactor = 1.0f;
	this->computeModelMatrix();
}


Mesh::Mesh(std::vector<Point> &vertices, std::vector<t_tri_id> &indices)
{
	this->nbVertices = vertices.size() * 9;
	this->vertices = new float[this->nbVertices];

	if (this->vertices == NULL)
		throw new std::invalid_argument("Mesh vertrices alloc failed");

	int	id;
	for (int i = 0; i < this->nbVertices; i += 9)
	{
		id = i / 9;
		this->vertices[i    ] = vertices[id].pos.x;
		this->vertices[i + 1] = vertices[id].pos.y;
		this->vertices[i + 2] = vertices[id].pos.z;
		this->vertices[i + 3] = vertices[id].normal.x;
		this->vertices[i + 4] = vertices[id].normal.y;
		this->vertices[i + 5] = vertices[id].normal.z;
		this->vertices[i + 6] = vertices[id].r;
		this->vertices[i + 7] = vertices[id].g;
		this->vertices[i + 8] = vertices[id].b;
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
		id = i / 3;
		this->indices[i    ] = indices[id].p1;
		this->indices[i + 1] = indices[id].p2;
		this->indices[i + 2] = indices[id].p3;
	}

	this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->degrees_x = 0.0f;
	this->degrees_y = 0.0f;
	this->degrees_z = 0.0f;
	this->scaleFactor = 1.0f;
	this->computeModelMatrix();
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

	this->translation = obj.translation;
	this->degrees_x = obj.degrees_x;
	this->degrees_y = obj.degrees_y;
	this->degrees_z = obj.degrees_z;
	this->scaleFactor = obj.scaleFactor;
	this->computeModelMatrix();
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

void Mesh::setVertices(std::vector<Point> & vertices)
{
	if (this->vertices)
	{
		delete [] this->vertices;
		this->vertices = NULL;
	}

	this->nbVertices = vertices.size() * 9;
	this->vertices = new float[this->nbVertices];

	if (this->vertices == NULL)
		throw new std::invalid_argument("Mesh vertrices alloc failed");

	int	id;
	for (int i = 0; i < this->nbVertices; i += 9)
	{
		id = i / 9;
		this->vertices[i    ] = vertices[id].pos.x;
		this->vertices[i + 1] = vertices[id].pos.y;
		this->vertices[i + 2] = vertices[id].pos.z;
		this->vertices[i + 3] = vertices[id].normal.x;
		this->vertices[i + 4] = vertices[id].normal.y;
		this->vertices[i + 5] = vertices[id].normal.z;
		this->vertices[i + 6] = vertices[id].r;
		this->vertices[i + 7] = vertices[id].g;
		this->vertices[i + 8] = vertices[id].b;
	}
}
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

	this->translation = obj.translation;
	this->degrees_x = obj.degrees_x;
	this->degrees_y = obj.degrees_y;
	this->degrees_z = obj.degrees_z;
	this->scaleFactor = obj.scaleFactor;
	this->computeModelMatrix();

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Mesh::translate(glm::vec3 translation)
{
	this->translation += translation;
	this->computeModelMatrix();
}


void	Mesh::rotate(glm::vec3 rotation, float degrees)
{
	rotation *= degrees;
	this->degrees_x += rotation.x;
	this->degrees_y += rotation.y;
	this->degrees_z += rotation.z;
	this->computeModelMatrix();
}


void	Mesh::scale(float scale)
{
	this->scaleFactor *= scale;
	this->computeModelMatrix();
}


void	Mesh::draw(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName)
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

	int modelLoc = glGetUniformLocation(shader->getShaderId(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->model));
	int viewLoc = glGetUniformLocation(shader->getShaderId(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));
	int projLoc = glGetUniformLocation(shader->getShaderId(), "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection()));
	int cameraPosLoc = glGetUniformLocation(shader->getShaderId(), "cameraPos");
	glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera->getPosition()));

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

void	Mesh::computeModelMatrix(void)
{
	this->model = glm::mat4(1.0f);

	this->model = glm::scale(this->model, glm::vec3(this->scaleFactor, this->scaleFactor, this->scaleFactor));
	this->model = glm::translate(this->model, this->translation);
	// if (this->degrees != 0.0f)
	this->model = glm::rotate(this->model, glm::radians(this->degrees_x), glm::vec3(1.0f, 0.0f, 0.0f));
	this->model = glm::rotate(this->model, glm::radians(this->degrees_y), glm::vec3(0.0f, 1.0f, 0.0f));
	this->model = glm::rotate(this->model, glm::radians(this->degrees_z), glm::vec3(0.0f, 0.0f, 1.0f));
}
