#ifndef POINT_HPP
# define POINT_HPP

# include <engine/maths/Vec3.hpp>

class Point
{
public:
	Vec3	pos;
	float	r, g, b, imgx, imgy;

	Point(void);
	Point(double x, double y, double z);
	Point(double x, double y, double z, int r, int g, int b);
	Point(double x, double y, double z, double r, double g, double b);
	Point(double x, double y, double z, double imgx, double imgy);
	Point(double x, double y, double z, double r, double g, double b, double imgx, double imgy);
	Point(Vec3 &pos);
	Point(Vec3 &pos, int r, int g, int b);
	Point(Vec3 &pos, double r, double g, double b);
	Point(Vec3 &pos, double imgx, double imgy);
	Point(Vec3 &pos, double r, double g, double b, double imgx, double imgy);
	Point(const Point &obj);
	~Point();

	Point	&operator=(const Point &obj);

private:
};

#endif
