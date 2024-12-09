#ifndef SHADER_HPP
# define SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

class Shader
{
public:
	Shader(void);
	Shader(std::string vShaderPath, std::string fShaderPath);
	Shader(const Shader &obj);
	~Shader();

	Shader	&operator=(const Shader &obj);

	void	use();

private:
	unsigned int	id;

	void			allocateMemory(void);
	unsigned int	getVertexShader(std::string shaderPath);
	unsigned int	getFragmentShader(std::string shaderPath);
};

#endif
