#ifndef COMPUTESHADER_HPP
# define COMPUTESHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

class ComputeShader
{
public:
	ComputeShader(void);
	ComputeShader(std::string cShaderPath);
	ComputeShader(const ComputeShader &obj);
	~ComputeShader();

	unsigned int	getShaderId(void);

	ComputeShader	&operator=(const ComputeShader &obj);

	void	load(std::string cShaderPath);
	void	use(void);

private:
	unsigned int	id;

};

#endif
