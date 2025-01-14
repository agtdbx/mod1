#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>

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


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager, Terrain *terrain)
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

	int lightPosLoc = glGetUniformLocation(shaderId, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(camera->getLightPosition()));

	int cameraPosLoc = glGetUniformLocation(shaderId, "cameraPos");
	glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera->getPosition()));

	int cameraFrontLoc = glGetUniformLocation(shaderId, "cameraFront");
	glUniform3fv(cameraFrontLoc, 1, glm::value_ptr(camera->getFront()));

	int cameraRightLoc = glGetUniformLocation(shaderId, "cameraRight");
	glUniform3fv(cameraRightLoc, 1, glm::value_ptr(camera->getRight()));

	int cameraUpLoc = glGetUniformLocation(shaderId, "cameraUp");
	glUniform3fv(cameraUpLoc, 1, glm::value_ptr(camera->getUp()));

	int cameraFOVLoc = glGetUniformLocation(shaderId, "cameraFOV");
	glUniform1f(cameraFOVLoc, CAMERA_FOV);

	int cameraFarLoc = glGetUniformLocation(shaderId, "cameraFar");
	glUniform1f(cameraFarLoc, CAMERA_MAX_VIEW_DIST);

	int planeWidthLoc = glGetUniformLocation(shaderId, "planeWidth");
	glUniform1f(planeWidthLoc, camera->getPlaneWidth());

	int planeHeightLoc = glGetUniformLocation(shaderId, "planeHeight");
	glUniform1f(planeHeightLoc, camera->getPlaneHeight());

	int waterRadius2Loc = glGetUniformLocation(shaderId, "waterRadius2");
	glUniform1f(waterRadius2Loc, WATER_RADIUS2);

	int waterColorLoc = glGetUniformLocation(shaderId, "waterColor");
	glUniform3fv(waterColorLoc, 1, glm::value_ptr(WATER_COLOR));

	int renderCellSizeLoc = glGetUniformLocation(shaderId, "renderCellSize");
	glUniform1f(renderCellSizeLoc, RENDER_CELL_SIZE);

	int nbPositionsLoc = glGetUniformLocation(shaderId, "nbPositions");
	glUniform1i(nbPositionsLoc, this->nbParticules);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPositions);
	glUniform1i(glGetUniformLocation(shaderId, "positionsBuffer"), 0);

	int mapSizeLoc = glGetUniformLocation(shaderId, "mapSize");
	glUniform1i(mapSizeLoc, MAP_SIZE);

	int mapHeightLoc = glGetUniformLocation(shaderId, "mapHeight");
	glUniform1i(mapHeightLoc, WATER_MAX_HEIGHT);

	int renderGridWLoc = glGetUniformLocation(shaderId, "renderGridW");
	glUniform1i(renderGridWLoc, this->renderGridW);

	int renderGridHLoc = glGetUniformLocation(shaderId, "renderGridH");
	glUniform1i(renderGridHLoc, this->renderGridH);

	int renderGridDLoc = glGetUniformLocation(shaderId, "renderGridD");
	glUniform1i(renderGridDLoc, this->renderGridD);

	int renderGridSizeLoc = glGetUniformLocation(shaderId, "renderGridSize");
	glUniform1i(renderGridSizeLoc, this->renderGridFlatSize);

	int renderOffsetsSizeLoc = glGetUniformLocation(shaderId, "renderOffsetsSize");
	glUniform1i(renderOffsetsSizeLoc, this->renderGridOffsetsSize);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureRenderGridFlat);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferRenderGridFlat);
	glUniform1i(glGetUniformLocation(shaderId, "renderGridBuffer"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureRenderGridOffsets);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferRenderGridOffsets);
	glUniform1i(glGetUniformLocation(shaderId, "renderOffsetsBuffer"), 2);

	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}

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

	glGenBuffers(1, &this->textureBufferGridFlat);
	glGenTextures(1, &this->textureGridFlat);

	glGenBuffers(1, &this->textureBufferGridOffsets);
	glGenTextures(1, &this->textureGridOffsets);

	glGenBuffers(1, &this->textureBufferRenderGridFlat);
	glGenTextures(1, &this->textureRenderGridFlat);

	glGenBuffers(1, &this->textureBufferRenderGridOffsets);
	glGenTextures(1, &this->textureRenderGridOffsets);
}


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


