#ifndef VEC3_HPP
# define VEC3_HPP

# include <iostream>

class Vec3
{
public:
	// Properties
	double	x;
	double	y;
	double	z;

	// Constructors / destructors
	Vec3(void);
	Vec3(const double x, const double y, const double z);
	Vec3(const Vec3 &vec3);
	~Vec3();

	// Getters
	bool	isNormalize(void) const;
	double	getNorm(void) const;

	// Method that modify vector
	void	normalize(void);
	void	translate(const Vec3 &dir, const double length);
	void	rotateX(const double degrees);
	void	rotateXRadian(const double radian);
	void	rotateY(const double degrees);
	void	rotateYRadian(const double radian);
	void	rotateZ(const double degrees);
	void	rotateZRadian(const double radian);
	void	rotateArround(const Vec3 &vec3, const double degrees);
	void	rotateArroundRadian(const Vec3 &vec3, const double radian);

	// Method that return values
	double	dot(const Vec3 &vec3) const;
	Vec3	cross(const Vec3 &vec3) const;
	double	distanceWith(const Vec3 &vec3) const;

	// Modify vector vector operator
	Vec3	&operator=(const Vec3 &vec3);
	Vec3	&operator+=(const Vec3 &vec3);
	Vec3	&operator-=(const Vec3 &vec3);

	// Modify vector number operator
	Vec3	&operator*=(const double number);
	Vec3	&operator/=(const double number);

	// Comparaison operator
	bool	operator==(const Vec3 &vec3) const;
	bool	operator!=(const Vec3 &vec3) const;
};

// Print operator
std::ostream&	operator<<(std::ostream& os, const Vec3 &vec3);

// Vector vector operator
inline Vec3		operator+(const Vec3 &v1, const Vec3 &v2);
inline Vec3		operator-(const Vec3 &v1, const Vec3 &v2);

// Vector number operator
inline Vec3		operator*(const Vec3 &vec3, const double number);
inline Vec3		operator*(const double number, const Vec3 &vec3);
inline Vec3		operator/(const Vec3 &vec3, const double number);

// Vector vector functions
inline double	vec3Dot(const Vec3 &v1, const Vec3 &v2);
inline Vec3		vec3Cross(const Vec3 &v1, const Vec3 &v2);

// Define Point3 as Vec3 for code clarity
using Point3 = Vec3;

#endif
