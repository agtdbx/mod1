#ifndef WATERSIMULATION_HPP
# define WATERSIMULATION_HPP

# include <vector>
# include <unordered_map>

# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/maths/Vec3.hpp>
# include <model/Terrain.hpp>

class WaterSimulation
{
public:
	WaterSimulation(void);
	WaterSimulation(const WaterSimulation &obj);
	~WaterSimulation();

	int	getNbParticules(void);

	WaterSimulation	&operator=(const WaterSimulation &obj);

	void	addWater(glm::vec3 position);
	void	addWater(glm::vec3 position, glm::vec3 velociy);
	void	tick(ShaderManager *shaderManager, Terrain *terrain,
					t_performanceLog *perfLog, float delta);
	void	draw(Camera *camera, ShaderManager *shaderManager,
					Terrain *terrain, glm::vec3 *waterColor, float waterDensity);
	void	clear(void);

private:
	std::vector<glm::vec4>	positions;
	std::vector<glm::vec4>	predictedPositions;
	std::vector<glm::vec4>	velocities;
	std::vector<float>		densities;
	std::vector<std::vector<int>> grid;
	std::vector<float>		gridFlat, gridOffsets;
	bool					needToUpdateBuffers;
	float					triangleOverScreen[12];
	int						nbParticules, gridSize, gridW, gridH, gridD,
							gridFlatSize, gridOffsetsSize, idHsize, numGroups,
							mapDensityW, mapDensityH, mapDensityD, mapDensityIdHsize, mapDensitySize,
							numGroupsPutInGrid, numGroupsMapDensity;
	GLuint					textureBufferPositions, texturePositions,
							textureBufferPredictedPositions, texturePredictedPositions,
							textureBufferVelocities, textureVelocities,
							textureBufferDensities, textureDensities,
							textureBufferMapDensities, textureMapDensities,
							textureBufferGridFlat, textureGridFlat,
							textureBufferGridOffsets, textureGridOffsets,
							ssboGridTmp;

	void		generateTextureBuffer(void);
	void		generateTriangleOverScreen(void);
	void		generateOffsetGrid(void);
	void		generateFlatGrid(void);
	void		generateMapDensity(void);

	void		positionsToBuffer(void);
	void		positionsFromBuffer(void);
	void		predictedPositionsToBuffer(void);
	void		predictedPositionsFromBuffer(void);
	void		velocitiesToBuffer(void);
	void		velocitiesFromBuffer(void);
	void		densitiesToBuffer(void);
	void		densitiesFromBuffer(void);
	void		gridFlatToBuffer(void);
	void		gridOffsetsToBuffer(void);

	void		computePredictedPositions(ShaderManager *shaderManager, float delta);
	void		putParticlesInGrid(ShaderManager *shaderManager);
	void		computeDensity(ShaderManager *shaderManager);
	void		computeMapDensity(ShaderManager *shaderManager);
	void		calculatesAndApplyPressure(ShaderManager *shaderManager, float delta);
	void		updatePositions(ShaderManager *shaderManager, Terrain *terrain, float delta);
};

#endif
