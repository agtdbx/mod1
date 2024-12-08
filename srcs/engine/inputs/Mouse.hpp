#ifndef MOUSE_HPP
# define MOUSE_HPP

# include <engine/inputs/Key.hpp>
# include <engine/maths/Vec2.hpp>

class Mouse
{
public:
	Key	left, middle, right;

	Mouse(void);
	Mouse(const Mouse &obj);
	~Mouse();

	Vec2	getPos(void);
	double	getScroll(void);

	Mouse	&operator=(const Mouse &obj);

	void	update(GLFWwindow *window);

private:
	Vec2	pos;
	double	scroll;
};

#endif
