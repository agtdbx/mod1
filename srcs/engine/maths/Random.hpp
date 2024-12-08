#ifndef RANDOM_HPP
# define RANDOM_HPP

class Random
{
public:
	// Constructors / destructors
	Random(void);
	Random(const Random &obj);
	Random(unsigned int seed);
	~Random();

	// Getters
	unsigned int	getSeed(void) const;

	// Setters
	void	setSeed(unsigned int seed);

	// Operator
	Random	&operator=(const Random &obj);

	// Random methods
	int		randInt(int min, int max) const;
	double	random(void) const;
	double	randDouble(double min, double max) const;

private:
	unsigned int seed;
};

#endif
