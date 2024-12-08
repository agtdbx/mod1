#include <engine/maths/Random.hpp>

#include <stdlib.h>
#include <time.h>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Random::Random( void )
{
	srand(time(NULL));
}


Random::Random(const Random &obj)
{

}

Random::Random(unsigned int seed)
{
	this->setSeed(seed);
}

//---- Destructor --------------------------------------------------------------

Random::~Random( void )
{
}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

unsigned int	Random::getSeed(void) const
{
	return (this->seed);
}



//---- Setters -----------------------------------------------------------------

void	Random::setSeed(unsigned int seed)
{
	this->seed = seed;
	srand(this->seed);
}

//---- Operators ---------------------------------------------------------------

Random	&Random::operator=(const Random &obj)
{
	if (this == &obj)
		return (*this);

	this->setSeed(obj.seed);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

int	Random::randInt(int min, int max) const
{
	return (rand() % (max - min) + min);
}


double	Random::random(void) const
{
	return ((double)rand() / RAND_MAX);
}


double	Random::randDouble(double min, double max) const
{
	return (min + this->random() * (max - min));
}
