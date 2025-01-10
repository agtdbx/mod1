#ifndef SHADERMANAGER_HPP
# define SHADERMANAGER_HPP

# include <string>
# include <unordered_map>

# include <engine/render/shader/Shader.hpp>
# include <engine/render/shader/WaterShader.hpp>
# include <engine/render/shader/ComputeShader.hpp>
# include <engine/render/MenuShader.hpp>

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager();

	Shader			*getShader(std::string shaderName);
	ComputeShader	*getComputeShader(std::string shaderName);
	WaterShader		*getWaterShader(void);
	MenuShader		*getMenuShader(void);
	unsigned int	getVAOId();
	unsigned int	getVBOId();
	unsigned int	getEBOId();

	ShaderManager	&operator=(const ShaderManager &obj);

	void	addShader(
				std::string shaderName,
				std::string vShaderFile,
				std::string fShaderFile);
	void	addComputeShader(
				std::string shaderName,
				std::string cShaderFile);
	void	loadWaterShaderFiles(
				std::string vShaderFile,
				std::string fShaderFile);
	void	loadMenuShaderFiles(
				std::string vShaderFile,
				std::string fShaderFile);

private:
	unsigned int									VAO, VBO, EBO;
	std::unordered_map<std::string, Shader>			shaders;
	std::unordered_map<std::string, ComputeShader>	computeShaders;
	WaterShader										waterShader;
	MenuShader								menuShader;
};

#endif
