#include <engine/render/Shader.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Shader::Shader(void)
{
	this->id = -1;
	this->VAO = -1;
	this->VBO = -1;
	this->EBO = -1;
}


Shader::Shader(std::string vShaderPath, std::string fShaderPath)
{
	this->id = -1;
	this->VAO = -1;
	this->VBO = -1;
	this->EBO = -1;

	this->load(vShaderPath, fShaderPath);
}


Shader::Shader(const Shader &obj)
{
	this->id = obj.id;
	this->VAO = obj.VAO;
	this->VBO = obj.VBO;
	this->EBO = obj.EBO;
}

//---- Destructor --------------------------------------------------------------

Shader::~Shader()
{
	if (this->VAO >= 0)
		glDeleteVertexArrays(1, &this->VAO);
	if (this->VBO >= 0)
		glDeleteBuffers(1, &this->VBO);
	if (this->EBO >= 0)
		glDeleteBuffers(1, &this->EBO);
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

unsigned int	Shader::getShaderId(void)
{
	return (this->id);
}


unsigned int	Shader::getVAOId(void)
{
	return (this->VAO);
}


unsigned int	Shader::getVBOId(void)
{
	return (this->VBO);
}


unsigned int	Shader::getEBOId(void)
{
	return (this->EBO);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Shader	&Shader::operator=(const Shader &obj)
{
	if (this == &obj)
		return (*this);

	this->id = obj.id;
	this->VAO = obj.VAO;
	this->VBO = obj.VBO;
	this->EBO = obj.EBO;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Shader::load(std::string vShaderPath, std::string fShaderPath)
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

	this->allocateMemory();

	unsigned int vertexShader = this->getVertexShader(vShaderPath);
	unsigned int fragmentShader = this->getFragmentShader(fShaderPath);

	// Create shader program
	this->id = glCreateProgram();

	// Link vertex and fragment shader to shader program
	glAttachShader(this->id, vertexShader);
	glAttachShader(this->id, fragmentShader);

	// Link shader to gpu
	glLinkProgram(this->id);

	int success;
	// Check if link
	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if(!success) {
		char infoLog[512];
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		std::string	infoString(infoLog);
		throw new std::invalid_argument("Shader program linking failed : " + infoString);
	}

	// Give info about how get vertice to draw triange with gpu
	// (vertice offset, nb vertice, need to normalize point, size of array, thing for weird cast)
	// Point position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// Point normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Point color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Delete base shader because there are useless now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void	Shader::use(void)
{
	if (this->id < 0)
		return ;

	// Select the program shader as active one
	glUseProgram(this->id);
}

//**** PRIVATE METHODS *********************************************************

void	Shader::allocateMemory(void)
{
	// Allocate memory for vertice array into gpu
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// Allocate memory into gpu
	glGenBuffers(1, &this->VBO);
	// Set the new allocated buffer as array one
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	// Allocate buffer for indices
	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
}


unsigned int	Shader::getVertexShader(std::string shaderPath)
{
	std::string			shaderCode;
	std::ifstream		file;
	std::stringstream	stream;

	file.open(shaderPath);
	if (!file.is_open())
		throw std::invalid_argument("Vertex shader file not found");
	stream << file.rdbuf();
	shaderCode = stream.str();
	file.close();

	const char *vShaderCode = shaderCode.c_str();

	// Create vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Put shader code into variable
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	// And compile it
	glCompileShader(vertexShader);

	// Check if shader compilation succeed
	int  success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::string	infoString(infoLog);
		throw std::invalid_argument("Vertex shader compilation failed : " + infoString);
	}

	return (vertexShader);
}


unsigned int	Shader::getFragmentShader(std::string shaderPath)
{
	// Create fragment shader
	std::string			shaderCode;
	std::ifstream		file;
	std::stringstream	stream;

	file.open(shaderPath);
	if (!file.is_open())
		throw std::invalid_argument("Fragment shader file not found");
	stream << file.rdbuf();
	shaderCode = stream.str();
	file.close();

	const char *fShaderCode = shaderCode.c_str();

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

	// Check if shader compilation succeed
	int  success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::string	infoString(infoLog);
		throw std::invalid_argument("Fragment shader compilation failed : " + infoString);
	}

	return (fragmentShader);
}
