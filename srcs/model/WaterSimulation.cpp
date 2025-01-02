#include <model/WaterSimulation.hpp>

#include <engine/render/WaterShader.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;
	this->positions = NULL;
	this->velocities = NULL;
	this->densities = NULL;
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	this->nbParticules = obj.nbParticules;
	this->positions = NULL;
	this->velocities = NULL;
	this->densities = NULL;

	this->positions = new glm::vec3[this->nbParticules + 1];
	this->velocities = new glm::vec3[this->nbParticules + 1];
	this->densities = new double[this->nbParticules + 1];

	if (this->positions && this->velocities && this->densities)
	{
		for (int i = 0; i < this->nbParticules; i++)
		{
			this->positions[i] = obj.positions[i];
			this->velocities[i] = obj.velocities[i];
			this->densities[i] = obj.densities[i];
		}
	}
	else
	{
		this->freeArrays();
		this->nbParticules = 0;
	}

}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{
	this->freeArrays();
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

	glm::vec3	*newPositions = NULL;
	glm::vec3	*newVelocities = NULL;
	double		*newDensities = NULL;
	int			newNbParticles = obj.nbParticules;

	newPositions = new glm::vec3[this->nbParticules + 1];
	if (newPositions == NULL)
		return (*this);

	newVelocities = new glm::vec3[this->nbParticules + 1];
	if (newPositions == NULL)
	{
		delete [] newPositions;
		return (*this);
	}

	newDensities = new double[this->nbParticules + 1];
	if (newPositions == NULL)
	{
		delete [] newPositions;
		delete [] newVelocities;
		return (*this);
	}

	this->freeArrays();

	for (int i = 0; i < newNbParticles; i++)
	{
		newPositions[i] = obj.positions[i];
		newVelocities[i] = obj.velocities[i];
		newDensities[i] = obj.densities[i];
	}

	this->positions = newPositions;
	this->velocities = newVelocities;
	this->densities = newDensities;
	this->nbParticules = obj.nbParticules;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(glm::vec3 position)
{
	glm::vec3	*newPositions = NULL;
	glm::vec3	*newVelocities = NULL;
	double		*newDensities = NULL;

	// Allocate new data arrays
	newPositions = new glm::vec3[this->nbParticules + 1];
	if (newPositions == NULL)
		return ;

	newVelocities = new glm::vec3[this->nbParticules + 1];
	if (newVelocities == NULL)
	{
		delete [] newPositions;
		return ;
	}

	newDensities = new double[this->nbParticules + 1];
	if (newDensities == NULL)
	{
		delete [] newPositions;
		delete [] newVelocities;
		return ;
	}

	// Copy old data
	for (int i = 0; i < this->nbParticules; i++)
	{
		newPositions[i] = this->positions[i];
		newVelocities[i] = this->velocities[i];
		newDensities[i] = this->densities[i];
	}

	// Add new data
	newPositions[this->nbParticules] = position;
	newVelocities[this->nbParticules] = glm::vec3(0.0f, 0.0f, 0.0f);
	newDensities[this->nbParticules] = 0.0;

	// Replace old data arrays by new ones
	this->freeArrays();
	this->positions = newPositions;
	this->velocities = newVelocities;
	this->densities = newDensities;

	this->nbParticules++;
}


void	WaterSimulation::addWaters(std::vector<glm::vec3> positions)
{
	glm::vec3	*newPositions = NULL;
	glm::vec3	*newVelocities = NULL;
	double		*newDensities = NULL;
	int			nbNewParticles = positions.size();

	// Allocate new data arrays
	newPositions = new glm::vec3[this->nbParticules + nbNewParticles];
	if (newPositions == NULL)
		return ;

	newVelocities = new glm::vec3[this->nbParticules + nbNewParticles];
	if (newVelocities == NULL)
	{
		delete [] newPositions;
		return ;
	}

	newDensities = new double[this->nbParticules + nbNewParticles];
	if (newDensities == NULL)
	{
		delete [] newPositions;
		delete [] newVelocities;
		return ;
	}

	// Copy old data
	for (int i = 0; i < this->nbParticules; i++)
	{
		newPositions[i] = this->positions[i];
		newVelocities[i] = this->velocities[i];
		newDensities[i] = this->densities[i];
	}

	// Add new data
	for (int i = 0; i < nbNewParticles; i++)
	{
		newPositions[this->nbParticules + i] = positions[i];
		newVelocities[this->nbParticules + i] = glm::vec3(0.0f, 0.0f, 0.0f);
		newDensities[this->nbParticules + i] = 0.0;
	}

	// Replace old data arrays by new ones
	this->freeArrays();
	this->positions = newPositions;
	this->velocities = newVelocities;
	this->densities = newDensities;

	this->nbParticules += nbNewParticles;
}


void	WaterSimulation::tick(float delta)
{
	for (int i = 0; i < this->nbParticules; i++)
	{
		// Apply gravity
		this->velocities[i] += glm::vec3(0, -0.1, 0) * delta;

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
	}
}


void	WaterSimulation::draw(Camera *camera, ShaderManager *shaderManager)
{
	WaterShader *shader;

	if (this->nbParticules == 0)
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
		glUniform3fv(positionsLoc, 1, glm::value_ptr(this->positions[i]));
	}

	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}

//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::freeArrays(void)
{
	if (this->positions)
	{
		delete [] this->positions;
		this->positions = NULL;
	}
	if (this->velocities)
	{
		delete [] this->velocities;
		this->velocities = NULL;
	}
	if (this->densities)
	{
		delete [] this->densities;
		this->densities = NULL;
	}
}
