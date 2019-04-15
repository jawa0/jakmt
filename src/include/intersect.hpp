#pragma once

#include <algorithm>

#include "vector3.hpp"


bool intersectSweptSpheres(Real radiusA, Real radiusB, Vector3 posA0, Vector3 posA1, Vector3 posB0, Vector3 posB1, Real & tIntersection)
{
    auto initialDisplacementAB = posB0 - posA0;
    auto displacementA = posA1 - posA0;
    auto displacementB = posB1 - posB0;
    auto displacementAB = displacementB - displacementA;
    
    auto sumOfRadii = radiusA + radiusB;
    
    // Are the spheres initially overlapping?
    if (dot(initialDisplacementAB, initialDisplacementAB) < sumOfRadii * sumOfRadii)
    {
    	tIntersection = 0.0;
    	return true;
    }
    
	// We'll use the quadratic equation to solve for normalized time. Set
	// up the coefficients A, B, C.
    
    auto A = dot(displacementAB, displacementAB);
    auto B = 2 * dot(displacementAB, initialDisplacementAB);
    auto C = dot(initialDisplacementAB, initialDisplacementAB) - sumOfRadii * sumOfRadii;
    
    // No real roots <=> no intersection
    auto discriminant = B*B - 4*A*C;
    if (A == 0 || discriminant < 0)
    {
        tIntersection = 0.0;
        return false;
    }

    auto root0 = (-B + sqrt(discriminant)) / (2*A);
    auto root1 = (-B - sqrt(discriminant)) / (2*A);

    // @todo: figure out / test whether either root can be < 0. E.g. for both spheres
    // flying away from a point where they would have intersected, in the past.
    
    tIntersection = std::min(root0, root1);
    return true;
}