void	WaterSimulation::generateFlatGrid(void)
{
	int	cellSize;

	// grid
	this->gridFlat.clear();
	this->gridOffsets.clear();

	this->gridFlatSize = 0;
	this->gridOffsetsSize = 0;

	for (std::vector<int> &cell : this->grid)
	{
		cellSize = cell.size();
		for (int i = 0; i < cellSize; i++)
			this->gridFlat.push_back(cell[i]);
		this->gridOffsets.push_back(this->gridFlatSize);
		this->gridFlatSize += cellSize;
		this->gridOffsetsSize++;
	}

	// render grid
	this->renderGridFlat.clear();
	this->renderGridOffsets.clear();

	this->renderGridFlatSize = 0;
	this->renderGridOffsetsSize = 0;

	for (std::vector<int> &cell : this->renderGrid)
	{
		cellSize = cell.size();
		for (int i = 0; i < cellSize; i++)
			this->renderGridFlat.push_back(cell[i]);
		this->renderGridOffsets.push_back(this->renderGridFlatSize);
		this->renderGridFlatSize += cellSize;
		this->renderGridOffsetsSize++;
	}
}


void	WaterSimulation::positionsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPositions);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec4) * this->nbParticules,
					this->positions.data(), GL_DYNAMIC_DRAW);
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
					this->predictedPositions.data(), GL_DYNAMIC_DRAW);
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
					this->velocities.data(), GL_DYNAMIC_DRAW);
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
					this->densities.data(), GL_DYNAMIC_DRAW);
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
					this->gridFlat.data(), GL_DYNAMIC_DRAW);
}


void	WaterSimulation::gridOffsetsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridOffsetsSize,
					this->gridOffsets.data(), GL_DYNAMIC_DRAW);
}


void	WaterSimulation::renderGridFlatToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferRenderGridFlat);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->renderGridFlatSize,
					this->renderGridFlat.data(), GL_DYNAMIC_DRAW);
}


void	WaterSimulation::renderGridOffsetsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferRenderGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->renderGridOffsetsSize,
					this->renderGridOffsets.data(), GL_DYNAMIC_DRAW);
}


void	WaterSimulation::computePredictedPositions(ShaderManager *shaderManager, float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("predictedPositions");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	int deltaLoc = glGetUniformLocation(shaderId, "delta");
	glUniform1f(deltaLoc, delta);

	int waterRadiusLoc = glGetUniformLocation(shaderId, "waterRadius");
	glUniform1f(waterRadiusLoc, WATER_RADIUS);

	int waterMaxXZLoc = glGetUniformLocation(shaderId, "waterMaxXZ");
	glUniform1f(waterMaxXZLoc, WATER_MAX_XZ);

	int waterMaxYLoc = glGetUniformLocation(shaderId, "waterMaxY");
	glUniform1f(waterMaxYLoc, WATER_MAX_HEIGHT);

	int gravityForceLoc = glGetUniformLocation(shaderId, "gravityForce");
	glUniform1f(gravityForceLoc, GRAVITY_FORCE);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPositions);
	glUniform1i(glGetUniformLocation(shaderId, "positionsBuffer"), 2);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePredictedPositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPredictedPositions);
	glBindImageTexture(0, this->texturePredictedPositions, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureVelocities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferVelocities);
	glBindImageTexture(1, this->textureVelocities, 0, GL_FALSE, 0,
							GL_READ_WRITE, GL_RGBA32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Buffer to vector
	this->predictedPositionsFromBuffer();
}


void	WaterSimulation::putParticlesInGrid(ShaderManager *shaderManager)
{
	int	gx, gy, gz, gid;

	// Clear grid
	for (int i = 0; i < this->gridSize; i++)
		this->grid[i].clear();

	// Put particles into grid
	for (int i = 0; i < this->nbParticules; i++)
	{
		gx = this->predictedPositions[i].x / SMOOTHING_RADIUS;
		gy = this->predictedPositions[i].y / SMOOTHING_RADIUS;
		gz = this->predictedPositions[i].z / SMOOTHING_RADIUS;

		gid = gx + gz * this->gridW + gy * this->idHsize;
		this->grid[gid].push_back(i);
	}

	// Clear render grid
	for (int i = 0; i < this->renderGridSize; i++)
		this->renderGrid[i].clear();

	// Put particles into render grid
	for (int i = 0; i < this->nbParticules; i++)
	{
		gx = this->predictedPositions[i].x / RENDER_CELL_SIZE;
		gy = this->predictedPositions[i].y / RENDER_CELL_SIZE;
		gz = this->predictedPositions[i].z / RENDER_CELL_SIZE;

		gid = gx + gz * this->renderGridW + gy * this->renderIdHsize;
		this->renderGrid[gid].push_back(i);
	}

	this->generateFlatGrid();
	this->gridFlatToBuffer();
	this->gridOffsetsToBuffer();
	this->renderGridFlatToBuffer();
	this->renderGridOffsetsToBuffer();
}


