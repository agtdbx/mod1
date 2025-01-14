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

	GLuint	getTextureBufferTerrainGridData(void);
	GLuint	getTextureTerrainGridData(void);
	GLuint	getTextureBufferTerrainGridFlat(void);
	GLuint	getTextureTerrainGridFlat(void);
	int		getSizeTerrainGridFlat(void);
	GLuint	getTextureBufferTerrainGridOffsets(void);
	GLuint	getTextureTerrainGridOffsets(void);
	int		getSizeTerrainGridOffsets(void);
	void	getGridSize(int sizes[3]);

	Terrain	&operator=(const Terrain &obj);

	void	loadFromFile(char *filePath);
	void	renderMesh(Camera *camera, ShaderManager *shaderManager);

private:
	std::vector<Vec3>					parameterPoints;
	std::vector<std::vector<double>>	heightmap;
	std::vector<Point>					vertices;
	std::vector<t_rectangle>			rectangles;
	Mesh								mesh;
	int									terrainGridW, terrainGridH, terrainGridD,
										terrainIdHsize, flatTerrainGridSize, offsetsTerrainGridSize;
	GLuint								textureBufferTerrainGridData, textureTerrainGridData,
										textureBufferTerrainGridFlat, textureTerrainGridFlat,
										textureBufferTerrainGridOffsets, textureTerrainGridOffsets;

	void	initEmptyMap(void);
	void	interpolate(void);
	void	createMesh(void);
	void	generateGridTextures(void);
};

#endif
