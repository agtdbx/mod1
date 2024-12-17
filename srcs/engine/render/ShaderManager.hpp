#ifndef SHADERMANAGER_HPP
# define SHADERMANAGER_HPP

# include <engine/render/Shader.hpp>

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager();

	Shader			*getTerrainShader(void);
	Shader			*getWaterShader(void);
	unsigned int	getVAOId(void);

	ShaderManager	&operator=(const ShaderManager &obj);

	void	loadShaderFiles(void);

private:
	unsigned int	VAO, VBO, EBO;
	Shader			terrainShader, waterShader;
};

#endif
