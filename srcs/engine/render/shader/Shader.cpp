#include <engine/render/shader/Shader.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Shader::Shader(void)
{
	this->id = -1;
}


Shader::Shader(std::string vShaderPath, std::string fShaderPath)
{
	this->id = -1;

	this->load(vShaderPath, fShaderPath);
}


Shader::Shader(const Shader &obj)
{
	this->id = obj.id;
}

//---- Destructor --------------------------------------------------------------

Shader::~Shader()
{
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

unsigned int	Shader::getShaderId(void)
{
	return (this->id);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Shader	&Shader::operator=(const Shader &obj)
{
	if (this == &obj)
		return (*this);

	this->id = obj.id;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Shader::load(std::string vShaderPath, std::string fShaderPath)
{
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
	if(!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		std::string	infoString(infoLog);
		throw new std::invalid_argument("Shader program linking failed : " + infoString);
	}

	// Delete base shader because there are useless now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void	Shader::use(void)
{
	if (this->id < 0)
		return ;

	// Give info about how get vertice to draw trianlges with gpu
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

	// Select the program shader as active one
	glUseProgram(this->id);
}

//**** PRIVATE METHODS *********************************************************

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
