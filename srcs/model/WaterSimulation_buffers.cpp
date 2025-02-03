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

	glGenBuffers(1, &this->textureBufferPressures);
	glGenTextures(1, &this->texturePressures);

	glGenBuffers(1, &this->textureBufferMapDensities);
	glGenTextures(1, &this->textureMapDensities);

	glDeleteBuffers(1, &this->textureBufferMapPressures);
	glDeleteTextures(1, &this->textureMapPressures);

	glGenBuffers(1, &this->ssboGrid1);
	glGenBuffers(1, &this->ssboGrid2);
}


void	WaterSimulation::generateGridBuffer(void)
{
	// Grid buffer size
	int	size = this->gridSize * 1001;
	int	bufferSize = sizeof(int) * size;
	std::vector<int> initialData(size, 0);

	// Grid buffer 1 init
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ssboGrid1);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, initialData.data());

	// Grid buffer 2 init
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ssboGrid2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, initialData.data());

	this->ssboGrid1Used = true;
}


void	WaterSimulation::generateMapBuffer(void)
{
	std::vector<float>		mapBufferFloat;
	std::vector<glm::vec4>	mapBufferVec4;

	for (int i = 0; i < this->mapBufferSize; i++)
	{
		mapBufferFloat.push_back(0.0f);
		mapBufferVec4.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	}

	// Map densities
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferMapDensities);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->mapBufferSize,
					mapBufferFloat.data(), GL_STATIC_DRAW);

	// Map pressures
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferMapPressures);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->mapBufferSize,
					mapBufferFloat.data(), GL_STATIC_DRAW);
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


void	WaterSimulation::pressuresToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPressures);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->nbParticules,
					this->densities.data(), GL_STATIC_DRAW);
}
