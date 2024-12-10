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

	unsigned int	getShaderId(void);
	unsigned int	getVAOId(void);
	unsigned int	getVBOId(void);
	unsigned int	getEBOId(void);

	Shader	&operator=(const Shader &obj);

	void	load(std::string vShaderPath, std::string fShaderPath);
	void	use(void);

private:
	unsigned int	id, VAO, VBO, EBO;

	void			allocateMemory(void);
	unsigned int	getVertexShader(std::string shaderPath);
	unsigned int	getFragmentShader(std::string shaderPath);
};

#endif
