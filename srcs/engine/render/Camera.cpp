#include <engine/render/Camera.hpp>

#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera::Camera(void)
{
	this->projection = glm::perspective(glm::radians(CAMERA_FOV),
										(float)WIN_W/(float)WIN_H,
										CAMERA_MIN_VIEW_DIST, CAMERA_MAX_VIEW_DIST);

	this->position = glm::vec3(-691.0f, 304.0f, 492.0f);
	this->pitch = -6.317865f;
	this->yaw = 0.034562f;
	this->roll = 0.0f;

	this->front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
	this->up = glm::normalize(glm::cross(this->front, this->right));

	this->computeRotation();
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

	if (pitch > 89.0f){
		pitch = 89.0f;
	}
	if (pitch < -89.0f){
		pitch = -89.0f;
	}

	this->computeRotation();
	this->computeView();
}


void	Camera::rotateY(float degrees)
{
	this->yaw += degrees;
	this->computeRotation();
	this->computeView();
}


void	Camera::rotateZ(float degrees)
{
	this->roll += degrees;
	this->computeRotation();
	this->computeView();
}


void	Camera::printCoordonates(void)
{
	printf("pos : %f, %f, %f\n", this->position.x, this->position.y, this->position.z);
	printf("dir : %f, %f, %f\n", this->pitch, this->yaw, this->roll);
}

//**** PRIVATE METHODS *********************************************************

void	Camera::computeRotation(void)
{
	this->front.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
	this->front.y = sin(glm::radians(this->pitch));
	this->front.z = cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
	this->up = glm::normalize(glm::cross(this->front, this->right));
}


void	Camera::computeView(void)
{
	this->view = glm::lookAt(this->position,
							this->position + this->front,
							this->up);
}
