#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>


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


using ParticleArrayPtr = std::unique_ptr< Particle[] >;
auto g_particleArrays = std::make_unique< ParticleArrayPtr[] >(2);


void stepParticles(int iFirst, int iLast)
{
	using namespace std;

	// cout << iFirst << " " << iLast << endl;
	// cout << g_particleArrays.get() << endl;
	for (int i = iFirst; i <= iLast; ++i)
	{
		g_particleArrays[1][i].position[0] = g_particleArrays[0][i].position[0] + 0.4242;
	}
}


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

	// We want a pre-step and a post-step array.
	cout << "Allocating memory for 2 buffers of " << NUM_PARTICLES << " particles..." << endl;

	auto start = chrono::high_resolution_clock::now();

	g_particleArrays[0] = make_unique< Particle[] >(NUM_PARTICLES);
	g_particleArrays[1] = make_unique< Particle[] >(NUM_PARTICLES);
	
	std::chrono::duration< double > elapsedSeconds = chrono::high_resolution_clock::now() - start;

	cout << "Finished allocating memory." << endl;
	cout << "Allocation took " << elapsedSeconds.count() << " seconds." << endl;
	cout << endl;

	for (auto s = 0; s < 3; ++s)
	{
		// Fake a time-step
		cout << "Stepping particles..." << endl;
		
		constexpr int NUM_THREADS = 4;
		assert( NUM_PARTICLES >= NUM_THREADS );
		int particlesPerThread = NUM_PARTICLES / NUM_THREADS;

		auto iFirstParticle = 0;
		auto threads = make_unique< unique_ptr< thread >[] >(NUM_THREADS);

		start = chrono::high_resolution_clock::now();	
		for (int iThread = 0; iThread < NUM_THREADS; ++iThread)
		{
			auto iLastParticle = min( iFirstParticle + particlesPerThread-1, NUM_PARTICLES-1 );
			// cout << iThread << " : " << iFirstParticle << " -> " << iLastParticle << endl;
			
			threads[iThread] = make_unique< thread >(thread(stepParticles, iFirstParticle, iLastParticle));

			iFirstParticle += particlesPerThread;
		}

		for (int iThread = 0; iThread < NUM_THREADS; ++iThread)
		{
			threads[iThread]->join();
		}

		elapsedSeconds = chrono::high_resolution_clock::now() - start;
		cout << "Particle step took " << elapsedSeconds.count() << " seconds." << endl;

		// Output some results so that the step isn't optimized away.
		for (int i = 0; i < 2; ++i)
		{
			cout << g_particleArrays[1][i].position[0] << endl;
		}	
	}
	return 0;
}
