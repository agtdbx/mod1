#include <engine/maths/Vec2.hpp>
#include <engine/maths/math_constants.hpp>

#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Vec2::Vec2(void)
{
	this->x = 0.0;
	this->y = 0.0;
}


Vec2::Vec2(const double x, const double y)
{
	this->x = x;
	this->y = y;
}


Vec2::Vec2(const Vec2 &vec2)
{
	this->x = vec2.x;
	this->y = vec2.y;
}

//---- Destructor --------------------------------------------------------------

Vec2::~Vec2()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

bool	Vec2::isNormalize(void) const
{
	return (this->getNorm() == 1.0);
}


double	Vec2::getNorm(void) const
{
	return (sqrt((this->x * this->x) + (this->y * this->y)));
}

//**** PUBLIC METHODS **********************************************************
//---- Method that modify vector -----------------------------------------------

void	Vec2::normalize(void)
{
	double	norm = this->getNorm();

	if (norm == 0.0 || norm == 1.0)
		return;

	norm = 1.0 / norm;

	this->x *= norm;
	this->y *= norm;
}


void	Vec2::translate(const Vec2 &dir, const double length)
{
	this->x += dir.x * length;
	this->y += dir.y * length;
}


void	Vec2::rotate(const double degrees)
{
	double radian = degrees * pi_div_180;

	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) - (sinAngle * y);
	this->y = (sinAngle * x) + (cosAngle * y);
}


void	Vec2::rotateRadian(double radian)
{
	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) - (sinAngle * y);
	this->y = (sinAngle * x) + (cosAngle * y);
}


void	Vec2::rotateArround(const Vec2 &arrond_vec, const double degrees)
{
	double radian = degrees * pi_div_180;

	this->x -= arrond_vec.x;
	this->y -= arrond_vec.y;

	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) - (sinAngle * y);
	this->y = (sinAngle * x) + (cosAngle * y);

	this->x += arrond_vec.x;
	this->y += arrond_vec.y;
}


void	Vec2::rotateArroundRadian(const Vec2 &arrond_vec, const double radian)
{
	this->x -= arrond_vec.x;
	this->y -= arrond_vec.y;

	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) - (sinAngle * y);
	this->y = (sinAngle * x) + (cosAngle * y);

	this->x += arrond_vec.x;
	this->y += arrond_vec.y;
}

//---- Method that return values -----------------------------------------------

double	Vec2::dot(const Vec2 &vec2) const
{
	return ((this->x * vec2.x) + (this->y * vec2.y));
}


double	Vec2::cross(const Vec2 &vec2) const
{
	return ((this->x * vec2.y) - (this->y * vec2.x));
}

double	Vec2::distanceWith(const Vec2 &vec2) const
{
	double	xDiff = this->x - vec2.x;
	double	yDiff = this->y - vec2.y;

	return (sqrt((xDiff * xDiff) + (yDiff * yDiff)));
}


//**** MEMBER OPERATORS ********************************************************
//---- Modify vector vector operator -------------------------------------------

Vec2	&Vec2::operator=(const Vec2 &vec2)
{
	this->x = vec2.x;
	this->y = vec2.y;

	return (*this);
}


Vec2	&Vec2::operator+=(const Vec2 &vec2)
{
	this->x += vec2.x;
	this->y += vec2.y;

	return (*this);
}


Vec2	&Vec2::operator-=(const Vec2 &vec2)
{
	this->x -= vec2.x;
	this->y -= vec2.y;

	return (*this);
}

//---- Modify vector number operator -------------------------------------------

Vec2	&Vec2::operator*=(const double number)
{
	this->x *= number;
	this->y *= number;

	return (*this);
}


Vec2	&Vec2::operator/=(const double number)
{
	if (number == 0.0f)
		return (*this);

	double inverse = 1.0 / number;
	this->x *= inverse;
	this->y *= inverse;

	return (*this);
}

//---- Comparaison operator ----------------------------------------------------

bool	Vec2::operator==(const Vec2 &vec2) const
{
	return (this->x == vec2.x && this->y == vec2.y);
}

bool	Vec2::operator!=(const Vec2 &vec2) const
{
	return (this->x != vec2.x || this->y != vec2.y);

}


//**** NON MEMBER OPERATORS ****************************************************
//---- Print operator ----------------------------------------------------------

std::ostream&	operator<<(std::ostream &os, const Vec2 &vec2)
{
	os << '(' << vec2.x << ", " << vec2.y << ')';

	return (os);
}

//---- Vector vector operator --------------------------------------------------

inline Vec2		operator+(const Vec2 &v1, const Vec2 &v2)
{
	return (Vec2(v1.x + v2.x, v1.y + v2.y));
}


inline Vec2		operator-(const Vec2 &v1, const Vec2 &v2)
{
	return (Vec2(v1.x - v2.x, v1.y - v2.y));
}

//---- Vector number operator --------------------------------------------------
inline Vec2		operator*(const Vec2 &vec2, const double number)
{
	return (Vec2(vec2.x * number, vec2.y * number));
}

inline Vec2		operator*(const double number, const Vec2 &vec2)
{
	return (Vec2(vec2.x * number, vec2.y * number));
}

inline Vec2		operator/(const Vec2 &vec2, const double number)
{
	if (number == 0.0)
		return (Vec2());

	double inverse = 1.0 / number;
	return (Vec2(vec2.x * inverse, vec2.y * inverse));
}


//**** FUNCTIONS ***************************************************************
//---- Vector vector functions -------------------------------------------------

inline double	vec2Dot(const Vec2 &v1, const Vec2 &v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y));
}


inline double	vec2Cross(const Vec2 &v1, const Vec2 &v2)
{
	return ((v1.x * v2.y) - (v1.y * v2.x));
}
