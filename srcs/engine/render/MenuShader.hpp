#ifndef MENU_SHADER_HPP
# define MENU_SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

class MenuShader
{
public:
	MenuShader(void);
	MenuShader(std::string vShaderPath, std::string fShaderPath);
	MenuShader(const MenuShader &obj);
	~MenuShader();

	unsigned int	getShaderId(void);

	MenuShader	&operator=(const MenuShader &obj);

	void	load(std::string vShaderPath, std::string fShaderPath);
	void	use(void);

private:
	unsigned int	id;

	unsigned int	getVertexShader(std::string shaderPath);
	unsigned int	getFragmentShader(std::string shaderPath);
};

#endif
