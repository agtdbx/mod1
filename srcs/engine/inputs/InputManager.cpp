#include <engine/inputs/InputManager.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

InputManager::InputManager(void)
{
	this->mouse = Mouse();
	this->escape = Key(GLFW_KEY_ESCAPE);
	this->w = Key(GLFW_KEY_W);
	this->z = Key(GLFW_KEY_Z);
	this->a = Key(GLFW_KEY_A);
	this->q = Key(GLFW_KEY_Q);
	this->s = Key(GLFW_KEY_S);
	this->d = Key(GLFW_KEY_D);
	this->up = Key(GLFW_KEY_UP);
	this->down = Key(GLFW_KEY_DOWN);
	this->left = Key(GLFW_KEY_LEFT);
	this->right = Key(GLFW_KEY_RIGHT);
}


InputManager::InputManager(const InputManager &obj)
{
	this->mouse = obj.mouse;
	this->escape = obj.escape;
	this->w = obj.w;
	this->z = obj.z;
	this->a = obj.a;
	this->q = obj.q;
	this->s = obj.s;
	this->d = obj.d;
	this->up = obj.up;
	this->down = obj.down;
	this->left = obj.left;
	this->right = obj.right;
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
	this->w = obj.w;
	this->z = obj.z;
	this->a = obj.a;
	this->q = obj.q;
	this->s = obj.s;
	this->d = obj.d;
	this->up = obj.up;
	this->down = obj.down;
	this->left = obj.left;
	this->right = obj.right;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	InputManager::update(GLFWwindow *window)
{
	this->mouse.update(window);

	this->escape.update(window);
	this->w.update(window);
	this->z.update(window);
	this->a.update(window);
	this->q.update(window);
	this->s.update(window);
	this->d.update(window);
	this->up.update(window);
	this->down.update(window);
	this->left.update(window);
	this->right.update(window);
}

//**** PRIVATE METHODS *********************************************************
