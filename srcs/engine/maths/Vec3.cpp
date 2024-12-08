#include <engine/maths/Vec3.hpp>
#include <engine/maths/math_constants.hpp>

#include <cmath>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Vec3::Vec3(void)
{
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}


Vec3::Vec3(const double x, const double y, const double z)
{
	this->x = x;
	this->y = y;
	this->y = y;
}


Vec3::Vec3(const Vec3 &vec3)
{
	this->x = vec3.x;
	this->y = vec3.y;
	this->z = vec3.z;
}

//---- Destructor --------------------------------------------------------------

Vec3::~Vec3()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

bool	Vec3::isNormalize(void) const
{
	return (this->getNorm() == 1.0);
}


double	Vec3::getNorm(void) const
{
	return (sqrt((this->x * this->x)
					+ (this->y * this->y)
					+ (this->z * this->z)));
}

//**** PUBLIC METHODS **********************************************************
//---- Method that modify vector -----------------------------------------------

void	Vec3::normalize(void)
{
	double	norm = this->getNorm();

	if (norm == 0.0 || norm == 1.0)
		return;

	norm = 1.0 / norm;

	this->x *= norm;
	this->y *= norm;
	this->z *= norm;
}


void	Vec3::translate(const Vec3 &dir, const double length)
{
	this->x += dir.x * length;
	this->y += dir.y * length;
	this->z += dir.z * length;
}


void	Vec3::rotateX(const double degrees)
{
	double radian = degrees * pi_div_180;

	double	y = this->y;
	double	z = this->z;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->y = (cosAngle * y) + (sinAngle * z);
	this->z = (-sinAngle * y) + (cosAngle * z);
}


void	Vec3::rotateXRadian(const double radian)
{
	double	y = this->y;
	double	z = this->z;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->y = (cosAngle * y) + (sinAngle * z);
	this->z = (-sinAngle * y) + (cosAngle * z);
}


void	Vec3::rotateY(const double degrees)
{
	double radian = degrees * pi_div_180;

	double	x = this->x;
	double	z = this->z;;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) + (sinAngle * z);
	this->z = -(sinAngle * x) + (cosAngle * z);
}


void	Vec3::rotateYRadian(const double radian)
{
	double	x = this->x;
	double	z = this->z;;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) + (sinAngle * z);
	this->z = -(sinAngle * x) + (cosAngle * z);
}


void	Vec3::rotateZ(const double degrees)
{
	double radian = degrees * pi_div_180;

	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) + (sinAngle * y);
	this->y = (-sinAngle * x) + (cosAngle * y);
}


void	Vec3::rotateZRadian(const double radian)
{
	double	x = this->x;
	double	y = this->y;
	double	cosAngle = cos(radian);
	double	sinAngle = sin(radian);

	this->x = (cosAngle * x) + (sinAngle * y);
	this->y = (-sinAngle * x) + (cosAngle * y);
}


void	Vec3::rotateArround(const Vec3 &vec3, const double degrees)
{
	double radian = degrees * pi_div_180;

	const double	cosAngle = cos(radian);
	const double	sinAngle = sin(radian);
	const double	invCosAngle = 1.0 - cosAngle;
	const double	squareVec3X = vec3.x * vec3.x;
	const double	squareVec3Y = vec3.y * vec3.y;
	const double	squareVec3Z = vec3.z * vec3.z;
	const double	invCosAnglePerVec3X = invCosAngle * vec3.x;
	const double	invCosAnglePerVec3XY = invCosAnglePerVec3X * vec3.y;
	const double	invCosAnglePerVec3XZ = invCosAnglePerVec3X * vec3.z;
	const double	invCosAnglePerVec3YZ = invCosAngle * vec3.y * vec3.z;
	Vec3	ret;
	Vec3	vec3PerSinAngle = vec3 * sinAngle;

	ret.y = this->dot(Vec3(invCosAnglePerVec3XY + vec3PerSinAngle.z,
							cosAngle + (invCosAngle * squareVec3Y),
							invCosAnglePerVec3YZ - vec3PerSinAngle.x));

	if (fabs(this->y) >= 0.995 && fabs(ret.y) >= fabs(this->y))
		return;

	ret.x = this->dot(Vec3(cosAngle + (invCosAngle * squareVec3X),
							invCosAnglePerVec3XY - vec3PerSinAngle.z,
							invCosAnglePerVec3XZ + vec3PerSinAngle.y));
	ret.z = this->dot(Vec3(invCosAnglePerVec3XZ - vec3PerSinAngle.y,
							invCosAnglePerVec3YZ + vec3PerSinAngle.x,
							cosAngle + (invCosAngle * squareVec3Z)));
	this->x = ret.x;
	this->y = ret.y;
	this->z = ret.z;
}


