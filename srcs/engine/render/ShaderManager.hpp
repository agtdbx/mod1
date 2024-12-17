#ifndef SHADERMANAGER_HPP
# define SHADERMANAGER_HPP

# include <string>
# include <unordered_map>

# include <engine/render/Shader.hpp>

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager();

	Shader			*getShader(std::string shaderName);
	unsigned int	getVAOId(void);

	ShaderManager	&operator=(const ShaderManager &obj);

	void	addShader(
				std::string shaderName,
				std::string vShaderFile,
				std::string fShaderFile);

private:
	unsigned int							VAO, VBO, EBO;
	std::unordered_map<std::string, Shader>	shaders;
};

#endif
