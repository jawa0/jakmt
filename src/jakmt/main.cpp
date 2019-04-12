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

	// Initialize particle positions and velocities...

	// We're going to measure time in nanoseconds. So one time unit will be one ns.
	// We're going to measure position in terms of the size of the simulation box. One box width
	// will be 1 length unit.
	// We're going to measure speed in box widths / nanosecond. So a speed of 1 means that
	// the particle goes BOX_SIDE meters per 1e-9 seconds.

	constexpr auto  NS_PER_S = 1e9;	// nanoseconds (ns) per second (s)
	constexpr auto  TIME_STEP = 1;	// nanoseconds (ns)
	cout << "dt (s) = " << 1 / NS_PER_S << endl;

	constexpr auto BOX_SIDE = 1e-6;	// m
	cout << "Simulation box side (m): " << BOX_SIDE << endl;

	constexpr auto INITIAL_SPEED_COORDINATE_MPS = 500;	// m/s
	constexpr auto  SCALED_SPEED = INITIAL_SPEED_COORDINATE_MPS / BOX_SIDE / NS_PER_S;
	cout << "Initial speed coordinate (m/s): " << INITIAL_SPEED_COORDINATE_MPS << endl;
	cout << "Initial speed coordinate (box width / step): " << SCALED_SPEED << endl;

	random_device rd;
	mt19937 rand_gen(rd());	// Mersenne Twister
	uniform_real_distribution<> rand_coordinate(-0.5, 0.5);
	uniform_real_distribution<> rand_speed(-SCALED_SPEED, SCALED_SPEED);	// m/s


	for (auto i = 0; i < NUM_PARTICLES; ++i)
	{
		particleArrays[0][i].position[0] = rand_coordinate(rand_gen);
		particleArrays[0][i].position[1] = rand_coordinate(rand_gen);
		particleArrays[0][i].position[2] = rand_coordinate(rand_gen);

		particleArrays[0][i].velocity[0] = rand_speed(rand_gen);
		particleArrays[0][i].velocity[1] = rand_speed(rand_gen);
		particleArrays[0][i].velocity[2] = rand_speed(rand_gen);		
	}

	cout << particleArrays[0][0].position << endl;	

	auto src = 0;
	auto dst = (src + 1) % 2;

	for (auto t = 0; t < 4; ++t)
	{
		// Fake a time-step
		cout << "Stepping particles..." << endl;
		
		start = chrono::high_resolution_clock::now();	
		for (int i = 0; i < NUM_PARTICLES; ++i)
		{
			particleArrays[dst][i].position = particleArrays[src][i].position + TIME_STEP * particleArrays[src][i].velocity;
			particleArrays[dst][i].velocity = particleArrays[src][i].velocity;
		}
		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Particle step took " << elapsedSeconds.count() << " seconds." << endl;

		// Output some results so that the step isn't optimized away.
		for (int i = 0; i < 1; ++i)
		{
			cout << particleArrays[dst][i].position << endl;
		}

		// Swap source and destination arrays...
		src = (src + 1) % 2;
		dst = (dst + 1) % 2;
		assert(src != dst);
	}
	return 0;
}
