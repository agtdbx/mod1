#ifndef WATER_SHADER_HPP
# define WATER_SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

class WaterShader
{
public:
	WaterShader(void);
	WaterShader(std::string vShaderPath, std::string fShaderPath);
	WaterShader(const WaterShader &obj);
	~WaterShader();

	unsigned int	getShaderId(void);

	WaterShader	&operator=(const WaterShader &obj);

	void	load(std::string vShaderPath, std::string fShaderPath);
	void	use(void);

private:
	unsigned int	id;

	unsigned int	getVertexShader(std::string shaderPath);
	unsigned int	getFragmentShader(std::string shaderPath);
};

#endif
