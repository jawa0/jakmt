#include <array>
#include <iostream>
#include <memory>
#include <thread>


class Vector3
{
	std::array< double, 3 > e;	// elements

public:
	double operator[](int i) const
	{
		return e[i];
	}

	double & operator[](int i)
	{
		return e[i];
	}
};


class Particle
{
public:
	Vector3 position;
	Vector3 velocity;
};


int main()
{
	using namespace std;

	cout << "=====================================================" << endl;
	cout << "jaKMT - Kinetic Molecular Theory Ideal Gas Simulation" << endl;
	cout << "=====================================================" << endl;
	cout << "Number of hardware threads supported: " << thread::hardware_concurrency() << endl;
	cout << "  NOTE: Currently only using one thread." << endl;
	cout << endl;
	cout << "C++ high-resolution clock period: " << chrono::high_resolution_clock::period::num <<
		" / " << chrono::high_resolution_clock::period::den << endl;
	cout << "=====================================================" << endl;
	cout << endl;


	constexpr int ONE_MILLION = 1000000;
	constexpr int NUM_PARTICLES = 30 * ONE_MILLION;

	using ParticleArrayPtr = unique_ptr< Particle[] >;

	// We want a pre-step and a post-step array.
	cout << "Allocating memory for 2 buffers of " << NUM_PARTICLES << " particles..." << endl;

	auto start = chrono::high_resolution_clock::now();

	auto particleArrays = make_unique< ParticleArrayPtr[] >(2);
	particleArrays[0] = make_unique< Particle[] >(NUM_PARTICLES);
	particleArrays[1] = make_unique< Particle[] >(NUM_PARTICLES);
	
	std::chrono::duration< double > elapsedSeconds = chrono::high_resolution_clock::now() - start;

	cout << "Finished allocating memory." << endl;
	cout << "Allocation took " << elapsedSeconds.count() << " seconds." << endl;
	cout << endl;

	for (auto t = 0; t < 3; ++t)
	{
		// Fake a time-step
		cout << "Stepping particles..." << endl;
		
		start = chrono::high_resolution_clock::now();	
		for (int i = 0; i < NUM_PARTICLES; ++i)
		{
			particleArrays[1][i].position[0] = particleArrays[0][i].position[0] + 0.4242;
		}
		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Particle step took " << elapsedSeconds.count() << " seconds." << endl;

		// Output some results so that the step isn't optimized away.
		for (int i = 0; i < 2; ++i)
		{
			cout << particleArrays[1][i].position[0] << endl;
		}	
	}
	return 0;
}
