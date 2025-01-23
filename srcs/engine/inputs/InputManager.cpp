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
	this->_0.update(window);
	this->_1.update(window);
	this->_2.update(window);
	this->_3.update(window);
	this->_4.update(window);
	this->_5.update(window);
	this->_6.update(window);
	this->_7.update(window);
	this->_8.update(window);
	this->_9.update(window);
	this->del.update(window);
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
	this->_0 = Key(GLFW_KEY_0);
	this->_1 = Key(GLFW_KEY_1);
	this->_2 = Key(GLFW_KEY_2);
	this->_3 = Key(GLFW_KEY_3);
	this->_4 = Key(GLFW_KEY_4);
	this->_5 = Key(GLFW_KEY_5);
	this->_6 = Key(GLFW_KEY_6);
	this->_7 = Key(GLFW_KEY_7);
	this->_8 = Key(GLFW_KEY_8);
	this->_9 = Key(GLFW_KEY_9);
	this->del = Key(GLFW_KEY_BACKSPACE);
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
	this->_0 = obj._0;
	this->_1 = obj._1;
	this->_2 = obj._2;
	this->_3 = obj._3;
	this->_4 = obj._4;
	this->_5 = obj._5;
	this->_6 = obj._6;
	this->_7 = obj._7;
	this->_8 = obj._8;
	this->_9 = obj._9;
	this->del = obj.del;
}
