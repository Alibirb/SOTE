#ifndef RANDOMGENERATOR_H_
#define RANDOMGENERATOR_H_

class RandomGenerator
{
public:
	static void seed()
	{
		srand(time(0));
	}

	static int nextInt(int maxValue)
	{
		return rand() % maxValue;
	}

	static double nextDouble()
	{
		return (double) rand() / (double)RAND_MAX;
	}
};

#endif // RANDOMGENERATOR_H_
