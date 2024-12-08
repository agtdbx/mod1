#include "Mouse.hpp"

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mouse::Mouse(void)
{
	this->left = Key(GLFW_MOUSE_BUTTON_LEFT);
	this->middle = Key(GLFW_MOUSE_BUTTON_MIDDLE);
	this->right = Key(GLFW_MOUSE_BUTTON_RIGHT);
	this->pos = Vec2(0, 0);
	this->scroll = 0;
}


Mouse::Mouse(const Mouse &obj)
{
	this->left = obj.left;
	this->middle = obj.middle;
	this->right = obj.right;
	this->pos = obj.pos;
	this->scroll = obj.scroll;
}

//---- Destructor --------------------------------------------------------------

Mouse::~Mouse()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

Vec2	Mouse::getPos(void)
{
	return (this->pos);
}


double	Mouse::getScroll(void)
{
	return (this->scroll);
}


bool	Mouse::getVisible(void)
{
	return (this->visible);
}

//---- Setters -----------------------------------------------------------------

void	Mouse::setScroll(double scroll)
{
	this->scroll = scroll;
}


void	Mouse::setVisible(GLFWwindow *window, bool visible)
{
	this->visible = visible;
	if (visible)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

//---- Operators ---------------------------------------------------------------

Mouse	&Mouse::operator=(const Mouse &obj)
{
	if (this == &obj)
		return (*this);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Mouse::update(GLFWwindow *window)
{
	this->left.updateForMouse(window);
	this->middle.updateForMouse(window);
	this->right.updateForMouse(window);

	glfwGetCursorPos(window, &this->pos.x, &this->pos.y);
}

//**** PRIVATE METHODS *********************************************************
