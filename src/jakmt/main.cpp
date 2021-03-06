#include <cassert>
#include <iostream>
#include <iomanip>
#include <memory>
#include <queue>
#include <random>
#include <thread>

#include "intersect.hpp"
#include "vector3.hpp"


void
initializeParticleArrays(int numParticles, Particle particles[], Real scaledSpeed, std::random_device &rd);


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
	cout << endl;
	cout << "sizeof(Particle) = " << sizeof(Particle) << " bytes." << endl;
	cout << "sizeof(CollisionRecord) = " << sizeof(CollisionRecord) << " bytes." << endl;
	// cout << "sizeof(particleArrays[0])"
	cout << "=====================================================" << endl;
	cout << endl;


	constexpr int ONE_MILLION = 1000000;
	// constexpr int NUM_PARTICLES = 30 * ONE_MILLION;
	constexpr int NUM_PARTICLES = 30 * ONE_MILLION;

	// We want a pre-step and a post-step array.
	cout << "Allocating memory for 2 buffers of " << NUM_PARTICLES << " particles..." << endl;

	auto start = chrono::high_resolution_clock::now();

	array< unique_ptr< Particle[] >, 2 > particleArrays;
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
    initializeParticleArrays(NUM_PARTICLES, particleArrays[0].get(), SCALED_SPEED, rd);

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

			particleArrays[dst][i].position = particleArrays[src][i].position +
			        TIME_STEP * particleArrays[src][i].velocity;

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

		cout << "\nAllocating priority queue of CollisionRecord..." << endl;
		priority_queue< CollisionRecord, vector< CollisionRecord >, GreaterThanCollisionRecord > collisionQ;
		cout << "Finished allocating priority queue of CollisionRecord.\n" << endl;


		auto t_prev_collision_measurement = chrono::high_resolution_clock::now();
		double num_collision_pairs_checked = 0;
		double num_pairs_since_last_update = 0;
		size_t numCollisionRecords = 0;

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

					cout << "First collision time: " << collisionQ.top().t << endl;
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
					// @todo @perf remove dynamic allocation
					CollisionRecord cr{tIntersection, i, j};
					collisionQ.push(cr);

					numCollisionRecords++;
					if (numCollisionRecords % (size_t)1e2 == 0 &&
						numCollisionRecords > 0 )
					{
						cout << "# CollisionRecords = " << numCollisionRecords << endl;
					}
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

void
initializeParticleArrays(int numParticles, Particle particles[], Real scaledSpeed, std::random_device &rd) {
    assert(particles);

    std::mt19937 rand_gen(rd());    // Mersenne Twister
    std::uniform_real_distribution<> rand_coordinate(-0.5, 0.5);
    std::uniform_real_distribution<> rand_speed(-scaledSpeed, scaledSpeed);


    std::cout << "Initializing particle positions and velocities..." << std::endl;

    // Let's set particle initial positions on a cubic lattice so that they
    // initially are not intersecting each other. This saves us from having
    // to push apart a bunch of particles at time 0.
    // @assume This will not add artifacts to the simulation? After how
    // much time will any "memory" of being in a lattice be lost?

    const int numParticlesPerEdge = (int)ceil(cbrt(numParticles));
    const int numParticlesPerPlane = numParticlesPerEdge * numParticlesPerEdge;
    std::cout << "Num particles per lattice edge: " << numParticlesPerEdge << std::endl;

    // We want to have "numParticlesPerEdge" particles, with "separation" distance
    // between them in each coordinate. We also don't want particles right on
    // the edges of the box, so we'll have a margin on each end equal to half the
    // linear separation. So if we have E particles per edge, then there are
    // E-1 gaps between them. Adding another gap for the two margins, we should
    // divide the total box width by E.

    const Real separation = 1.0 / numParticlesPerEdge;
    const Real marginPad = 0.5f * separation;

    for (auto i = 0; i < numParticles; ++i)
    {
        const auto iPlane = i / numParticlesPerPlane;
        const auto iRow = (i % numParticlesPerPlane) / numParticlesPerEdge;
        const auto iCol = i % numParticlesPerEdge;

        // cout << iPlane << ", " << iRow << ", " << iCol << endl;

        particles[i].position[0] = -0.5 + marginPad + iCol * separation;
        particles[i].position[1] = -0.5 + marginPad + iRow * separation;
        particles[i].position[2] = -0.5 + marginPad + iPlane * separation;

        particles[i].velocity[0] = rand_speed(rand_gen);
        particles[i].velocity[1] = rand_speed(rand_gen);
        particles[i].velocity[2] = rand_speed(rand_gen);
    }
}
