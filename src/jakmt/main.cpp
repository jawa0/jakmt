#include <iostream>
#include <memory>
#include <random>
#include <thread>

#include "vector3.hpp"


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

	auto TIME_STEP = 1e-9;	// seconds
	cout << "dt = " << TIME_STEP << endl;

	// Initialize particle positions...
	random_device rd;
	mt19937 rand_gen(rd());	// Mersenne Twister
	uniform_real_distribution<> rand_coordinate(-0.5, 0.5);

	for (auto i = 0; i < NUM_PARTICLES; ++i)
	{
		particleArrays[0][i].position[0] = rand_coordinate(rand_gen);
		particleArrays[0][i].position[1] = rand_coordinate(rand_gen);
		particleArrays[0][i].position[2] = rand_coordinate(rand_gen);
	}

	cout << particleArrays[0][0].position << endl;	

	for (auto t = 0; t < 2; ++t)
	{
		// Fake a time-step
		cout << "Stepping particles..." << endl;
		
		start = chrono::high_resolution_clock::now();	
		for (int i = 0; i < NUM_PARTICLES; ++i)
		{
			particleArrays[1][i].position = particleArrays[0][i].position + TIME_STEP * particleArrays[0][i].velocity;
		}
		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Particle step took " << elapsedSeconds.count() << " seconds." << endl;

		// Output some results so that the step isn't optimized away.
		for (int i = 0; i < 2; ++i)
		{
			cout << particleArrays[1][i].position << endl;
		}	
	}
	return 0;
}
