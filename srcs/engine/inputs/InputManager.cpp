#include "InputManager.hpp"

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

InputManager::InputManager(void)
{
	this->mouse = Mouse();
	this->escape = Key(GLFW_KEY_ESCAPE);
	this->e = Key(GLFW_KEY_E);
}


InputManager::InputManager(const InputManager &obj)
{
	this->mouse = obj.mouse;
	this->escape = obj.escape;
	this->e = obj.e;
}

//---- Destructor --------------------------------------------------------------

InputManager::~InputManager()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

InputManager	&InputManager::operator=(const InputManager &obj)
{
	if (this == &obj)
		return (*this);

	this->mouse = obj.mouse;
	this->escape = obj.escape;
	this->e = obj.e;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	InputManager::update(GLFWwindow *window)
{
	this->mouse.update(window);

	this->escape.update(window);
	this->e.update(window);
}

//**** PRIVATE METHODS *********************************************************
