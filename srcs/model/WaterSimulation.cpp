#include <model/WaterSimulation.hpp>

#include <engine/render/WaterShader.hpp>

//**** STATIC FUNCTIONS ********************************************************

static float	vec3Length(glm::vec3 vec)
{
	return (sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}


static float	smoothingKernel(float dst)
{
	float	tmp;

	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	tmp = SMOOTHING_RADIUS - dst;
	return ((tmp * tmp) / SMOOTHING_VOLUME);
}


static float	smoothingKernelDerivate(float dst)
{
	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	return ((dst - SMOOTHING_RADIUS) * SMOOTHING_SCALE);
}


static float	viscositySmoothingKernel(float dst)
{
	float	value;

	if (dst > SMOOTHING_RADIUS)
		return (0.0);

	// value = (dst * dst) / WATER_RADIUS2;
	value = WATER_RADIUS2 - (dst * dst);
	return ((value * value * value) / SMOOTHING_VOLUME);
	// return (value * value * value);
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
	this->gridSize = this->gridW * this->gridH * this->gridD;
	this->idHsize = this->gridW * this->gridH;
	this->gridFlatSize = 0;
	this->gridOffsetsSize = 0;

	printf("GRID SIZE %i %i %i (total %i)\n", this->gridW, this->gridH, this->gridD, this->gridSize);

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

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{
	glDeleteBuffers(1, &this->textureBufferPositions);
	glDeleteTextures(1, &this->texturePositions);

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

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(glm::vec3 position)
{
	this->positions.push_back(position);
	this->predictedPositions.push_back(position);
	this->velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	this->densities.push_back(0.0);
	this->nbParticules++;
}


void	WaterSimulation::tick(float delta)
{
	int			gx, gy, gz, gid;
	// glm::vec3	pressureForce, pressureAcceleration, viscosityForce;

	// Clear grid
	for (int i = 0; i < this->gridSize; i++)
		this->grid[i].clear();

	// Compute predicted position and put particles into grid
	for (int i = 0; i < this->nbParticules; i++)
	{
		// Apply gravity
		// this->velocities[i] += glm::vec3(0, -0.1, 0) * delta;
		// Compute predicted position
		this->predictedPositions[i] = this->positions[i] + this->velocities[i] * delta;
		// Check if particule is out of the map on x axis
		if (this->predictedPositions[i].x < WATER_RADIUS)
			this->predictedPositions[i].x = WATER_RADIUS;
		else if (this->predictedPositions[i].x >= WATER_MAX_XZ)
			this->predictedPositions[i].x = WATER_MAX_XZ;
		// Check if particule is out of the map on y axis
		if (this->predictedPositions[i].y < WATER_RADIUS)
			this->predictedPositions[i].y = WATER_RADIUS;
		else if (this->predictedPositions[i].y >= WATER_MAX_HEIGHT)
			this->predictedPositions[i].y = WATER_MAX_HEIGHT;
		// Check if particule is out of the map on z axis
		if (this->predictedPositions[i].z < WATER_RADIUS)
			this->predictedPositions[i].z = WATER_RADIUS;
		else if (this->predictedPositions[i].z >= WATER_MAX_XZ)
			this->predictedPositions[i].z = WATER_MAX_XZ;

		gx = this->predictedPositions[i].x / SMOOTHING_RADIUS;
		gy = this->predictedPositions[i].y / SMOOTHING_RADIUS;
		gz = this->predictedPositions[i].z / SMOOTHING_RADIUS;

		gid = gx + gz * this->gridW + gy * this->idHsize;
		this->grid[gid].push_back(i);
	}

	// Compute densities
	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	this->densities[i] = this->calculateDensity(this->predictedPositions[i]);
	// }

	float	density = this->calculateDensity(glm::vec3(25, 12.5, 25));
	printf("SMOOTHING_RADIUS : %2.1f, density : %.3f\n", SMOOTHING_RADIUS, density);
	// SMOOTHING_RADIUS : 5.0, density : 0.525
	// SMOOTHING_RADIUS : 10.0, density : 0.033
	// SMOOTHING_RADIUS : 20.0, density : 0.002

	// Calculate and apply pressure
	// for (int i = 0; i < this->nbParticules; i++)
	// {
	// 	if (this->densities[i] == 0.0f)
	// 	{
	// 		this->velocities[i] = glm::vec3(0.0f, 0.0f, 0.0f);
	// 		continue;
	// 	}

	// 	pressureForce = this->calculatePressureForce(i);
	// 	pressureAcceleration = pressureForce / this->densities[i];
	// 	this->velocities[i] += pressureAcceleration * delta;

	// 	viscosityForce = calculateViscosityForce(i);
	// 	this->velocities[i] += viscosityForce * delta;
	// }

	// Update positions with screen collision
	for (int i = 0; i < this->nbParticules; i++)
	{

		// Update particule position
		this->positions[i] += this->velocities[i] * delta;

		// Check if particule is out of the map on x axis
		if (this->positions[i].x < WATER_RADIUS)
		{
			this->positions[i].x = WATER_RADIUS;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].x >= WATER_MAX_XZ)
		{
			this->positions[i].x = WATER_MAX_XZ;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		// Check if particule is out of the map on y axis
		if (this->positions[i].y < WATER_RADIUS)
		{
			this->positions[i].y = WATER_RADIUS;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].y >= WATER_MAX_HEIGHT)
		{
			this->positions[i].y = WATER_MAX_HEIGHT;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		// Check if particule is out of the map on z axis
		if (this->positions[i].z < WATER_RADIUS)
		{
			this->positions[i].z = WATER_RADIUS;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].z >= WATER_MAX_XZ)
		{
			this->positions[i].z = WATER_MAX_XZ;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
	}
}


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager)
{
	WaterShader *shader;
	int			shaderId;

	if (this->nbParticules == 0)
		return ;

	this->generateFlatGrid();

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
	glUniform1i(smoothingRadiusLoc, SMOOTHING_RADIUS);

	int nbPositionsLoc = glGetUniformLocation(shaderId, "nbPositions");
	glUniform1i(nbPositionsLoc, this->nbParticules);

	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferPositions);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * this->nbParticules,
					this->positions.data(), GL_DYNAMIC_DRAW);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texturePositions);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, this->textureBufferPositions);
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

	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridFlat);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridFlatSize,
					this->gridFlat.data(), GL_DYNAMIC_DRAW);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, this->textureGridFlat);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, this->textureBufferGridFlat);
	glUniform1i(glGetUniformLocation(shaderId, "gridBuffer"), 1);

	int offsetsSizeLoc = glGetUniformLocation(shaderId, "offsetsSize");
	glUniform1i(offsetsSizeLoc, this->gridOffsetsSize);

	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBufferGridOffsets);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * this->gridOffsetsSize,
					this->gridOffsets.data(), GL_DYNAMIC_DRAW);
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
	glGenTextures(1, &texturePositions);

	glGenBuffers(1, &this->textureBufferGridFlat);
	glGenTextures(1, &textureGridFlat);

	glGenBuffers(1, &this->textureBufferGridOffsets);
	glGenTextures(1, &textureGridOffsets);
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


