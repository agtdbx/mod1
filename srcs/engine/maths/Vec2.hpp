#ifndef VEC2_HPP
# define VEC2_HPP

# include <iostream>

class Vec2
{
public:
	// Properties
	double	x;
	double	y;

	// Constructors / destructors
	Vec2(void);
	Vec2(const double x, const double y);
	Vec2(const Vec2 &vec2);
	~Vec2();

	// Getters
	bool	isNormalize(void) const;
	double	getNorm(void) const;

	// Method that modify vector
	void	normalize(void);
	void	translate(const Vec2 &dir, double length);
	void	rotate(const double degrees);
	void	rotateRadian(const double radian);
	void	rotateArround(const Vec2 &arrond_vec, const double degrees);
	void	rotateArroundRadian(const Vec2 &arrond_vec, const double radian);

	// Method that return values
	double	dot(const Vec2 &vec2) const;
	double	cross(const Vec2 &vec2) const;
	double	distanceWith(const Vec2 &vec2) const;

	// Modify vector vector operator
	Vec2	&operator=(const Vec2 &vec2);
	Vec2	&operator+=(const Vec2 &vec2);
	Vec2	&operator-=(const Vec2 &vec2);

	// Modify vector number operator
	Vec2	&operator*=(const double number);
	Vec2	&operator/=(const double number);

	// Comparaison operator
	bool	operator==(const Vec2 &vec2) const;
	bool	operator!=(const Vec2 &vec2) const;
};

// Print operator
std::ostream&	operator<<(std::ostream &os, const Vec2 &vec2);

// Vector vector operator
inline Vec2		operator+(const Vec2 &v1, const Vec2 &v2);
inline Vec2		operator-(const Vec2 &v1, const Vec2 &v2);

// Vector number operator
inline Vec2		operator*(const Vec2 &vec2, const double number);
inline Vec2		operator*(const double number, const Vec2 &vec2);
inline Vec2		operator/(const Vec2 &vec2, const double number);

// Vector vector functions
inline double	vec2Dot(const Vec2 &v1, const Vec2 &v2);
inline double	vec2Cross(const Vec2 &v1, const Vec2 &v2);

// Define Point2 as Vec2 for code clarity
using Point2 = Vec2;

#endif
