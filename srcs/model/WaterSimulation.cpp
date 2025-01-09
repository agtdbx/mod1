#include <model/WaterSimulation.hpp>

#include <engine/render/shader/WaterShader.hpp>

//**** STATIC FUNCTIONS ********************************************************

static float	vec4Length(glm::vec4 vec)
{
	return (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}


static float	smoothingKernel(float dst)
{
	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	return (pow(SMOOTHING_RADIUS - dst, 2) * SMOOTHING_SCALE);
}


static float	smoothingKernelDerivate(float dst)
{
	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	return ((dst - SMOOTHING_RADIUS) * SMOOTHING_DERIVATE_SCALE);
}


static float	viscositySmoothingKernel(float dst)
{
	float	value;

	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	value = WATER_RADIUS2 - (dst * dst);
	return ((value * value * value) * SMOOTHING_VISCOSITY_SCALE);
}


static float	convertDensityToPressure(float density)
{
	return ((density - TARGET_DENSITY) * PRESSURE_MULTIPLIER);
}


static float	calculateSharedPressure(float densityA, float densityB)
{
	float	pressureA, pressureB;

	pressureA = convertDensityToPressure(densityA);
	pressureB = convertDensityToPressure(densityB);

	return ((pressureA + pressureB) / 2.0f);
}

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;

	int smoothing_radius = SMOOTHING_RADIUS;

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
	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;

	for (int i = 0; i < this->gridSize; i++)
	{
		std::vector<int>	gridContent;
		this->grid.push_back(gridContent);
	}

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
	this->positions.push_back(glm::vec4(position, 0.0f));
	this->predictedPositions.push_back(glm::vec4(position, 0.0f));
	this->velocities.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	this->densities.push_back(0.0);
	this->nbParticules++;
	this->numGroups = (this->nbParticules + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
	this->needToUpdateBuffers = true;
}


void	WaterSimulation::tick(ShaderManager *shaderManager, float delta)
{
	if (this->needToUpdateBuffers == true)
	{
		this->positionsToBuffer();
		this->predictedPositionsToBuffer();
		this->velocitiesToBuffer();
		this->densitiesToBuffer();
		this->gridFlatToBuffer();
		this->gridOffsetsToBuffer();

		this->needToUpdateBuffers = false;
	}

	this->applyGravityAndEnergyLose(shaderManager, delta); // gpu

	this->computePredictedPositions(shaderManager, delta); // gpu

	this->putParticlesInGrid(shaderManager); // cpu

	this->computeDensity(shaderManager); // gpu

	this->calculatesAndApplyPressure(shaderManager, delta); // cpu -> gpu

	this->updatePositions(shaderManager, delta); // gpu
}


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager)
{
	WaterShader *shader;
	int			shaderId;

	if (this->nbParticules == 0)
		return ;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 13,
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

	int smoothingRadiusLoc = glGetUniformLocation(shaderId, "smoothingRadius");
	glUniform1f(smoothingRadiusLoc, SMOOTHING_RADIUS);

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

	int gridWLoc = glGetUniformLocation(shaderId, "gridW");
	glUniform1i(gridWLoc, this->gridW);

	int gridHLoc = glGetUniformLocation(shaderId, "gridH");
	glUniform1i(gridHLoc, this->gridH);

	int gridDLoc = glGetUniformLocation(shaderId, "gridD");
	glUniform1i(gridDLoc, this->gridD);

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
	int	cellSize, currentOffset;

	this->gridFlat.clear();
	this->gridOffsets.clear();

	this->gridFlatSize = 0;
	this->gridOffsetsSize = 0;

	currentOffset = 0;
	for (std::vector<int> &cell : this->grid)
	{
		cellSize = cell.size();
		for (int i = 0; i < cellSize; i++)
			this->gridFlat.push_back(cell[i]);
		this->gridOffsets.push_back(currentOffset);
		currentOffset += cellSize;
		this->gridFlatSize += cellSize;
		this->gridOffsetsSize++;
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


void	WaterSimulation::gridFlatFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridFlat);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(float) * this->nbParticules,
						this->gridFlat.data());
}


void	WaterSimulation::gridOffsetsToBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridOffsetsSize,
					this->gridOffsets.data(), GL_DYNAMIC_DRAW);
}


void	WaterSimulation::gridOffsetsFromBuffer(void)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridOffsets);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(float) * this->nbParticules,
						this->gridOffsets.data());
}


