#ifndef WATERSIMULATION_HPP
# define WATERSIMULATION_HPP

# include <vector>
# include <unordered_map>

# include <engine/render/Mesh.hpp>
# include <engine/render/Camera.hpp>
# include <engine/render/shader/ShaderManager.hpp>
# include <engine/maths/Vec3.hpp>

class WaterSimulation
{
public:
	WaterSimulation(void);
	WaterSimulation(const WaterSimulation &obj);
	~WaterSimulation();

	int	getNbParticules(void);

	WaterSimulation	&operator=(const WaterSimulation &obj);

	void	addWater(glm::vec3 position);
	void	tick(ShaderManager *shaderManager, float delta);
	void	draw(Camera *camera, ShaderManager *shaderManager);

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
							gridFlatSize, gridOffsetsSize, idHsize, numGroups;
	GLuint					textureBufferPositions, texturePositions,
							textureBufferPredictedPositions, texturePredictedPositions,
							textureBufferVelocities, textureVelocities,
							textureBufferDensities, textureDensities,
							textureBufferGridFlat, textureGridFlat,
							textureBufferGridOffsets, textureGridOffsets;

	void		generateTextureBuffer(void);
	void		generateTriangleOverScreen(void);
	void		generateFlatGrid(void);

	void		positionsToBuffer(void);
	void		positionsFromBuffer(void);
	void		predictedPositionsToBuffer(void);
	void		predictedPositionsFromBuffer(void);
	void		velocitiesToBuffer(void);
	void		velocitiesFromBuffer(void);
	void		densitiesToBuffer(void);
	void		densitiesFromBuffer(void);
	void		gridFlatToBuffer(void);
	void		gridFlatFromBuffer(void);
	void		gridOffsetsToBuffer(void);
	void		gridOffsetsFromBuffer(void);

	void		computePredictedPositions(ShaderManager *shaderManager, float delta);
	void		putParticlesInGrid(ShaderManager *shaderManager);
	void		computeDensity(ShaderManager *shaderManager);
	void		calculatesAndApplyPressure(ShaderManager *shaderManager, float delta);
	void		updatePositions(ShaderManager *shaderManager, float delta);
};

#endif
