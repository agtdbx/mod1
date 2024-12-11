#include <engine/render/Camera.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Camera::Camera(void)
{
	this->view = glm::mat4(1.0f);
	this->view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));

	this->projection = glm::perspective(glm::radians(45.0f), (float)WIN_W/(float)WIN_H, 0.1f, 100.0f);
}


Camera::Camera(const Camera &obj)
{
	this->view = obj.view;
	this->projection = obj.projection;
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

	this->view = obj.view;
	this->projection = obj.projection;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

//**** PRIVATE METHODS *********************************************************
