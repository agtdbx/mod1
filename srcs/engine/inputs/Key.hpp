#ifndef KEY_HPP
# define KEY_HPP

# include <GL/glew.h>
# include <GLFW/glfw3.h>

class Key
{
public:
	Key(void);
	Key(int keycode);
	Key(const Key &obj);
	~Key();

	bool	isPressed(void);
	bool	isReleased(void);
	bool	isDown(void);

	Key	&operator=(const Key &obj);

	void	update(GLFWwindow *window);
	void	updateForMouse(GLFWwindow *window);

private:
	int		keycode;
	bool	pressed, released, down;
};

#endif
