#ifndef MOUSE_HPP
# define MOUSE_HPP

# include <engine/inputs/Key.hpp>
# include <engine/maths/Vec2.hpp>

class Mouse
{
public:
	Key	left, middle, right;

	Mouse(void);
	Mouse(GLFWwindow* window);
	Mouse(const Mouse &obj);
	~Mouse();

	Vec2	getPos(void);
	double	getScroll(void);
	bool	getVisible(void);

	void	setVisible(GLFWwindow *window, bool visible);

	Mouse	&operator=(const Mouse &obj);

	void	update(GLFWwindow *window);
	void	goTo(GLFWwindow *window, float x, float y);


private:
	Vec2	pos;
	double	scroll;
	bool	visible;
};

#endif