void	WaterSimulation::applyGravityAndEnergyLose(ShaderManager *shaderManager, float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("velocityEffect");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	this->velocitiesToBuffer();

	computeShader->use();

	// Compute shader inputs setup
	int deltaLoc = glGetUniformLocation(shaderId, "delta");
	glUniform1f(deltaLoc, delta);

	int energyLoseLoc = glGetUniformLocation(shaderId, "energyLose");
	glUniform1f(energyLoseLoc, ENERGY_LOSE);

	int gravityForceLoc = glGetUniformLocation(shaderId, "gravityForce");
	glUniform1f(gravityForceLoc, GRAVITY_FORCE);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureVelocities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferVelocities);
	glBindImageTexture(0, this->textureVelocities, 0, GL_FALSE, 0,
							GL_READ_WRITE, GL_RGBA32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Buffer to vector
	this->velocitiesFromBuffer();

	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	// Apply energy lose
	// 	this->velocities[i] *= ENERGY_LOSE;
	// 	// Apply gravity
	// 	this->velocities[i] += glm::vec4(0, -GRAVITY_FORCE, 0, 0) * delta;
	// }
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

	// Vectors to buffers
	this->positionsToBuffer();
	this->velocitiesToBuffer();

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

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPositions);
	glUniform1i(glGetUniformLocation(shaderId, "positionsBuffer"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureVelocities);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferVelocities);
	glUniform1i(glGetUniformLocation(shaderId, "velocitiesBuffer"), 2);

	// Compute shader output setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePredictedPositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, this->textureBufferPredictedPositions);
	glBindImageTexture(0, this->texturePredictedPositions, 0, GL_FALSE, 0,
							GL_WRITE_ONLY, GL_RGBA32F);

	// Run compute shader
	glDispatchCompute((unsigned int)this->numGroups, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Buffer to vector
	this->predictedPositionsFromBuffer();

	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	// Compute predicted position
	// 	this->predictedPositions[i] = this->positions[i] + this->velocities[i] * delta;

	// 	// Check if particule is out of the map on x axis
	// 	if (this->predictedPositions[i].x < WATER_RADIUS)
	// 		this->predictedPositions[i].x = WATER_RADIUS;
	// 	else if (this->predictedPositions[i].x >= WATER_MAX_XZ)
	// 		this->predictedPositions[i].x = WATER_MAX_XZ;

	// 	// Check if particule is out of the map on y axis
	// 	if (this->predictedPositions[i].y < WATER_RADIUS)
	// 		this->predictedPositions[i].y = WATER_RADIUS;
	// 	else if (this->predictedPositions[i].y >= WATER_MAX_HEIGHT)
	// 		this->predictedPositions[i].y = WATER_MAX_HEIGHT;

	// 	// Check if particule is out of the map on z axis
	// 	if (this->predictedPositions[i].z < WATER_RADIUS)
	// 		this->predictedPositions[i].z = WATER_RADIUS;
	// 	else if (this->predictedPositions[i].z >= WATER_MAX_XZ)
	// 		this->predictedPositions[i].z = WATER_MAX_XZ;
	// }
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

	this->generateFlatGrid();
	this->gridFlatToBuffer();
	this->gridOffsetsToBuffer();
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

	this->densitiesToBuffer();
	this->positionsFromBuffer();

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

	// Buffer to vector
	this->densitiesFromBuffer();

	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	this->densities[i] = this->calculateDensity(this->predictedPositions[i]);
	// }
}


