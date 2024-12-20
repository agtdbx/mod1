#include <model/WaterSimulation.hpp>

#include <engine/render/WaterShader.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;
	this->waterPositions = NULL;
	this->needCreateWaterPositions = true;
	this->model = glm::mat4(1.0f);
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	// this->mesh = obj.mesh;
	this->nbParticules = obj.nbParticules;
	this->positions = obj.positions;
	this->velocities = obj.velocities;
	this->waterPositions = NULL;
	this->needCreateWaterPositions = true;
	this->model = obj.model;
}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{
	if (this->waterPositions)
	{
		delete [] this->waterPositions;
		this->waterPositions = NULL;
	}
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
	this->velocities = obj.velocities;
	this->needCreateWaterPositions = true;
	this->model = obj.model;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(Vec3 position)
{
	this->positions.push_back(position);
	this->velocities.push_back(Vec3(0, 0, 0));
	this->nbParticules++;
	this->needCreateWaterPositions = true;
}


void	WaterSimulation::tick(double delta)
{
	if (this->needCreateWaterPositions)
	{
		this->needCreateWaterPositions = false;
		this->createWaterPositions();
	}

	for (int i = 0; i < this->nbParticules; i++)
	{
		// Apply gravity
		this->velocities[i] += Vec3(0, -0.1, 0) * delta;

		// Update particule position
		this->positions[i] += this->velocities[i] * delta;

		// Check if particule is out of the map on x axis
		if (this->positions[i].x < WATER_RADIUS)
		{
			this->positions[i].x = WATER_RADIUS;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].x >= MAP_SIZE)
		{
			this->positions[i].x = MAP_SIZE;
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
			this->positions[i].y = MAP_SIZE;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		// Check if particule is out of the map on z axis
		if (this->positions[i].z < WATER_RADIUS)
		{
			this->positions[i].z = WATER_RADIUS;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}
		else if (this->positions[i].z >= MAP_SIZE)
		{
			this->positions[i].z = MAP_SIZE;
			this->velocities[i] *= -COLLISION_ENERGY_KEEP;
		}

		// Update water position list
		this->waterPositions[i].x = this->positions[i].x;
		this->waterPositions[i].y = this->positions[i].y;
		this->waterPositions[i].z = this->positions[i].z;
	}
}


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager)
{
	WaterShader *shader;

	if (this->waterPositions == NULL)
		return ;

	float	triangleOverScreen[12] = {
		// Triangle 1
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,

		// Triangle 2
		-1.0f, -1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 13, triangleOverScreen, GL_STATIC_DRAW);

	shader = shaderManager->getWaterShader();
	shader->use();

	int lightPosLoc = glGetUniformLocation(shader->getShaderId(), "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(camera->getLightPosition()));

	int cameraPosLoc = glGetUniformLocation(shader->getShaderId(), "cameraPos");
	glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera->getPosition()));

	int cameraFrontLoc = glGetUniformLocation(shader->getShaderId(), "cameraFront");
	glUniform3fv(cameraFrontLoc, 1, glm::value_ptr(camera->getFront()));

	int cameraRightLoc = glGetUniformLocation(shader->getShaderId(), "cameraRight");
	glUniform3fv(cameraRightLoc, 1, glm::value_ptr(camera->getRight()));

	int cameraUpLoc = glGetUniformLocation(shader->getShaderId(), "cameraUp");
	glUniform3fv(cameraUpLoc, 1, glm::value_ptr(camera->getUp()));

	int cameraFOVLoc = glGetUniformLocation(shader->getShaderId(), "cameraFOV");
	glUniform1f(cameraFOVLoc, CAMERA_FOV);

	int cameraFarLoc = glGetUniformLocation(shader->getShaderId(), "cameraFar");
	glUniform1f(cameraFarLoc, CAMERA_MAX_VIEW_DIST);

	int planeWidthLoc = glGetUniformLocation(shader->getShaderId(), "planeWidth");
	glUniform1f(planeWidthLoc, camera->getPlaneWidth());

	int planeHeightLoc = glGetUniformLocation(shader->getShaderId(), "planeHeight");
	glUniform1f(planeHeightLoc, camera->getPlaneHeight());

	int waterRadius2Loc = glGetUniformLocation(shader->getShaderId(), "waterRadius2");
	glUniform1f(waterRadius2Loc, WATER_RADIUS2);

	int waterColorLoc = glGetUniformLocation(shader->getShaderId(), "waterColor");
	glUniform3fv(waterColorLoc, 1, glm::value_ptr(WATER_COLOR));

	int nbPositionsLoc = glGetUniformLocation(shader->getShaderId(), "nbPositions");
	glUniform1i(nbPositionsLoc, this->nbParticules);

	int	positionsLoc;
	std::string	id;
	for (int i = 0; i < this->nbParticules; i++)
	{
		id = "positions[" + std::to_string(i) + "]";
		positionsLoc = glGetUniformLocation(shader->getShaderId(), id.c_str());
		glUniform3fv(positionsLoc, 1, glm::value_ptr(this->waterPositions[i]));
	}

	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}

//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::createWaterPositions(void)
{
	if (this->waterPositions)
	{
		delete [] this->waterPositions;
		this->waterPositions = NULL;
	}

	this->waterPositions = new glm::vec3[this->nbParticules];

	if (this->waterPositions == NULL)
		return ;

	for (int i = 0; i < this->nbParticules; i++)
	{
		this->waterPositions[i].x = 0.0f;
		this->waterPositions[i].y = 0.0f;
		this->waterPositions[i].z = 0.0f;
	}
}
