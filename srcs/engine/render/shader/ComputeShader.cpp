#include <engine/render/shader/ComputeShader.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

ComputeShader::ComputeShader(void)
{
	this->id = -1;
}


ComputeShader::ComputeShader(std::string cShaderPath)
{
	this->id = -1;
	this->load(cShaderPath);
}


ComputeShader::ComputeShader(const ComputeShader &obj)
{
	this->id = obj.id;
}

//---- Destructor --------------------------------------------------------------

ComputeShader::~ComputeShader()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

unsigned int	ComputeShader::getShaderId(void)
{
	return (this->id);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

ComputeShader	&ComputeShader::operator=(const ComputeShader &obj)
{
	if (this == &obj)
		return (*this);

	this->id = obj.id;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	ComputeShader::load(std::string cShaderPath)
{
	std::string			shaderCode;
	std::ifstream		file;
	std::stringstream	stream;

	file.open(cShaderPath);
	if (!file.is_open())
		throw std::invalid_argument("Compute shader file not found");
	stream << file.rdbuf();
	shaderCode = stream.str();
	file.close();

	const char		*cShaderCode = shaderCode.c_str();
	unsigned int	computeShader;
	int				success;
	char			infoLog[512] = {0};

	// compute shader
	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &cShaderCode, NULL);
	glCompileShader(computeShader);

	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		printf("tkt %s\n", infoLog);
		std::string	infoString(infoLog);
		throw std::invalid_argument("Compute shader compilation failed : " + infoString);
	}

	// shader Program
	this->id = glCreateProgram();
	glAttachShader(this->id, computeShader);
	glLinkProgram(this->id);

	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		std::string	infoString(infoLog);
		throw std::invalid_argument("Shader program linking failed : " + infoString);
	}

	glDeleteShader(computeShader);
}


void	ComputeShader::use(void)
{
	if (this->id < 0)
		return ;

	// Select the program shader as active one
	glUseProgram(this->id);
}

//**** PRIVATE METHODS *********************************************************
