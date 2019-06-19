#include <iostream>
#include <iomanip>
#include <memory>
#include <queue>
#include <random>
#include <thread>

#include "intersect.hpp"
#include "vector3.hpp"



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
	// constexpr int NUM_PARTICLES = 30 * ONE_MILLION;
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
	cout << "Simulation time step: " << 1 / NS_PER_S << " s" << endl;

	constexpr auto BOX_SIDE = 1e-6;	// m
	
	// Kinetic diameter is a measure applied to atoms and molecules that 
	// expresses the likelihood that a molecule in a gas will collide with 
	// another molecule. https://en.wikipedia.org/wiki/Kinetic_diameter
	// Nitrogen gas N_2 kinetic diameter is 364 pm

	constexpr auto PARTICLE_RADIUS_M = 0.5 * 0.364e-9;

	cout << "Simulation box side: " << BOX_SIDE << " m" << endl;
	cout << "Molecule kinetic diameter: " << 2 * PARTICLE_RADIUS_M << " m" << endl;

	constexpr auto PARTICLE_RADIUS = PARTICLE_RADIUS_M / BOX_SIDE;
	cout << "\t=> " << PARTICLE_RADIUS << " box widths" << endl;

	constexpr auto INITIAL_SPEED_COORDINATE_MPS = 500;	// m/s
	constexpr auto  SCALED_SPEED = INITIAL_SPEED_COORDINATE_MPS / BOX_SIDE / NS_PER_S;
	cout << "Initial speed coordinate: " << INITIAL_SPEED_COORDINATE_MPS << " m/s" << endl;
	cout << "\t=> " << SCALED_SPEED << " box widths / step" << endl;

	random_device rd;
	mt19937 rand_gen(rd());	// Mersenne Twister
	uniform_real_distribution<> rand_coordinate(-0.5, 0.5);
	uniform_real_distribution<> rand_speed(-SCALED_SPEED, SCALED_SPEED);


	cout << "Initializing particle positions and velocities..." << endl;

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

	for (auto t = 0; t < 1; ++t)
	{
		// Fake a time-step

		cout << "Stepping particles..." << endl;
		start = chrono::high_resolution_clock::now();	

		// 1) Move all the particles...
		for (int i = 0; i < NUM_PARTICLES; ++i)
		{
			// cout << i << "..." << endl;
			// cout << collisionQ.size() << endl;

			particleArrays[dst][i].position = particleArrays[src][i].position + TIME_STEP * particleArrays[src][i].velocity;
			particleArrays[dst][i].velocity = particleArrays[src][i].velocity;
		}
		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Particle step took " << elapsedSeconds.count() << " seconds." << endl;


		// 2) Test each pair of particles for collision...
		cout << "Finding collisions..." << endl;
		start = chrono::high_resolution_clock::now();	

		ParticleCollider collider(PARTICLE_RADIUS + PARTICLE_RADIUS);
		collider.setParticleArrays(NUM_PARTICLES, particleArrays[src].get(), particleArrays[dst].get());

		//
		// Ooookay. Let's do some N^2 collision detection :/
		//

		constexpr double NUM_COLLISION_PAIRS = (NUM_PARTICLES * ((double)NUM_PARTICLES - 1)) / 2.0;
		priority_queue< CollisionRecord, vector< CollisionRecord >, GreaterThanCollisionRecord > collisionQ;


		auto t_prev_collision_measurement = chrono::high_resolution_clock::now();
		double num_collision_pairs_checked = 0;
		double num_pairs_since_last_update = 0;

		for (int i = 0; i < NUM_PARTICLES; ++i)
		{
			if (i % 100 == 0)
			{
				auto elapsed_collision_time = chrono::high_resolution_clock::now() - t_prev_collision_measurement;

				cout << i << "..." << endl;
				cout << "Checked " << num_collision_pairs_checked << " out of " << NUM_COLLISION_PAIRS << " pairs." << endl;

				{
					ios oldState(nullptr);
					oldState.copyfmt(cout);

					auto percentDone = 100.0 * num_collision_pairs_checked / NUM_COLLISION_PAIRS;
					cout << "Percent done: " << fixed << setw(8) << setprecision(4) << percentDone << " %" << endl;

					cout.copyfmt(oldState);
				}

				double pairs_per_second;
				if (num_pairs_since_last_update > 0)
				{
					auto elapsed_collision_seconds = 
						chrono::duration_cast< chrono::seconds >(elapsed_collision_time).count();
						
					cout << "Pairs processed since last update: " << num_pairs_since_last_update << endl;
					cout << "Took " << elapsed_collision_seconds << " seconds." << endl;

					pairs_per_second = num_pairs_since_last_update / elapsed_collision_seconds;

				
					cout << "Collision pairs processed per second: " << pairs_per_second << endl;

					auto num_collision_pairs_remaining = NUM_COLLISION_PAIRS - num_collision_pairs_checked;
					auto seconds_remaining = num_collision_pairs_remaining / pairs_per_second;

					constexpr auto SECONDS_PER_MINUTE = 60;
					constexpr auto SECONDS_PER_HOUR = SECONDS_PER_MINUTE * SECONDS_PER_MINUTE;

					cout << "Remaining time: ";

					if (seconds_remaining > SECONDS_PER_HOUR)
					{
						cout << seconds_remaining / SECONDS_PER_HOUR << " hours." << endl;
					}
					else if (seconds_remaining > SECONDS_PER_MINUTE)
					{
						cout << seconds_remaining / SECONDS_PER_MINUTE << " minutes." << endl;
					}
					else
					{
						cout << seconds_remaining << " seconds." << endl;
					}
				}


				t_prev_collision_measurement = chrono::high_resolution_clock::now();
				num_pairs_since_last_update = 0;
			}

			for (auto j = i + 1; j < NUM_PARTICLES; j++)
			{
				Real tIntersection;
				bool didIntersect = collider.intersectSweptSpheres(i, j, tIntersection);
				if (didIntersect)
				{
					CollisionRecord cr{tIntersection, i, j};
					collisionQ.push(cr);
				}

				num_collision_pairs_checked++;
				num_pairs_since_last_update++;
			}
		}
		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Colllision detection took " << elapsedSeconds.count() << " seconds." << endl;

		// Output some results so that the step isn't optimized away.
		for (int i = 0; i < 1; ++i)
		{
			cout << particleArrays[dst][i].position << endl;
		}
		cout << collisionQ.top().i << " collided with " << collisionQ.top().j << endl;

		// Swap source and destination arrays...
		src = (src + 1) % 2;
		dst = (dst + 1) % 2;
	}
	return 0;
}
