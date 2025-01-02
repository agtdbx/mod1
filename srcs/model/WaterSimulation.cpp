#include <model/WaterSimulation.hpp>

#include <engine/render/WaterShader.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

WaterSimulation::WaterSimulation(void)
{
	this->nbParticules = 0;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
}


WaterSimulation::WaterSimulation(const WaterSimulation &obj)
{
	this->nbParticules = obj.nbParticules;

	this->positions = obj.positions;
	this->velocities = obj.velocities;
	this->densities = obj.densities;

	this->generateTextureBuffer();
	this->generateTriangleOverScreen();
}

//---- Destructor --------------------------------------------------------------

WaterSimulation::~WaterSimulation()
{
	glDeleteBuffers(1, &this->textureBuffer);
	glDeleteTextures(1, &this->texture);
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

	this->positions = obj.positions;
	this->velocities = obj.velocities;
	this->densities = obj.densities;
	this->nbParticules = obj.nbParticules;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	WaterSimulation::addWater(glm::vec3 position)
{
	this->positions.push_back(position);
	this->velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	this->densities.push_back(0.0);
	this->nbParticules++;
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

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 13, this->triangleOverScreen, GL_STATIC_DRAW);

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

	glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * this->nbParticules, this->positions.data(), GL_STATIC_DRAW);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, this->textureBuffer);

	glUniform1i(glGetUniformLocation(shader->getShaderId(), "positionsBuffer"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, this->texture);

	glBindVertexArray(shaderManager->getVAOId());
	glDrawArrays(GL_TRIANGLES, 0, 12);
}

//**** PRIVATE METHODS *********************************************************

void	WaterSimulation::generateTextureBuffer(void)
{
	this->textureBuffer = 0;
	glGenBuffers(1, &this->textureBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, this->textureBuffer);

	this->texture = 0;
	glGenTextures(1, &texture);
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
