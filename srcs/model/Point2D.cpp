#include <model/Point2D.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Point2D::Point2D(void)
{
	this->pos = Vec2(0, 0);
	this->r = 0.5;
	this->g = 0.5;
	this->b = 0.5;
}


Point2D::Point2D(Vec2 pos)
{
	this->pos = pos;
	this->r = 0.5;
	this->g = 0.5;
	this->b = 0.5;
}


Point2D::Point2D(Vec2 pos, double r, double g, double b)
{
	this->pos = pos;
	this->r = r;
	this->g = g;
	this->b = b;
}


Point2D::Point2D(const Point2D &obj)
{
	this->pos = obj.pos;
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;
}

//---- Destructor --------------------------------------------------------------

Point2D::~Point2D()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

Point2D	&Point2D::operator=(const Point2D &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->r = obj.r;
	this->g = obj.g;
	this->b = obj.b;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

//**** PRIVATE METHODS *********************************************************
