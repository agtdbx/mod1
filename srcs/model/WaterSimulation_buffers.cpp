#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>
#include <engine/render/shader/ShaderFunctions.hpp>

//**** STATIC VARIABLES ********************************************************


//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::generateTextureBuffer(void)
{
	glGenBuffers(1, &this->textureBufferPositions);
	glGenTextures(1, &this->texturePositions);

	glGenBuffers(1, &this->textureBufferPredictedPositions);
	glGenTextures(1, &this->texturePredictedPositions);

	glGenBuffers(1, &this->textureBufferVelocities);
	glGenTextures(1, &this->textureVelocities);

	glGenBuffers(1, &this->textureBufferDensities);
	glGenTextures(1, &this->textureDensities);

	glGenBuffers(1, &this->textureBufferMapDensities);
	glGenTextures(1, &this->textureMapDensities);

	glGenBuffers(1, &this->textureBufferGridFlat);
	glGenTextures(1, &this->textureGridFlat);

	glGenBuffers(1, &this->textureBufferGridOffsets);
	glGenTextures(1, &this->textureGridOffsets);

	glGenBuffers(1, &this->ssboGridTmp);
}


void	WaterSimulation::generateOffsetGrid(void)
{
	// Default value for offsets
	this->gridOffsets.clear();
	this->gridOffsetsSize = this->gridSize;
	for (int i = 0; i < this->gridOffsetsSize; i++)
		this->gridOffsets.push_back(0.0f);
	this->gridOffsetsToBuffer();

	// Grid tmp
	int	size = this->gridSize * 2500;
	int	bufferSize = sizeof(int) * size;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ssboGridTmp);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	std::vector<int> initialData(size, 0);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, initialData.data());
}


void	WaterSimulation::generateFlatGrid(void)
{
	// Default value of flat grid
	this->gridFlat.clear();
	this->gridFlatSize = this->nbParticules;
	for (int i = 0; i < this->gridFlatSize; i++)
		this->gridFlat.push_back(0.0f);
	this->gridFlatToBuffer();
}


void	WaterSimulation::generateMapDensity(void)
{
	std::vector<float> mapDensities;

	for (int i = 0; i < this->mapDensitySize; i++)
		mapDensities.push_back(0.0f);

	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferMapDensities);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->mapDensitySize,
					mapDensities.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::positionsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPositions);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * this->nbParticules,
					this->positions.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::positionsFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPositions);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * this->nbParticules,
						this->positions.data());
}


void	WaterSimulation::predictedPositionsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPredictedPositions);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * this->nbParticules,
					this->predictedPositions.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::predictedPositionsFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPredictedPositions);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * this->nbParticules,
						this->predictedPositions.data());
}


void	WaterSimulation::velocitiesToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferVelocities);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * this->nbParticules,
					this->velocities.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::velocitiesFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferVelocities);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(glm::vec4) * this->nbParticules,
						this->velocities.data());
}


void	WaterSimulation::densitiesToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferDensities);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->nbParticules,
					this->densities.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::densitiesFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferDensities);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(float) * this->nbParticules,
						this->densities.data());
}


void	WaterSimulation::gridFlatToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridFlat);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridFlatSize,
					this->gridFlat.data(), GL_STATIC_DRAW);
}


void	WaterSimulation::gridOffsetsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridOffsetsSize,
					this->gridOffsets.data(), GL_STATIC_DRAW);
}
