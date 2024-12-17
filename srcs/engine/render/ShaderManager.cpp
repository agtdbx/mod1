#include <engine/render/ShaderManager.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

ShaderManager::ShaderManager(void)
{
	// Allocate memory for vertice array into gpu
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// Allocate memory into gpu
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	// Allocate buffer for indices
	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
}

//---- Destructor --------------------------------------------------------------

ShaderManager::~ShaderManager()
{
	if (this->VAO >= 0)
		glDeleteVertexArrays(1, &this->VAO);
	this->VAO = -1;
	if (this->VBO >= 0)
		glDeleteBuffers(1, &this->VBO);
	this->VBO = -1;
	if (this->EBO >= 0)
		glDeleteBuffers(1, &this->EBO);
	this->EBO = -1;
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

Shader	*ShaderManager::getTerrainShader(void)
{
	return (&this->terrainShader);
}


Shader	*ShaderManager::getWaterShader(void)
{
	return (&this->waterShader);
}


unsigned int	ShaderManager::getVAOId(void)
{
	return (this->VAO);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

ShaderManager	&ShaderManager::operator=(const ShaderManager &obj)
{
	if (this == &obj)
		return (*this);

	this->VAO = -1;
	this->VBO = -1;
	this->EBO = -1;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	ShaderManager::loadShaderFiles(void)
{
	this->terrainShader.load("data/shaders/terrain.vs", "data/shaders/terrain.fs");
	this->waterShader.load("data/shaders/water.vs", "data/shaders/water.fs");
}

//**** PRIVATE METHODS *********************************************************
