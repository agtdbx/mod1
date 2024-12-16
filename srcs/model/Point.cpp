#include <model/Point.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Point::Point(void)
{
	this->pos = Vec3(0, 0, 0);
	this->normal = Vec3(0, 1, 0);
	this->r = 0.5;
	this->g = 0.5;
	this->b = 0.5;
}


Point::Point(Vec3 pos)
{
	this->pos = pos;
	this->normal = Vec3(0, 1, 0);
	this->r = 0.5;
	this->g = 0.5;
	this->b = 0.5;
}


Point::Point(Vec3 pos, Vec3 normal)
{
	this->pos = pos;
	this->normal = normal;
	this->r = 0.5;
	this->g = 0.5;
	this->b = 0.5;
}


Point::Point(Vec3 pos, double r, double g, double b)
{
	this->pos = pos;
	this->normal = Vec3(0, 1, 0);
	this->r = r;
	this->g = g;
	this->b = b;
}


Point::Point(Vec3 pos, Vec3 normal, double r, double g, double b)
{
	this->pos = pos;
	this->normal = normal;
	this->r = r;
	this->g = g;
	this->b = b;
}


Point::Point(const Point &obj)
{
	this->pos = obj.pos;
	this->normal = obj.normal;
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;
}

//---- Destructor --------------------------------------------------------------

Point::~Point()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Point	&Point::operator=(const Point &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->normal = obj.normal;
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

//**** PRIVATE METHODS *********************************************************
