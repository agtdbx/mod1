#include <engine/inputs/Key.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Key::Key(void)
{
	this->keycode = 0;
	this->pressed = false;
	this->down = false;
	this->released = false;
}


Key::Key(int keycode)
{
	this->keycode = keycode;
	this->pressed = false;
	this->down = false;
	this->released = false;
}


Key::Key(const Key &obj)
{
	this->keycode = obj.keycode;
	this->pressed = obj.pressed;
	this->down = obj.down;
	this->released = obj.released;
}

//---- Destructor --------------------------------------------------------------

Key::~Key()
{
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

bool	Key::isPressed(void)
{
	return (this->pressed);
}


bool	Key::isReleased(void)
{
	return (this->released);
}


bool	Key::isDown(void)
{
	return (this->down);
}

//---- Setters -----------------------------------------------------------------

void	Key::update(GLFWwindow *window)
{
	int status;

	status = glfwGetKey(window, this->keycode);

	if (status == GLFW_PRESS && !this->down)
	{
		this->pressed = true;
		this->down = true;
		this->released = false;
	}
	else if (status == GLFW_RELEASE && this->down)
	{
		this->pressed = false;
		this->down = false;
		this->released = true;
	}
	else
	{
		this->pressed = false;
		this->released = false;
	}
}


void	Key::updateForMouse(GLFWwindow *window)
{
	int status;

	status = glfwGetMouseButton(window, this->keycode);

	if (status == GLFW_PRESS && !this->down)
	{
		this->pressed = true;
		this->down = true;
		this->released = false;
	}
	else if (status == GLFW_RELEASE && this->down)
	{
		this->pressed = false;
		this->down = false;
		this->released = true;
	}
	else
	{
		this->pressed = false;
		this->released = false;
	}
}

//---- Operators ---------------------------------------------------------------

Key	&Key::operator=(const Key &obj)
{
	if (this == &obj)
		return (*this);

	this->keycode = obj.keycode;
	this->pressed = obj.pressed;
	this->down = obj.down;
	this->released = obj.released;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************


//**** PRIVATE METHODS *********************************************************
