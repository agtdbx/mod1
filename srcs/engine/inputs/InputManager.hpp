#ifndef INPUTMANAGER_HPP
# define INPUTMANAGER_HPP

# include <engine/inputs/Key.hpp>
# include <engine/inputs/Mouse.hpp>

class InputManager
{
public:
	Mouse	mouse;
	Key		escape, e;

	InputManager(void);
	InputManager(const InputManager &obj);
	~InputManager();

	InputManager	&operator=(const InputManager &obj);

	void	update(GLFWwindow *window);

private:
};

#endif
