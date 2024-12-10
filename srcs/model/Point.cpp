#include <model/Point.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Point::Point(void)
{
	this->pos = Vec3();
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
	this->imgx = 0.0;
	this->imgy = 0.0;
}

Point::Point(double x, double y, double z)
{
	this->pos = Vec3(x, y, z);
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(double x, double y, double z, int r, int g, int b)
{
	this->pos = Vec3(x, y, z);
	this->r = (double)r / 255.0;
	this->g = (double)g / 255.0;
	this->b = (double)b / 255.0;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(double x, double y, double z, double r, double g, double b)
{
	this->pos = Vec3(x, y, z);
	this->r = r;
	this->g = g;
	this->b = b;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(double x, double y, double z, double imgx, double imgy)
{
	this->pos = Vec3(x, y, z);
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
	this->imgx = imgx;
	this->imgy = imgy;
}


Point::Point(double x, double y, double z, double r, double g, double b, double imgx, double imgy)
{
	this->pos = Vec3(x, y, z);
	this->r = r;
	this->g = g;
	this->b = b;
	this->imgx = imgx;
	this->imgy = imgy;
}


Point::Point(Vec3 &pos)
{
	this->pos = pos;
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(Vec3 &pos, int r, int g, int b)
{
	this->pos = pos;
	this->r = (double)r / 255.0;
	this->g = (double)g / 255.0;
	this->b = (double)b / 255.0;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(Vec3 &pos, double r, double g, double b)
{
	this->pos = pos;
	this->r = r;
	this->g = g;
	this->b = b;
	this->imgx = 0.0;
	this->imgy = 0.0;
}


Point::Point(Vec3 &pos, double imgx, double imgy)
{
	this->pos = pos;
	this->r = 0.0;
	this->g = 0.0;
	this->b = 0.0;
	this->imgx = imgx;
	this->imgy = imgy;
}


Point::Point(Vec3 &pos, double r, double g, double b, double imgx, double imgy)
{
	this->pos = pos;
	this->r = r;
	this->g = g;
	this->b = b;
	this->imgx = imgx;
	this->imgy = imgy;
}


Point::Point(const Point &obj)
{
	this->pos = obj.pos;
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;
	this->imgx = obj.imgx;
	this->imgy = obj.imgy;
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
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;
	this->imgx = obj.imgx;
	this->imgy = obj.imgy;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

//**** PRIVATE METHODS *********************************************************