void	WaterSimulation::computeDensity(ShaderManager *shaderManager)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("densities");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	int smoothingRadiusLoc = glGetUniformLocation(shaderId, "smoothingRadius");
	glUniform1f(smoothingRadiusLoc, SMOOTHING_RADIUS);

	int smoothingScaleLoc = glGetUniformLocation(shaderId, "smoothingScale");
	glUniform1f(smoothingScaleLoc, SMOOTHING_SCALE);

	int waterMassLoc = glGetUniformLocation(shaderId, "waterMass");
	glUniform1f(waterMassLoc, WATER_MASS);

	int gridWLoc = glGetUniformLocation(shaderId, "gridW");
	glUniform1i(gridWLoc, this->gridW);

	int gridHLoc = glGetUniformLocation(shaderId, "gridH");
	glUniform1i(gridHLoc, this->gridH);

	int gridDLoc = glGetUniformLocation(shaderId, "gridD");
	glUniform1i(gridDLoc, this->gridD);

	int idHsizeLoc = glGetUniformLocation(shaderId, "idHsize");
	glUniform1i(idHsizeLoc, this->idHsize);

	int gridSizeLoc = glGetUniformLocation(shaderId, "gridSize");
	glUniform1i(gridSizeLoc, this->gridFlatSize);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureGridFlat);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferGridFlat);
	glUniform1i(glGetUniformLocation(shaderId, "gridBuffer"), 1);

	int offsetsSizeLoc = glGetUniformLocation(shaderId, "offsetsSize");
	glUniform1i(offsetsSizeLoc, this->gridOffsetsSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureGridOffsets);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferGridOffsets);
	glUniform1i(glGetUniformLocation(shaderId, "offsetsBuffer"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPositions);
	glUniform1i(glGetUniformLocation(shaderId, "positionsBuffer"), 3);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureDensities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferDensities);
	glBindImageTexture(0, this->textureDensities, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_R32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::calculatesAndApplyPressure(ShaderManager *shaderManager, float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("pressure");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Compute shader inputs setup
	int deltaLoc = glGetUniformLocation(shaderId, "delta");
	glUniform1f(deltaLoc, delta);

	int smoothingRadiusLoc = glGetUniformLocation(shaderId, "smoothingRadius");
	glUniform1f(smoothingRadiusLoc, SMOOTHING_RADIUS);

	int smoothingDerivateScaleLoc = glGetUniformLocation(shaderId, "smoothingDerivateScale");
	glUniform1f(smoothingDerivateScaleLoc, SMOOTHING_DERIVATE_SCALE);

	int smoothingViscosityScaleLoc = glGetUniformLocation(shaderId, "smoothingViscosityScale");
	glUniform1f(smoothingViscosityScaleLoc, SMOOTHING_VISCOSITY_SCALE);

	int waterMassLoc = glGetUniformLocation(shaderId, "waterMass");
	glUniform1f(waterMassLoc, WATER_MASS);

	int waterRadius2Loc = glGetUniformLocation(shaderId, "waterRadius2");
	glUniform1f(waterRadius2Loc, WATER_RADIUS2);

	int targetDensityLoc = glGetUniformLocation(shaderId, "targetDensity");
	glUniform1f(targetDensityLoc, TARGET_DENSITY);

	int pressureMultiplierLoc = glGetUniformLocation(shaderId, "pressureMultiplier");
	glUniform1f(pressureMultiplierLoc, PRESSURE_MULTIPLIER);

	int viscosityStrengthLoc = glGetUniformLocation(shaderId, "viscosityStrength");
	glUniform1f(viscosityStrengthLoc, VISCOSITY_FORCE);

	int gridWLoc = glGetUniformLocation(shaderId, "gridW");
	glUniform1i(gridWLoc, this->gridW);

	int gridHLoc = glGetUniformLocation(shaderId, "gridH");
	glUniform1i(gridHLoc, this->gridH);

	int gridDLoc = glGetUniformLocation(shaderId, "gridD");
	glUniform1i(gridDLoc, this->gridD);

	int idHsizeLoc = glGetUniformLocation(shaderId, "idHsize");
	glUniform1i(idHsizeLoc, this->idHsize);

	int gridSizeLoc = glGetUniformLocation(shaderId, "gridSize");
	glUniform1i(gridSizeLoc, this->gridFlatSize);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureGridFlat);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferGridFlat);
	glUniform1i(glGetUniformLocation(shaderId, "gridBuffer"), 1);

	int offsetsSizeLoc = glGetUniformLocation(shaderId, "offsetsSize");
	glUniform1i(offsetsSizeLoc, this->gridOffsetsSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureGridOffsets);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferGridOffsets);
	glUniform1i(glGetUniformLocation(shaderId, "offsetsBuffer"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePredictedPositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPredictedPositions);
	glUniform1i(glGetUniformLocation(shaderId, "predictedPositionsBuffer"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureDensities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferDensities);
	glUniform1i(glGetUniformLocation(shaderId, "densitiesBuffer"), 4);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureVelocities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferVelocities);
	glBindImageTexture(0, this->textureVelocities, 0, GL_FALSE, 0,
							GL_READ_WRITE, GL_RGBA32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void	WaterSimulation::updatePositions(
							ShaderManager *shaderManager,
							Terrain *terrain,
							float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;
	int				sizes[4], terrainFlatGridSize, terrainOffsetsGridSize;
	GLuint			terrainBufferTextureDataGrid, terrainTextureDataGrid,
					terrainBufferTextureFlatGrid, terrainTextureFlatGrid,
					terrainBufferTextureOffsetsGrid, terrainTextureOffsetsGrid;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("updatePositions");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	computeShader->use();

	// Get terrain data
	terrainBufferTextureDataGrid = terrain->getTextureBufferTerrainGridData();
	terrainTextureDataGrid = terrain->getTextureTerrainGridData();
	terrainBufferTextureFlatGrid = terrain->getTextureBufferTerrainGridFlat();
	terrainTextureFlatGrid = terrain->getTextureTerrainGridFlat();
	terrainFlatGridSize = terrain->getSizeTerrainGridFlat();
	terrainBufferTextureOffsetsGrid = terrain->getTextureBufferTerrainGridOffsets();
	terrainTextureOffsetsGrid = terrain->getTextureTerrainGridOffsets();
	terrainOffsetsGridSize = terrain->getSizeTerrainGridOffsets();
	terrain->getGridSize(sizes);

	// Compute shader inputs setup
	int deltaLoc = glGetUniformLocation(shaderId, "delta");
	glUniform1f(deltaLoc, delta);

	int waterRadiusLoc = glGetUniformLocation(shaderId, "waterRadius");
	glUniform1f(waterRadiusLoc, WATER_RADIUS);

	int waterMaxXZLoc = glGetUniformLocation(shaderId, "waterMaxXZ");
	glUniform1f(waterMaxXZLoc, WATER_MAX_XZ);

	int waterMaxYLoc = glGetUniformLocation(shaderId, "waterMaxY");
	glUniform1f(waterMaxYLoc, WATER_MAX_HEIGHT);

	int collisionEnergyKeepLoc = glGetUniformLocation(shaderId, "collisionEnergyKeep");
	glUniform1f(collisionEnergyKeepLoc, COLLISION_ENERGY_KEEP);

	int terrainCellSizeLoc = glGetUniformLocation(shaderId, "terrainCellSize");
	glUniform1f(terrainCellSizeLoc, TERRAIN_CELL_SIZE);

	int terrainGridWLoc = glGetUniformLocation(shaderId, "terrainGridW");
	glUniform1i(terrainGridWLoc, sizes[0]);

	int terrainGridHLoc = glGetUniformLocation(shaderId, "terrainGridH");
	glUniform1i(terrainGridHLoc, sizes[1]);

	int terrainGridDLoc = glGetUniformLocation(shaderId, "terrainGridD");
	glUniform1i(terrainGridDLoc, sizes[2]);

	int terrainIdHsizeLoc = glGetUniformLocation(shaderId, "terrainIdHsize");
	glUniform1i(terrainIdHsizeLoc, sizes[3]);

	int terrainGridSizeLoc = glGetUniformLocation(shaderId, "terrainGridSize");
	glUniform1i(terrainGridSizeLoc, terrainFlatGridSize);

	int terrainOffsetsSizeLoc = glGetUniformLocation(shaderId, "terrainOffsetsSize");
	glUniform1i(terrainOffsetsSizeLoc, terrainOffsetsGridSize);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, terrainTextureDataGrid);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, terrainBufferTextureDataGrid);
	glUniform1i(glGetUniformLocation(shaderId, "terrainDataBuffer"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_BUFFER, terrainTextureFlatGrid);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, terrainBufferTextureFlatGrid);
	glUniform1i(glGetUniformLocation(shaderId, "terrainGridBuffer"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_BUFFER, terrainTextureOffsetsGrid);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, terrainBufferTextureOffsetsGrid);
	glUniform1i(glGetUniformLocation(shaderId, "terrainOffsetsBuffer"), 4);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPositions);
	glBindImageTexture(0, this->texturePositions, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureVelocities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferVelocities);
	glBindImageTexture(1, this->textureVelocities, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_RGBA32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