void	Vec3::rotateArroundRadian(const Vec3 &vec3, const double radian)
{
	const double	cosAngle = cos(radian);
	const double	sinAngle = sin(radian);
	const double	invCosAngle = 1.0 - cosAngle;
	const double	squareVec3X = vec3.x * vec3.x;
	const double	squareVec3Y = vec3.y * vec3.y;
	const double	squareVec3Z = vec3.z * vec3.z;
	const double	invCosAnglePerVec3X = invCosAngle * vec3.x;
	const double	invCosAnglePerVec3XY = invCosAnglePerVec3X * vec3.y;
	const double	invCosAnglePerVec3XZ = invCosAnglePerVec3X * vec3.z;
	const double	invCosAnglePerVec3YZ = invCosAngle * vec3.y * vec3.z;
	Vec3	ret;
	Vec3	vec3PerSinAngle = vec3 * sinAngle;

	ret.y = this->dot(Vec3(invCosAnglePerVec3XY + vec3PerSinAngle.z,
							cosAngle + (invCosAngle * squareVec3Y),
							invCosAnglePerVec3YZ - vec3PerSinAngle.x));

	if (fabs(this->y) >= 0.995 && fabs(ret.y) >= fabs(this->y))
		return;

	ret.x = this->dot(Vec3(cosAngle + (invCosAngle * squareVec3X),
							invCosAnglePerVec3XY - vec3PerSinAngle.z,
							invCosAnglePerVec3XZ + vec3PerSinAngle.y));
	ret.z = this->dot(Vec3(invCosAnglePerVec3XZ - vec3PerSinAngle.y,
							invCosAnglePerVec3YZ + vec3PerSinAngle.x,
							cosAngle + (invCosAngle * squareVec3Z)));
	this->x = ret.x;
	this->y = ret.y;
	this->z = ret.z;
}

//---- Method that return values -----------------------------------------------

double	Vec3::dot(const Vec3 &vec3) const
{
	return ((this->x * vec3.x) + (this->y * vec3.y) + (this->z * vec3.z));
}


Vec3	Vec3::cross(const Vec3 &vec3) const
{
	return (Vec3((this->y * vec3.z) - (this->z * vec3.y),
					(this->z * vec3.x) - (this->x - vec3.z),
					(this->x * vec3.y) - (this->y * vec3.x)));
}

double	Vec3::distanceWith(const Vec3 &vec3) const
{
	double	xDiff = this->x - vec3.x;
	double	yDiff = this->y - vec3.y;
	double	zDiff = this->z - vec3.z;

	return (sqrt((xDiff * xDiff) + (yDiff * yDiff) + (zDiff * zDiff)));
}


//**** MEMBER OPERATORS ********************************************************
//---- Modify vector vector operator -------------------------------------------

Vec3	&Vec3::operator=(const Vec3 &vec3)
{
	this->x = vec3.x;
	this->y = vec3.y;
	this->z = vec3.z;

	return (*this);
}


Vec3	&Vec3::operator+=(const Vec3 &vec3)
{
	this->x += vec3.x;
	this->y += vec3.y;
	this->z += vec3.z;

	return (*this);
}


Vec3	&Vec3::operator-=(const Vec3 &vec3)
{
	this->x -= vec3.x;
	this->y -= vec3.y;
	this->z -= vec3.z;

	return (*this);
}

//---- Modify vector number operator -------------------------------------------

Vec3	&Vec3::operator*=(const double number)
{
	this->x *= number;
	this->y *= number;
	this->z *= number;

	return (*this);
}


Vec3	&Vec3::operator/=(const double number)
{
	if (number == 0.0f)
		return (*this);

	double inverse = 1 / number;
	this->x *= inverse;
	this->y *= inverse;
	this->z *= inverse;

	return (*this);
}

//---- Comparaison operator ----------------------------------------------------

bool	Vec3::operator==(const Vec3 &vec3) const
{
	return (this->x == vec3.x && this->y == vec3.y && this->z == vec3.z);
}

bool	Vec3::operator!=(const Vec3 &vec3) const
{
	return (this->x != vec3.x || this->y != vec3.y || this->z != vec3.z);

}


//**** NON MEMBER OPERATORS ****************************************************
//---- Print operator ----------------------------------------------------------

std::ostream&	operator<<(std::ostream &os, const Vec3 &vec3)
{
	os << '(' << vec3.x << ", " << vec3.y << ", " << vec3.z << ')';

	return (os);
}

//---- Vector vector operator --------------------------------------------------

inline Vec3		operator+(const Vec3 &v1, const Vec3 &v2)
{
	return (Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
}


inline Vec3		operator-(const Vec3 &v1, const Vec3 &v2)
{
	return (Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
}

//---- Vector number operator --------------------------------------------------
inline Vec3		operator*(const Vec3 &vec3, const double number)
{
	return (Vec3(vec3.x * number, vec3.y * number, vec3.z * number));
}

inline Vec3		operator*(const double number, const Vec3 &vec3)
{
	return (Vec3(vec3.x * number, vec3.y * number, vec3.z * number));
}

inline Vec3		operator/(const Vec3 &vec3, const double number)
{
	if (number == 0.0)
		return (Vec3());

	double inverse = 1 / number;
	return (Vec3(vec3.x * inverse, vec3.y * inverse, vec3.z * inverse));
}


//**** FUNCTIONS ***************************************************************
//---- Vector vector functions -------------------------------------------------

inline double	Vec3Dot(const Vec3 &v1, const Vec3 &v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}


inline Vec3	Vec3Cross(const Vec3 &v1, const Vec3 &v2)
{
	return (Vec3((v1.y * v2.z) - (v1.z * v2.y),
					(v1.z * v2.x) - (v1.x - v2.z),
					(v1.x * v2.y) - (v1.y * v2.x)));
}
