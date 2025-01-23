#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>
#include <engine/render/shader/ShaderFunctions.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;

	int smoothing_radius = SMOOTHING_RADIUS;
	int render_cell_size = RENDER_CELL_SIZE;

	// Grid init
	this->gridW = MAP_SIZE / smoothing_radius;
	if (MAP_SIZE > smoothing_radius && MAP_SIZE % smoothing_radius != 0)
		this->gridW++;
	this->gridH = MAP_MAX_HEIGHT / smoothing_radius;
	if (MAP_MAX_HEIGHT > smoothing_radius
		&& (int)MAP_MAX_HEIGHT % smoothing_radius != 0)
		this->gridH++;
	this->gridD = this->gridW;
	this->idHsize = this->gridW * this->gridD;
	this->gridSize = this->gridW * this->gridH * this->gridD;
	this->gridFlatSize = 0;
	this->gridOffsetsSize = 0;

	for (int i = 0; i < this->gridSize; i++)
	{
		std::vector<int>	gridContent;
		this->grid.push_back(gridContent);
	}

	// Render grid init
	this->renderGridW = MAP_SIZE / render_cell_size;
	if (MAP_SIZE > render_cell_size && MAP_SIZE % render_cell_size != 0)
		this->renderGridW++;
	this->renderGridH = MAP_MAX_HEIGHT / render_cell_size;
	if (MAP_MAX_HEIGHT > render_cell_size
		&& (int)MAP_MAX_HEIGHT % render_cell_size != 0)
		this->renderGridH++;
	this->renderGridD = this->renderGridW;
	this->renderIdHsize = this->renderGridW * this->renderGridD;
	this->renderGridSize = this->renderGridW * this->renderGridH * this->renderGridD;
	this->renderGridFlatSize = 0;
	this->renderGridOffsetsSize = 0;

	for (int i = 0; i < this->renderGridSize; i++)
	{
		std::vector<int>	rGridContent;
		this->renderGrid.push_back(rGridContent);
	}

	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->predictedPositions = obj.predictedPositions;
	this->velocities = obj.velocities;
	this->densities = obj.densities;
	this->grid = obj.grid;
	this->gridSize = obj.gridSize;
	this->gridW = obj.gridW;
	this->gridH = obj.gridH;
	this->gridD = obj.gridD;
	this->gridFlatSize = obj.gridFlatSize;
	this->gridOffsetsSize = obj.gridOffsetsSize;
	this->gridFlat = obj.gridFlat;
	this->gridOffsets = obj.gridOffsets;
	this->numGroups = obj.numGroups;
	this->needToUpdateBuffers = obj.needToUpdateBuffers;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{
	glDeleteBuffers(1, &this->textureBufferPositions);
	glDeleteTextures(1, &this->texturePositions);

	glDeleteBuffers(1, &this->textureBufferPredictedPositions);
	glDeleteTextures(1, &this->texturePredictedPositions);

	glDeleteBuffers(1, &this->textureBufferVelocities);
	glDeleteTextures(1, &this->textureVelocities);

	glDeleteBuffers(1, &this->textureBufferDensities);
	glDeleteTextures(1, &this->textureDensities);

	glDeleteBuffers(1, &this->textureBufferGridFlat);
	glDeleteTextures(1, &this->textureGridFlat);

	glDeleteBuffers(1, &this->textureBufferGridOffsets);
	glDeleteTextures(1, &this->textureGridOffsets);

	glDeleteBuffers(1, &this->textureBufferRenderGridFlat);
	glDeleteTextures(1, &this->textureRenderGridFlat);

	glDeleteBuffers(1, &this->textureBufferRenderGridOffsets);
	glDeleteTextures(1, &this->textureRenderGridOffsets);
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

int	WaterSimulation::getNbParticules(void)
{
	return (this->nbParticules);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

WaterSimulation	&WaterSimulation::operator=(const WaterSimulation &obj)
{
	if (this == &obj)
		return (*this);

	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->predictedPositions = obj.predictedPositions;
	this->velocities = obj.velocities;
	this->densities = obj.densities;
	this->grid = obj.grid;
	this->gridSize = obj.gridSize;
	this->gridW = obj.gridW;
	this->gridH = obj.gridH;
	this->gridD = obj.gridD;
	this->gridFlatSize = obj.gridFlatSize;
	this->gridOffsetsSize = obj.gridOffsetsSize;
	this->gridFlat = obj.gridFlat;
	this->gridOffsets = obj.gridOffsets;
	this->numGroups = obj.numGroups;
	this->needToUpdateBuffers = obj.needToUpdateBuffers;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(glm::vec3 position)
{
	if (this->needToUpdateBuffers == false)
	{
		this->positionsFromBuffer();
		this->predictedPositionsFromBuffer();
		this->velocitiesFromBuffer();
		this->densitiesFromBuffer();
	}

	this->positions.push_back(glm::vec4(position, 0.0f));
	this->predictedPositions.push_back(glm::vec4(position, 0.0f));
	this->velocities.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	this->densities.push_back(0.0);
	this->nbParticules++;
	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;
}


void	WaterSimulation::addWater(glm::vec3 position, glm::vec3 velocity)
{
	if (this->needToUpdateBuffers == false)
	{
		this->positionsFromBuffer();
		this->predictedPositionsFromBuffer();
		this->velocitiesFromBuffer();
		this->densitiesFromBuffer();
	}

	this->positions.push_back(glm::vec4(position, 0.0f));
	this->predictedPositions.push_back(glm::vec4(position, 0.0f));
	this->velocities.push_back(glm::vec4(velocity, 0.0f));
	this->densities.push_back(0.0);
	this->nbParticules++;
	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;
}


void	WaterSimulation::tick(ShaderManager *shaderManager, Terrain *terrain, float delta)
{
	if (this->needToUpdateBuffers == true)
	{
		this->positionsToBuffer();
		this->predictedPositionsToBuffer();
		this->velocitiesToBuffer();
		this->densitiesToBuffer();

		this->needToUpdateBuffers = false;
	}

	this->computePredictedPositions(shaderManager, delta); // gpu
	this->putParticlesInGrid(shaderManager); // cpu
	this->computeDensity(shaderManager); // gpu
	this->calculatesAndApplyPressure(shaderManager, delta); // gpu
	this->updatePositions(shaderManager, terrain, delta); // gpu
}


void	WaterSimulation::draw(
			Camera *camera,
			ShaderManager *shaderManager,
			Terrain *terrain,
			glm::vec3 *waterColor)
{
	WaterShader *shader;
	int			shaderId;
	int			sizes[4], terrainFlatGridSize, terrainOffsetsSize;
	GLuint		terrainBufferTextureDataGrid, terrainTextureDataGrid,
				terrainBufferTextureFlatGrid, terrainTextureFlatGrid,
				terrainBufferTextureOffsets, terrainTextureOffsets;

	// if (this->nbParticules == 0)
	// 	return ;

	// Get terrain data
	terrainBufferTextureDataGrid = terrain->getTextureBufferTerrainGridData();
	terrainTextureDataGrid = terrain->getTextureTerrainGridData();
	terrainBufferTextureFlatGrid = terrain->getTextureBufferTerrainGridFlat();
	terrainTextureFlatGrid = terrain->getTextureTerrainGridFlat();
	terrainFlatGridSize = terrain->getSizeTerrainGridFlat();
	terrainBufferTextureOffsets = terrain->getTextureBufferTerrainGridOffsets();
	terrainTextureOffsets = terrain->getTextureTerrainGridOffsets();
	terrainOffsetsSize = terrain->getSizeTerrainGridOffsets();
	terrain->getGridSize(sizes);

	// Give triangles to water shader
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12,
					this->triangleOverScreen, GL_STATIC_DRAW);

	// Use water shader
	shader = shaderManager->getWaterShader();
	shader->use();
	shaderId = shader->getShaderId();

	// Give parameters to shaders
	giveVec3ToShader(shaderId, "lightPos", camera->getLightPosition());
	giveVec3ToShader(shaderId, "cameraPos", camera->getPosition());
	giveVec3ToShader(shaderId, "cameraFront", camera->getFront());
	giveVec3ToShader(shaderId, "cameraRight", camera->getRight());
	giveVec3ToShader(shaderId, "cameraUp", camera->getUp());
	giveFloatToShader(shaderId, "cameraFOV", CAMERA_FOV);
	giveFloatToShader(shaderId, "cameraFar", CAMERA_MAX_VIEW_DIST);
	giveFloatToShader(shaderId, "planeWidth", camera->getPlaneWidth());
	giveFloatToShader(shaderId, "planeHeight", camera->getPlaneHeight());
	giveFloatToShader(shaderId, "rayStep", RAY_STEP);
	giveFloatToShader(shaderId, "waterRadius", WATER_RADIUS);
	giveFloatToShader(shaderId, "waterMaxXZ", WATER_MAX_XZ);
	giveFloatToShader(shaderId, "waterMaxY", WATER_MAX_HEIGHT);
	giveVec3ToShader(shaderId, "waterColor", *waterColor);

	giveIntToShader(shaderId, "gridW", this->gridW);
	giveIntToShader(shaderId, "gridH", this->gridH);
	giveIntToShader(shaderId, "gridD", this->gridD);
	giveIntToShader(shaderId, "idHsize", this->idHsize);
	giveIntToShader(shaderId, "gridSize", this->gridFlatSize);
	giveIntToShader(shaderId, "offsetsSize", this->gridOffsetsSize);
	giveFloatTextureToShader(shaderId, "gridBuffer", 0,
								this->textureBufferGridFlat,
								this->textureGridFlat);
	giveFloatTextureToShader(shaderId, "offsetsBuffer", 1,
								this->textureBufferGridOffsets,
								this->textureGridOffsets);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 2,
								this->textureBufferPositions,
								this->texturePositions);
	giveFloatTextureToShader(shaderId, "densitiesBuffer", 3,
								this->textureBufferDensities,
								this->textureDensities);

	giveFloatToShader(shaderId, "smoothingRadius", SMOOTHING_RADIUS); // TODO: CHECK IF THEY CAN BE REMOVE
	giveFloatToShader(shaderId, "smoothingScale", SMOOTHING_SCALE);
	giveFloatToShader(shaderId, "waterMass", WATER_MASS);

	giveIntToShader(shaderId, "terrainCellSize", TERRAIN_CELL_SIZE);
	giveIntToShader(shaderId, "terrainGridW", sizes[0]);
	giveIntToShader(shaderId, "terrainGridH", sizes[1]);
	giveIntToShader(shaderId, "terrainGridD", sizes[2]);
	giveIntToShader(shaderId, "terrainIdHsize", sizes[3]);
	giveIntToShader(shaderId, "terrainGridSize", terrainFlatGridSize);
	giveIntToShader(shaderId, "terrainOffsetsSize", terrainOffsetsSize);
	giveIntToShader(shaderId, "terrainNbRectangles", terrain->getDataNbRectangles());
	giveVec3TextureToShader(shaderId, "terrainDataBuffer", 4,
								terrainBufferTextureDataGrid,
								terrainTextureDataGrid);
	giveFloatTextureToShader(shaderId, "terrainGridBuffer", 5,
								terrainBufferTextureFlatGrid,
								terrainTextureFlatGrid);
	giveFloatTextureToShader(shaderId, "terrainOffsetsBuffer", 6,
								terrainBufferTextureOffsets,
								terrainTextureOffsets);

	// Draw triangles
	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}


void	WaterSimulation::drawTest(
			Camera *camera,
			ShaderManager *shaderManager,
			Terrain *terrain,
			glm::vec3 *waterColor)
{
	WaterShader *shader;
	int			shaderId;

	if (this->nbParticules == 0)
		return ;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12,
					this->triangleOverScreen, GL_STATIC_DRAW);

	shader = shaderManager->getWaterShader();
	shader->use();
	shaderId = shader->getShaderId();

	giveVec3ToShader(shaderId, "lightPos", camera->getLightPosition());
	giveVec3ToShader(shaderId, "cameraPos", camera->getPosition());
	giveVec3ToShader(shaderId, "cameraFront", camera->getFront());
	giveVec3ToShader(shaderId, "cameraRight", camera->getRight());
	giveVec3ToShader(shaderId, "cameraUp", camera->getUp());
	giveVec3ToShader(shaderId, "waterColor", *waterColor);
	giveFloatToShader(shaderId, "cameraFOV", CAMERA_FOV);
	giveFloatToShader(shaderId, "cameraFar", CAMERA_MAX_VIEW_DIST);
	giveFloatToShader(shaderId, "planeWidth", camera->getPlaneWidth());
	giveFloatToShader(shaderId, "planeHeight", camera->getPlaneHeight());
	giveFloatToShader(shaderId, "waterRadius2", WATER_RADIUS2);
	giveFloatToShader(shaderId, "renderCellSize", RENDER_CELL_SIZE);
	giveIntToShader(shaderId, "nbPositions", this->nbParticules);
	giveIntToShader(shaderId, "mapSize", MAP_SIZE);
	giveIntToShader(shaderId, "mapHeight", WATER_MAX_HEIGHT);
	giveIntToShader(shaderId, "renderGridW", this->renderGridW);
	giveIntToShader(shaderId, "renderGridH", this->renderGridH);
	giveIntToShader(shaderId, "renderGridD", this->renderGridD);
	giveIntToShader(shaderId, "renderGridSize", this->renderGridFlatSize);
	giveIntToShader(shaderId, "renderOffsetsSize", this->renderGridOffsetsSize);
	giveVec4TextureToShader(shaderId, "positionsBuffer", 0,
							this->textureBufferPositions, this->texturePositions);
	giveFloatTextureToShader(shaderId, "renderGridBuffer", 1,
							this->textureBufferRenderGridFlat,
							this->textureRenderGridFlat);
	giveFloatTextureToShader(shaderId, "renderOffsetsBuffer", 2,
							this->textureBufferRenderGridOffsets,
							this->textureRenderGridOffsets);

	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}


void	WaterSimulation::clear(void)
{
	this->nbParticules = 0;
	this->positions.clear();
	this->predictedPositions.clear();
	this->velocities.clear();
	this->densities.clear();
	this->needToUpdateBuffers = true;
}


//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::generateTriangleOverScreen(void)
{
	// triangle 1 : p1 p2 p3
	// triangle 2 : p4 p5 p6

	// p1
	this->triangleOverScreen[0] = -1.0f;
	this->triangleOverScreen[1] = -1.0f;
	// p2
	this->triangleOverScreen[2] = -1.0f;
	this->triangleOverScreen[3] = 1.0f;
	// p3
	this->triangleOverScreen[4] = 1.0f;
	this->triangleOverScreen[5] = 1.0f;
	// p4
	this->triangleOverScreen[6] = -1.0f;
	this->triangleOverScreen[7] = -1.0f;
	// p5
	this->triangleOverScreen[8] = 1.0f;
	this->triangleOverScreen[9] = 1.0f;
	// p6
	this->triangleOverScreen[10] = 1.0f;
	this->triangleOverScreen[11] = -1.0f;
}
