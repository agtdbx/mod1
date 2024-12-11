#ifndef TERRAIN_HPP
# define TERRAIN_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/Shader.hpp>
# include <engine/render/TextureManager.hpp>

class Terrain
{
public:
	Terrain(void);
	Terrain(const Terrain &obj);
	~Terrain();

	Terrain	&operator=(const Terrain &obj);

	void	loadFromFile(char *filePath);
	void	renderMesh(Camera *camera, Shader *shader, TextureManager *textureManager, std::string textureName);

private:
	std::vector<Vec3>					parameterPoints;
	std::vector<std::vector<double>>	heightmap;
	Mesh								mesh;

	void	initEmptyMap(void);
	void	interpolate(void);
	void	createMesh(void);
};

#endif