float	WaterSimulation::calculateDensity(glm::vec3 position)
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
					dst = vec3Length(this->positions[i] - position);
					influence = smoothingKernel(dst);
					density += WATER_MASS * influence;
				}
			}
		}
	}

	return (density);
}


glm::vec3	WaterSimulation::calculatePressureForce(int particuleIndex)
{
	glm::vec3	pressureForce, position, dir;
	float		density, dst, slope, sharedPressure;
	int			px, py, pz, gx, gy, gz, gid;

	pressureForce = glm::vec3(0.0f, 0.0f, 0.0f);

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
					dst = vec3Length(dir);
					if (dst == 0.0f)
						dir = glm::vec3(1.0f, 0.0f, 0.0f);
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


glm::vec3	WaterSimulation::calculateViscosityForce(int particuleIndex)
{
	glm::vec3	viscosityForce, position, velocity, velocityDir;
	float		velocityLength, dst, influence;
	int			px, py, pz, gx, gy, gz, gid;

	viscosityForce = glm::vec3(0.0f, 0.0f, 0.0f);

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
					velocityLength = vec3Length(velocityDir);
					if (velocityLength == 0.0f)
						continue;
					velocityDir /= velocityLength;
					dst = vec3Length(position - this->predictedPositions[i]);
					influence = viscositySmoothingKernel(dst);
					viscosityForce += velocityDir * influence;
				}
			}
		}
	}

	return (viscosityForce * VISCOSITY_FORCE);
}
