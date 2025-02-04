#ifndef POINT_HPP
# define POINT_HPP

# include <engine/maths/Vec3.hpp>

class Point
{
public:
	Vec3	pos, normal;
	float	r, g, b;

	Point(void);
	Point(Vec3 pos);
	Point(Vec3 pos, Vec3 normal);
	Point(Vec3 pos, double r, double g, double b);
	Point(Vec3 pos, Vec3 normal, double r, double g, double b);
	Point(Vec3 pos, Vec3 normal, Vec3 color);
	Point(const Point &obj);
	~Point();

	Point	&operator=(const Point &obj);

private:
};

#endif
