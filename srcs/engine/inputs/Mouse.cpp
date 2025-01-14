#include <engine/inputs/Mouse.hpp>

//**** STATIC VARIABLES ********************************************************
static double	mouseScroll = 0.0;

//**** STATIC FUNCTIONS ********************************************************
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	mouseScroll = yoffset;
}


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


Mouse::Mouse(GLFWwindow* window)
{
	this->left = Key(GLFW_MOUSE_BUTTON_LEFT);
	this->middle = Key(GLFW_MOUSE_BUTTON_MIDDLE);
	this->right = Key(GLFW_MOUSE_BUTTON_RIGHT);
	this->pos = Vec2(0, 0);
	this->scroll = 0;
	glfwSetScrollCallback(window, scroll_callback);
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

	this->scroll = mouseScroll;
	mouseScroll = 0.0;
}

void	Mouse::goTo(GLFWwindow *window, float x, float y)
{
	this->pos.x = x;
	this->pos.y = y;
	glfwSetCursorPos(window, x, y);
}

//**** PRIVATE METHODS *********************************************************
