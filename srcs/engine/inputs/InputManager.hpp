#ifndef INPUTMANAGER_HPP
# define INPUTMANAGER_HPP

# include <engine/inputs/Key.hpp>
# include <engine/inputs/Mouse.hpp>

class InputManager
{
public:
	Mouse	mouse;
	Key		escape, w, z, a, q, s, d, up, down, left, right, space, lshift, lcontrol, t, tab;

	InputManager(void);
	InputManager(GLFWwindow* window);
	InputManager(const InputManager &obj);
	~InputManager();

	InputManager	&operator=(const InputManager &obj);

	void	update(GLFWwindow *window);

private:
	void	initKeys(void);
	void	copyKeys(const InputManager &obj);
};

#endif
