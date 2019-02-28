#include <iostream>
#include <memory>


class Vector3
{
public:
	double e[3];
};


int main()
{
	using namespace std;

	const int NUM_PARTICLES = 35000000;

	using Vector3ArrayPtr = unique_ptr< Vector3[] >;

	// We want a pre-step and a post-step array.
	auto particleVectors = make_unique< Vector3ArrayPtr[] >(2);
	particleVectors[0] = make_unique< Vector3[] >(NUM_PARTICLES);
	particleVectors[1] = make_unique< Vector3[] >(NUM_PARTICLES);
	
	// Fake a time-step
	for (int i = 0; i < NUM_PARTICLES; ++i)
	{
		particleVectors[1][i].e[0] = particleVectors[0][i].e[0] + 0.4242;
	}

	// Output some results so that the step isn't optimized away.
	for (int i = 0; i < 10; ++i)
	{
		cout << particleVectors[1][i].e[0] << endl;
	}
	return 0;
}
