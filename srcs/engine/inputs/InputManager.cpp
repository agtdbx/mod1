#include <engine/inputs/InputManager.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

InputManager::InputManager(void)
{
	this->mouse = Mouse();
	this->initKeys();
}


InputManager::InputManager(GLFWwindow* window)
{
	this->mouse = Mouse(window);
	this->initKeys();
}



InputManager::InputManager(const InputManager &obj)
{
	this->mouse = obj.mouse;
	this->copyKeys(obj);
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
	this->copyKeys(obj);

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
	this->space.update(window);
	this->lshift.update(window);
	this->lcontrol.update(window);
	this->t.update(window);
	this->tab.update(window);
	this->alt.update(window);
}

//**** PRIVATE METHODS *********************************************************

void	InputManager::initKeys(void)
{
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
	this->space = Key(GLFW_KEY_SPACE);
	this->lshift = Key(GLFW_KEY_LEFT_SHIFT);
	this->lcontrol = Key(GLFW_KEY_LEFT_CONTROL);
	this->t = Key(GLFW_KEY_T);
	this->tab = Key(GLFW_KEY_TAB);
	this->alt = Key(GLFW_KEY_LEFT_ALT);
}


void	InputManager::copyKeys(const InputManager &obj)
{
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
	this->space = obj.space;
	this->lshift = obj.lshift;
	this->lcontrol = obj.lcontrol;
	this->t = obj.t;
	this->tab = obj.tab;
	this->alt = obj.alt;
}
