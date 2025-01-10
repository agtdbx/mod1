#ifndef TERRAIN_HPP
# define TERRAIN_HPP

# include <define.hpp>
# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/render/TextureManager.hpp>

typedef struct s_rectangle
{
	int	x;
	int	y;
	int	width;
	int	height;
}	t_rectangle;

class Terrain
{
public:
	Terrain(void);
	Terrain(const Terrain &obj);
	~Terrain();

	LUuint	getTextureBufferTerrainGridFlat(void);
	LUuint	getTextureTerrainGridFlat(void);
	LUuint	getTextureBufferTerrainGridOffsets(void);
	LUuint	getTextureTerrainGridOffsets(void);

	Terrain	&operator=(const Terrain &obj);

	void	loadFromFile(char *filePath);
	void	renderMesh(Camera *camera, ShaderManager *shaderManager);

private:
	std::vector<Vec3>					parameterPoints;
	std::vector<std::vector<double>>	heightmap;
	std::vector<t_rectangle>			rectangles;
	Mesh								mesh;
	int									terrainGridW, terrainGridH, terrainGridD,
										terrainIdHsize;
	GLuint								textureBufferTerrainGridFlat, textureTerrainGridFlat,
										textureBufferTerrainGridOffsets, textureTerrainGridOffsets;

	void	initEmptyMap(void);
	void	interpolate(void);
	void	createMesh(void);
	void	generateGridTextures(void);
};

#endif
