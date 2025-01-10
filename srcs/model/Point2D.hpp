#ifndef POINT2D_HPP
# define POINT2D_HPP

# include <engine/maths/Vec2.hpp>

class Point2D
{
public:
	Vec2	pos;
	float	r, g, b;

	Point2D(void);
	Point2D(Vec2 pos);
	Point2D(Vec2 pos, double r, double g, double b);
	Point2D(const Point2D &obj);
	~Point2D();

	Point2D	&operator=(const Point2D &obj);

private:
};

#endif
