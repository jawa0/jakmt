#pragma once

#include <algorithm>

#include "vector3.hpp"

class Particle
{
public:
	Vector3 position;
	Vector3 velocity;
};


class CollisionRecord
{
public:
	Real t;
	int i;
	int j;
};


class GreaterThanCollisionRecord
{
public:
	// @todo @perf Pass by reference? By value? What should I do?
	inline bool operator() (const CollisionRecord& cr1, const CollisionRecord& cr2)
	{
		return cr1.t > cr2.t;
	}
};


class ParticleCollider
{
private:
	Real _sumOfRadii;
	Real _sumOfRadiiSquared;
	int _N;

	const Particle * _particles0;
	const Particle * _particles1;
	Vector3 * _displacements;


public:
	ParticleCollider(Real sumOfRadii)
		: _sumOfRadii(sumOfRadii), _N(0), _particles0(nullptr), _particles1(nullptr), _displacements(nullptr)
	{
		_sumOfRadiiSquared = _sumOfRadii * _sumOfRadii;
	}

	~ParticleCollider()
	{
		if (_displacements)
		{
			delete[] _displacements;
		}
	}

	void setParticleArrays(int N, const Particle * const positions0, const Particle * const positions1)
	{
		_particles0 = positions0;
		_particles1 = positions1;
		if (_displacements && N > _N)
		{
			delete[] _displacements;
			_displacements = nullptr;
		}
		
		if (!_displacements)
		{
			_displacements = new Vector3[N];
			_N = N;
		}

		for (int i = 0; i < _N; ++i)
		{
			_displacements[i] = _particles1[i].position - _particles0[i].position;
		}
	}

	bool intersectSweptSpheres(int i, int j, Real& tIntersection)
	{
		// @note @perf Adding this check seemed like a good idea, but made this function
		// take almost TWICE as long! #AlwaysProfile

		// // We @assume that the spheres are not initially intersecting. If that's true
		// // AND they're moving away from each other, then they can't possibly intersect.

		// if (dot(_displacements[i], _displacements[j]) < 0.0)
		// {
		// 	return false;
		// }

		const auto initialDisplacementAB = _particles0[j].position - _particles0[i].position;
		const auto displacementAB = _displacements[j] - _displacements[i];

		// Are the spheres initially overlapping?
		if (dot(initialDisplacementAB, initialDisplacementAB) < _sumOfRadiiSquared)
		{
			tIntersection = 0.0;
			return true;
		}

		// We'll use the quadratic equation to solve for normalized time. Set
		// up the coefficients A, B, C.

		const auto A = dot(displacementAB, displacementAB);
		const auto B = 2 * dot(displacementAB, initialDisplacementAB);
		const auto C = dot(initialDisplacementAB, initialDisplacementAB) - _sumOfRadiiSquared;

		// No real roots <=> no intersection
		const auto discriminant = B * B - 4 * A * C;
		if (A == 0 || discriminant < 0)
		{
			tIntersection = 0.0;
			return false;
		}

		const auto root0 = (-B + sqrt(discriminant)) / (2 * A);
		const auto root1 = (-B - sqrt(discriminant)) / (2 * A);

		// @todo: figure out / test whether either root can be < 0. E.g. for both spheres
		// flying away from a point where they would have intersected, in the past.

		tIntersection = std::min(root0, root1);
		return true;
	}
};