void	WaterSimulation::calculatesAndApplyPressure(ShaderManager *shaderManager, float delta)
{
	glm::vec4	pressureForce, pressureAcceleration, viscosityForce;

	for (int i = 0; i < this->nbParticules; i++)
	{
		if (this->densities[i] == 0.0f)
		{
			this->velocities[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			continue;
		}

		pressureForce = this->calculatePressureForce(i);
		pressureAcceleration = pressureForce / this->densities[i];
		this->velocities[i] += pressureAcceleration * delta;

		viscosityForce = calculateViscosityForce(i);
		this->velocities[i] += viscosityForce * delta;
	}
}


void	WaterSimulation::updatePositions(ShaderManager *shaderManager, float delta)
{
	ComputeShader	*computeShader;
	unsigned int	shaderId;

	// Get the compute shader
	computeShader = shaderManager->getComputeShader("updatePositions");
	if (!computeShader)
		return ;
	shaderId = computeShader->getShaderId();

	// Vectors to buffers
	this->positionsToBuffer();
	this->velocitiesToBuffer();

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

	int collisionEnergyKeepLoc = glGetUniformLocation(shaderId, "collisionEnergyKeep");
	glUniform1f(collisionEnergyKeepLoc, COLLISION_ENERGY_KEEP);

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

	// Buffer to vector
	this->positionsFromBuffer();
	this->velocitiesFromBuffer();

	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	// Update particule position
	// 	this->positions[i] += this->velocities[i] * delta;

	// 	// Check if particule is out of the map on x axis
	// 	if (this->positions[i].x < WATER_RADIUS)
	// 	{
	// 		this->positions[i].x = WATER_RADIUS;
	// 		this->velocities[i].x *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}
	// 	else if (this->positions[i].x >= WATER_MAX_XZ)
	// 	{
	// 		this->positions[i].x = WATER_MAX_XZ;
	// 		this->velocities[i].x *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}

	// 	// Check if particule is out of the map on y axis
	// 	if (this->positions[i].y < WATER_RADIUS)
	// 	{
	// 		this->positions[i].y = WATER_RADIUS;
	// 		this->velocities[i].y *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}
	// 	else if (this->positions[i].y >= WATER_MAX_HEIGHT)
	// 	{
	// 		this->positions[i].y = WATER_MAX_HEIGHT;
	// 		this->velocities[i].y *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}

	// 	// Check if particule is out of the map on z axis
	// 	if (this->positions[i].z < WATER_RADIUS)
	// 	{
	// 		this->positions[i].z = WATER_RADIUS;
	// 		this->velocities[i].z *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}
	// 	else if (this->positions[i].z >= WATER_MAX_XZ)
	// 	{
	// 		this->positions[i].z = WATER_MAX_XZ;
	// 		this->velocities[i].z *= -1.0f;
	// 		this->velocities[i] *= COLLISION_ENERGY_KEEP;
	// 	}
	// }

	this->positionsToBuffer();
}


float	WaterSimulation::calculateDensity(glm::vec4 position)
{
	float	density, dst, influence;
	int		px, py, pz, gx, gy, gz, gid;

	density = 0.0f;
	px = position.x / SMOOTHING_RADIUS;
	py = position.y / SMOOTHING_RADIUS;
	pz = position.z / SMOOTHING_RADIUS;

	for (int cx = -1; cx <= 1; cx++)
	{
		gx = px + cx;
		if (gx < 0 || gx >= this->gridW)
			continue;

		for (int cy = -1; cy <= 1; cy++)
		{
			gy = py + cy;
			if (gy < 0 || gy >= this->gridH)
				continue;

			for (int cz = -1; cz <= 1; cz++)
			{
				gz = pz + cz;
				if (gz < 0 || gz >= this->gridD)
					continue;

				gid = gx + gz * this->gridW + gy * this->idHsize;

				for (const int &i : this->grid[gid])
				{
					dst = vec4Length(this->positions[i] - position);
					influence = smoothingKernel(dst);
					density += WATER_MASS * influence;
				}
			}
		}
	}

	return (density);
}


glm::vec4	WaterSimulation::calculatePressureForce(int particuleIndex)
{
	glm::vec4	pressureForce, position, dir;
	float		density, dst, slope, sharedPressure;
	int			px, py, pz, gx, gy, gz, gid;

	pressureForce = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	position = this->predictedPositions[particuleIndex];
	density = this->densities[particuleIndex];
	px = position.x / SMOOTHING_RADIUS;
	py = position.y / SMOOTHING_RADIUS;
	pz = position.z / SMOOTHING_RADIUS;

	for (int cx = -1; cx <= 1; cx++)
	{
		gx = px + cx;
		if (gx < 0 || gx >= this->gridW)
			continue;

		for (int cy = -1; cy <= 1; cy++)
		{
			gy = py + cy;
			if (gy < 0 || gy >= this->gridH)
				continue;

			for (int cz = -1; cz <= 1; cz++)
			{
				gz = pz + cz;
				if (gz < 0 || gz >= this->gridD)
					continue;

				gid = gx + gz * this->gridW + gy * this->idHsize;

				for (const int &i : this->grid[gid])
				{
					if (i == particuleIndex)
						continue;

					dir = this->predictedPositions[i] - position;
					dst = vec4Length(dir);
					if (dst == 0.0f)
						dir = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
					else
						dir /= dst;
					slope = smoothingKernelDerivate(dst);
					sharedPressure = calculateSharedPressure(density, this->densities[i]);
					pressureForce += sharedPressure * dir * slope * WATER_MASS / density;
				}
			}
		}
	}

	return (pressureForce);
}


glm::vec4	WaterSimulation::calculateViscosityForce(int particuleIndex)
{
	glm::vec4	viscosityForce, position, velocity, velocityDir;
	float		velocityLength, dst, influence;
	int			px, py, pz, gx, gy, gz, gid;

	viscosityForce = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	position = this->predictedPositions[particuleIndex];
	velocity = this->velocities[particuleIndex];
	px = position.x / SMOOTHING_RADIUS;
	py = position.y / SMOOTHING_RADIUS;
	pz = position.z / SMOOTHING_RADIUS;


	for (int cx = -1; cx <= 1; cx++)
	{
		gx = px + cx;
		if (gx < 0 || gx >= this->gridW)
			continue;

		for (int cy = -1; cy <= 1; cy++)
		{
			gy = py + cy;
			if (gy < 0 || gy >= this->gridH)
				continue;

			for (int cz = -1; cz <= 1; cz++)
			{
				gz = pz + cz;
				if (gz < 0 || gz >= this->gridD)
					continue;

				gid = gx + gz * this->gridW + gy * this->idHsize;

				for (const int &i : this->grid[gid])
				{
					if (i == particuleIndex)
						continue;

					velocityDir = this->velocities[i] - velocity;
					velocityLength = vec4Length(velocityDir);
					if (velocityLength == 0.0f)
						continue;
					velocityDir /= velocityLength;
					dst = vec4Length(position - this->predictedPositions[i]);
					influence = viscositySmoothingKernel(dst);
					viscosityForce += velocityDir * influence;
				}
			}
		}
	}

	return (viscosityForce * VISCOSITY_FORCE);
}
