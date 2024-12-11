#include <engine/render/Camera.hpp>

#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera::Camera(void)
{
	glm::vec3	up = glm::vec3(0.0f, 1.0f, 0.0f);

	this->projection = glm::perspective(glm::radians(45.0f),
										(float)WIN_W/(float)WIN_H, 0.1f, 100.0f);

	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->roll = 0.0f;
	this->position = glm::vec3(0.0f, 0.0f, 3.0f);

	this->front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
	this->right = glm::normalize(glm::cross(up, this->front));
	this->up = glm::normalize(glm::cross(this->front, this->right));

	this->computeView();
}


Camera::Camera(const Camera &obj)
{
	this->pitch = obj.pitch;
	this->yaw = obj.yaw;
	this->roll = obj.roll;
	this->view = obj.view;
	this->projection = obj.projection;
	this->position = obj.position;
	this->front = obj.front;
	this->up = obj.up;
	this->right = obj.right;
}

//---- Destructor --------------------------------------------------------------

Camera::~Camera()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

glm::mat4	Camera::getView(void)
{
	return (this->view);
}


glm::mat4	Camera::getProjection(void)
{
	return (this->projection);
}

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Camera	&Camera::operator=(const Camera &obj)
{
	if (this == &obj)
		return (*this);

	this->pitch = obj.pitch;
	this->yaw = obj.yaw;
	this->roll = obj.roll;
	this->view = obj.view;
	this->projection = obj.projection;
	this->position = obj.position;
	this->front = obj.front;
	this->up = obj.up;
	this->right = obj.right;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Camera::moveFront(float speed)
{
	this->position += this->front * speed;
	this->computeView();
}


void	Camera::moveUp(float speed)
{
	this->position += this->up * speed;
	this->computeView();
}


void	Camera::moveRight(float speed)
{
	this->position += this->right * speed;
	this->computeView();
}

void	Camera::rotateX(float degrees)
{
	this->pitch += degrees;
	this->computeView();
}


void	Camera::rotateY(float degrees)
{
	this->yaw += degrees;
	this->computeView();
}


void	Camera::rotateZ(float degrees)
{
	this->roll += degrees;
	this->computeView();
}

//**** PRIVATE METHODS *********************************************************

void	Camera::computeView(void)
{
	this->front.x = glm::radians(this->pitch) * cos(glm::radians(this->yaw));
	this->front.y = glm::radians(this->pitch);
	this->front.z = glm::radians(this->pitch) * sin(glm::radians(this->yaw));

	this->view = glm::lookAt(this->position,
							this->position + this->front,
							this->up);
}
