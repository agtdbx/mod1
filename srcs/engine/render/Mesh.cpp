#include <engine/render/Mesh.hpp>

#include <stdexcept>
#include <cmath>

#include <define.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mesh::Mesh(void)
{
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

	for (uint i = 0; i < vertices.size(); i++)
	{
		this->vertices.push_back(vertices[i].pos.x);
		this->vertices.push_back(vertices[i].pos.y);
		this->vertices.push_back(vertices[i].pos.z);
		this->vertices.push_back(vertices[i].normal.x);
		this->vertices.push_back(vertices[i].normal.y);
		this->vertices.push_back(vertices[i].normal.z);
		this->vertices.push_back(vertices[i].r);
		this->vertices.push_back(vertices[i].g);
		this->vertices.push_back(vertices[i].b);
	}

	this->nbIndices = indices.size() * 3;

	for (uint i = 0; i < indices.size(); i++)
	{
		this->indices.push_back(indices[i].p1);
		this->indices.push_back(indices[i].p2);
		this->indices.push_back(indices[i].p3);
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
	this->vertices = obj.vertices;
	this->nbIndices = obj.nbIndices;
	this->indices = obj.indices;
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
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------

void Mesh::setVertices(std::vector<Point> & vertices)
{
	this->vertices.clear();
	this->nbVertices = vertices.size() * 9;
	for (uint i = 0; i < vertices.size(); i++)
	{
		this->vertices.push_back(vertices[i].pos.x);
		this->vertices.push_back(vertices[i].pos.y);
		this->vertices.push_back(vertices[i].pos.z);
		this->vertices.push_back(vertices[i].normal.x);
		this->vertices.push_back(vertices[i].normal.y);
		this->vertices.push_back(vertices[i].normal.z);
		this->vertices.push_back(vertices[i].r);
		this->vertices.push_back(vertices[i].g);
		this->vertices.push_back(vertices[i].b);
	}
}

void	Mesh::setMesh(std::vector<Point> &vertices, std::vector<t_tri_id> &indices)
{
	this->vertices.clear();
	this->nbVertices = vertices.size() * 9;
	for (uint i = 0; i < vertices.size(); i++)
	{
		this->vertices.push_back(vertices[i].pos.x);
		this->vertices.push_back(vertices[i].pos.y);
		this->vertices.push_back(vertices[i].pos.z);
		this->vertices.push_back(vertices[i].normal.x);
		this->vertices.push_back(vertices[i].normal.y);
		this->vertices.push_back(vertices[i].normal.z);
		this->vertices.push_back(vertices[i].r);
		this->vertices.push_back(vertices[i].g);
		this->vertices.push_back(vertices[i].b);
	}

	this->indices.clear();
	this->nbIndices = indices.size() * 3;
	for (uint i = 0; i < indices.size(); i++)
	{
		this->indices.push_back(indices[i].p1);
		this->indices.push_back(indices[i].p2);
		this->indices.push_back(indices[i].p3);
	}
}


//---- Operators ---------------------------------------------------------------

Mesh	&Mesh::operator=(const Mesh &obj)
{
	if (this == &obj)
		return (*this);

	this->nbVertices = obj.nbVertices;
	this->vertices = obj.vertices;
	this->nbIndices = obj.nbIndices;
	this->indices = obj.indices;
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


void	Mesh::draw(Camera *camera, Shader *shader, unsigned int VAOid)
{
	if (shader == NULL)
		return ;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->nbVertices,
					this->vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->nbIndices,
					this->indices.data(), GL_STATIC_DRAW);

	shader->use();

	int modelLoc = glGetUniformLocation(shader->getShaderId(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->model));
	int viewLoc = glGetUniformLocation(shader->getShaderId(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));
	int projLoc = glGetUniformLocation(shader->getShaderId(), "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection()));
	int lightPosLoc = glGetUniformLocation(shader->getShaderId(), "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(camera->getLightPosition()));

	glBindVertexArray(VAOid);

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
