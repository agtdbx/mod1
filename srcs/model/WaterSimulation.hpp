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
					t_performanceLog *perfLog, float delta, t_holeInfo *holeInfo);
	void	draw(Camera *camera, ShaderManager *shaderManager,
					Terrain *terrain, glm::vec3 *waterColor,glm::vec3 *lightColor, float waterDensity);
	void	clear(void);
	void	removeHoledParticles(void);

private:
	std::vector<glm::vec4>	positions;
	std::vector<glm::vec4>	predictedPositions;
	std::vector<glm::vec4>	velocities;
	std::vector<float>		densities;
	bool					needToUpdateBuffers, ssboGrid1Used;
	float					triangleOverScreen[12];
	int						nbParticules, gridSize, gridW, gridH, gridD,
							idHsize, numGroups,
							mapBufferW, mapBufferH, mapBufferD, mapBufferIdHsize,
							mapBufferSize, numGroupsPutInGrid, numGroupsMapBuffer;
	GLuint					textureBufferPositions, texturePositions,
							textureBufferPredictedPositions, texturePredictedPositions,
							textureBufferVelocities, textureVelocities,
							textureBufferDensities, textureDensities,
							textureBufferPressures, texturePressures,
							textureBufferMapDensities, textureMapDensities,
							ssboCurrent, ssboGrid1, ssboGrid2;

	void		generateTextureBuffer(void);
	void		generateTriangleOverScreen(void);
	void		generateGridBuffer(void);
	void		generateMapBuffer(void);

	void		positionsToBuffer(void);
	void		positionsFromBuffer(void);
	void		predictedPositionsToBuffer(void);
	void		predictedPositionsFromBuffer(void);
	void		velocitiesToBuffer(void);
	void		velocitiesFromBuffer(void);
	void		densitiesToBuffer(void);
	void		densitiesFromBuffer(void);
	void		pressuresToBuffer(void);

	void		computePredictedPositions(ShaderManager *shaderManager,
											float delta, t_holeInfo *holeInfo);
	void		putParticlesInGrid(ShaderManager *shaderManager);
	void		putParticlesInGridInParallel(ShaderManager *shaderManager);
	void		computeMapDensity(ShaderManager *shaderManager);
	void		computeDensity(ShaderManager *shaderManager);
	void		computeMapPressureAcceleration(ShaderManager *shaderManager);
	void		calculatesAndApplyPressure(ShaderManager *shaderManager, float deltal, t_holeInfo *holeInfo);
	void		calculatesAndApplyViscosity(ShaderManager *shaderManager, float delta);
	void		updatePositions(ShaderManager *shaderManager, Terrain *terrain,
									float delta, t_holeInfo *holeInfo);
};

#endif
