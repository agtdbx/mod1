#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>
#include <engine/render/shader/ShaderFunctions.hpp>
#include <ctime>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;

	int smoothing_radius = SMOOTHING_RADIUS;

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
	this->numGroupsPutInGrid = (this->gridSize + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;

	// map density init
	this->mapDensityW = MAP_SIZE / MAP_DENSITY_CELL_SIZE;
	if (MAP_SIZE > MAP_DENSITY_CELL_SIZE && MAP_SIZE % MAP_DENSITY_CELL_SIZE != 0)
		this->mapDensityW++;
	this->mapDensityH = MAP_MAX_HEIGHT / MAP_DENSITY_CELL_SIZE;
	if (MAP_MAX_HEIGHT > MAP_DENSITY_CELL_SIZE
		&& (int)MAP_MAX_HEIGHT % MAP_DENSITY_CELL_SIZE != 0)
		this->mapDensityH++;
	this->mapDensityD = this->mapDensityW;
	this->mapDensityIdHsize = this->mapDensityW * this->mapDensityD;
	this->mapDensitySize = this->mapDensityW * this->mapDensityH * this->mapDensityD;
	this->numGroupsMapDensity = (this->mapDensitySize + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;


	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
	this->generateMapDensity();
	this->generateOffsetGrid();
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->predictedPositions = obj.predictedPositions;
	this->velocities = obj.velocities;
	this->densities = obj.densities;
	this->gridSize = obj.gridSize;
	this->gridW = obj.gridW;
	this->gridH = obj.gridH;
	this->gridD = obj.gridD;
	this->numGroupsPutInGrid = obj.numGroupsPutInGrid;
	this->mapDensityW = obj.mapDensityW;
	this->mapDensityH = obj.mapDensityH;
	this->mapDensityD = obj.mapDensityD;
	this->mapDensityIdHsize = obj.mapDensityIdHsize;
	this->mapDensitySize = obj.mapDensitySize;
	this->numGroupsMapDensity = obj.numGroupsMapDensity;
	this->numGroups = obj.numGroups;
	this->needToUpdateBuffers = true;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
	this->generateMapDensity();
	this->generateOffsetGrid();
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

	glDeleteBuffers(1, &this->textureBufferPressures);
	glDeleteTextures(1, &this->texturePressures);

	glDeleteBuffers(1, &this->textureBufferMapDensities);
	glDeleteTextures(1, &this->textureMapDensities);

	glDeleteBuffers(1, &this->ssboGrid);
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
	this->gridSize = obj.gridSize;
	this->gridW = obj.gridW;
	this->gridH = obj.gridH;
	this->gridD = obj.gridD;
	this->numGroupsPutInGrid = obj.numGroupsPutInGrid;
	this->mapDensityW = obj.mapDensityW;
	this->mapDensityH = obj.mapDensityH;
	this->mapDensityD = obj.mapDensityD;
	this->mapDensityIdHsize = obj.mapDensityIdHsize;
	this->mapDensitySize = obj.mapDensitySize;
	this->numGroupsMapDensity = obj.numGroupsMapDensity;
	this->numGroups = obj.numGroups;
	this->needToUpdateBuffers = true;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(glm::vec3 position)
{
	if (this->nbParticules >= NB_MAX_PARTICLES)
		return ;

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
	if (this->nbParticules >= NB_MAX_PARTICLES)
		return ;

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


void	WaterSimulation::tick(
			ShaderManager *shaderManager,
			Terrain *terrain,
			t_performanceLog *perfLog,
			float delta)
{
	if (this->needToUpdateBuffers == true)
	{
		this->positionsToBuffer();
		this->predictedPositionsToBuffer();
		this->velocitiesToBuffer();
		this->densitiesToBuffer();
		this->pressuresToBuffer();

		this->needToUpdateBuffers = false;
	}

	if (perfLog->moreStats)
	{
		GLuint query;
		GLuint64 elapsed_time = 0;
		glGenQueries(1, &query);

		glBeginQuery(GL_TIME_ELAPSED, query);
		this->computePredictedPositions(shaderManager, delta); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timePredictedPos += elapsed_time / 1000000000.0;

		glBeginQuery(GL_TIME_ELAPSED, query);
		// this->putParticlesInGrid(shaderManager); // gpu
		this->putParticlesInGridInParallel(shaderManager); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timePutInGrid += elapsed_time / 1000000000.0;

		glBeginQuery(GL_TIME_ELAPSED, query);
		this->computeDensity(shaderManager); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timeComputeDensity += elapsed_time / 1000000000.0;

		glBeginQuery(GL_TIME_ELAPSED, query);
		this->computeMapDensity(shaderManager); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timeComputeMapDensity += elapsed_time / 1000000000.0;

		glBeginQuery(GL_TIME_ELAPSED, query);
		this->calculatesAndApplyPressure(shaderManager, delta); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timeApplyPressure += elapsed_time / 1000000000.0;

		glBeginQuery(GL_TIME_ELAPSED, query);
		this->updatePositions(shaderManager, terrain, delta); // gpu
		glEndQuery(GL_TIME_ELAPSED);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		perfLog->timeUpdatePositions += elapsed_time / 1000000000.0;
	}
	else
	{
		this->computePredictedPositions(shaderManager, delta); // gpu
		// this->putParticlesInGrid(shaderManager); // gpu
		this->putParticlesInGridInParallel(shaderManager); // gpu
		this->computeDensity(shaderManager); // gpu
		this->computeMapDensity(shaderManager); // gpu
		this->calculatesAndApplyPressure(shaderManager, delta); // gpu
		this->updatePositions(shaderManager, terrain, delta); // gpu
	}
}


void	WaterSimulation::draw(
			Camera *camera,
			ShaderManager *shaderManager,
			Terrain *terrain,
			glm::vec3 *waterColor,
			float waterDensity)
{
	WaterShader *shader;
	int			shaderId;
	int			sizes[4], terrainFlatGridSize, terrainOffsetsSize;
	GLuint		terrainBufferTextureDataGrid, terrainTextureDataGrid,
				terrainBufferTextureFlatGrid, terrainTextureFlatGrid,
				terrainBufferTextureOffsets, terrainTextureOffsets;

	if (this->nbParticules == 0)
		return ;

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
	giveFloatToShader(shaderId, "waterDensity", waterDensity);

	giveIntToShader(shaderId, "mapDensityCellSize", MAP_DENSITY_CELL_SIZE);
	giveIntToShader(shaderId, "mapDensityW", this->mapDensityW);
	giveIntToShader(shaderId, "mapDensityH", this->mapDensityH);
	giveIntToShader(shaderId, "mapDensityD", this->mapDensityD);
	giveIntToShader(shaderId, "mapDensityIdHsize", this->mapDensityIdHsize);
	giveFloatTextureToShader(shaderId, "mapDensitiesBuffer", 0,
								this->textureBufferMapDensities,
								this->textureMapDensities);

	giveIntToShader(shaderId, "terrainCellSize", TERRAIN_CELL_SIZE);
	giveIntToShader(shaderId, "terrainGridW", sizes[0]);
	giveIntToShader(shaderId, "terrainGridH", sizes[1]);
	giveIntToShader(shaderId, "terrainGridD", sizes[2]);
	giveIntToShader(shaderId, "terrainIdHsize", sizes[3]);
	giveIntToShader(shaderId, "terrainGridSize", terrainFlatGridSize);
	giveIntToShader(shaderId, "terrainOffsetsSize", terrainOffsetsSize);
	giveIntToShader(shaderId, "terrainNbRectangles", terrain->getDataNbRectangles());
	giveVec3TextureToShader(shaderId, "terrainDataBuffer", 1,
								terrainBufferTextureDataGrid,
								terrainTextureDataGrid);
	giveFloatTextureToShader(shaderId, "terrainGridBuffer", 2,
								terrainBufferTextureFlatGrid,
								terrainTextureFlatGrid);
	giveFloatTextureToShader(shaderId, "terrainOffsetsBuffer", 3,
								terrainBufferTextureOffsets,
								terrainTextureOffsets);

	// Draw triangles
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
	this->generateMapDensity();
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
