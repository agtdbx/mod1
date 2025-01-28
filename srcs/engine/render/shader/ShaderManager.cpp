#include <engine/render/shader/ShaderManager.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);

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

Shader	*ShaderManager::getShader(std::string shaderName)
{
	std::unordered_map<std::string, Shader>::iterator	checkShader;

	checkShader = this->shaders.find(shaderName);

	if (checkShader == this->shaders.end())
		return (NULL);

	return (&checkShader->second);
}


ComputeShader	*ShaderManager::getComputeShader(std::string shaderName)
{
	std::unordered_map<std::string, ComputeShader>::iterator	checkShader;

	checkShader = this->computeShaders.find(shaderName);

	if (checkShader == this->computeShaders.end())
		return (NULL);

	return (&checkShader->second);
}


WaterShader	*ShaderManager::getWaterShader(void)
{
	return (&this->waterShader);
}


MenuShader	*ShaderManager::getMenuShader(void)
{
	return (&this->menuShader);
}


unsigned int	ShaderManager::getVAOId()
{
	return (this->VAO);
}

unsigned int	ShaderManager::getVBOId()
{
	return (this->VBO);
}

unsigned int	ShaderManager::getEBOId()
{
	return (this->EBO);
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

	this->shaders = obj.shaders;
	this->waterShader = obj.waterShader;
	this->computeShaders = obj.computeShaders;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	ShaderManager::addShader(
						std::string shaderName,
						std::string vShaderFile,
						std::string fShaderFile)
{
	std::unordered_map<std::string, Shader>::const_iterator	checkShader;

	checkShader = this->shaders.find(shaderName);

	// If shader exist, error
	if (checkShader != this->shaders.end())
		throw std::invalid_argument("Shader name '" + shaderName
										+ "' is already took");

	Shader	shader(vShaderFile, fShaderFile);

	this->shaders.insert(std::pair<std::string, Shader>(shaderName, shader));
}


void	ShaderManager::addComputeShader(
						std::string shaderName,
						std::string cShaderFile)
{
	std::unordered_map<std::string, ComputeShader>::const_iterator	checkShader;

	checkShader = this->computeShaders.find(shaderName);

	// If shader exist, error
	if (checkShader != this->computeShaders.end())
		throw std::invalid_argument("Compute shader name '" + shaderName
										+ "' is already took");

	ComputeShader	computeShader(cShaderFile);

	this->computeShaders.insert(
			std::pair<std::string, ComputeShader>(shaderName, computeShader));
}


void	ShaderManager::loadWaterShaderFiles(
						std::string vShaderFile,
						std::string fShaderFile)
{
	this->waterShader.load(vShaderFile, fShaderFile);
}


void	ShaderManager::loadMenuShaderFiles(
						std::string vShaderFile,
						std::string fShaderFile)
{
	this->menuShader.load(vShaderFile, fShaderFile);
}

//**** PRIVATE METHODS *********************************************************
