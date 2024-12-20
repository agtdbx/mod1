#include <engine/render/Camera.hpp>

#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera::Camera(void)
{
	this->projection = glm::perspective(glm::radians(CAMERA_FOV),
										WIN_RATIO,
										CAMERA_MIN_VIEW_DIST, CAMERA_MAX_VIEW_DIST);

	this->planeHeight = tan(CAMERA_FOV * 0.5f * (3.141592 / 180)) * 2;
	this->planeWidth = planeHeight * WIN_RATIO;

	this->lightPosition = glm::vec3(100.0f, 200.0f, 100.0f);

	this->position = glm::vec3(-127.0f, 75.0f, 103.0f);
	this->pitch = -15.463547f;
	this->yaw = -0.144872f;
	this->roll = 0.0f;

	// this->position = glm::vec3(-6.0f, 7.5f, 3.0f);
	// this->pitch = -15.463547f;
	// this->yaw = -0.144872f;
	// this->roll = 0.0f;

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
	this->lightPosition = obj.lightPosition;
	this->planeWidth = obj.planeWidth;
	this->planeHeight = obj.planeHeight;
}

//---- Destructor --------------------------------------------------------------

Camera::~Camera()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

glm::vec3	Camera::getPosition(void)
{
	return (this->position);
}


glm::vec3	Camera::getFront(void)
{
	return (this->front);
}


glm::vec3	Camera::getRight(void)
{
	return (this->right);
}


glm::vec3	Camera::getUp(void)
{
	return (this->up);
}


glm::mat4	Camera::getView(void)
{
	return (this->view);
}


glm::mat4	Camera::getProjection(void)
{
	return (this->projection);
}


glm::vec3	Camera::getLightPosition(void)
{
	return (this->lightPosition);
}


float	Camera::getPlaneWidth(void)
{
	return (this->planeWidth);
}


float	Camera::getPlaneHeight(void)
{
	return (this->planeHeight);
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
	this->lightPosition = obj.lightPosition;
	this->planeWidth = obj.planeWidth;
	this->planeHeight = obj.planeHeight;

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


void	Camera::printInfo(void)
{
	printf("pos : %f, %f, %f\n", this->position.x, this->position.y, this->position.z);
	printf("dir : %f, %f, %f\n", this->front.x, this->front.y, this->front.z);
	printf("axis : %f, %f, %f\n", this->pitch, this->yaw, this->roll);
	printf("\n");
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
