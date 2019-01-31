% MATLAB R2018b
% Moving sphere-sphere intersection test
% Adapted from Gamasutra article "Simple Intersection for Games"
% http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php
% Returns a 2-vector. The first element is 1 if the spheres intersect, and
% 0 otherwise. The second element is only valid if the spheres DO
% intersect, and is the normalized time of intersection. The time is
% normalized in the sense that it is in the closed interval [0.0, 1.0]. At
% time 0, the spheres are at their initial positions, and at time 1, the
% spheres are at their final positions.

function intersectedWhen = intersectSweptSpheres( ...
                            radiusA, radiusB, ...
                            initialPositionA, finalPositionA, ...
                            initialPositionB, finalPositionB)
    
                 
    initialDisplacementAB = initialPositionB - initialPositionA;
    displacementA = finalPositionA - initialPositionA;
    displacementB = finalPositionB - initialPositionB;
    displacementAB = displacementB - displacementA; % B displacement in frame of reference where A is stationary
    
    sumOfRadii = radiusA + radiusB;
    
    % Are the spheres initially overlapping?
    if (dot(initialDisplacementAB, initialDisplacementAB) < ...
            dot(sumOfRadii, sumOfRadii))
   
        intersectedWhen = [1 0.0];
        return;
    end
    
    % We'll use the quadratic equation to solve for normalized time. Set
    % up the coefficients A, B, C.
    
    A = dot(displacementAB, displacementAB);
    B = 2 * dot(displacementAB, initialDisplacementAB);
    C = dot(initialDisplacementAB, initialDisplacementAB) - ...
            sumOfRadii * sumOfRadii;
    
    % No real roots <=> no intersection
    discriminant = B^2 - 4*A*C;
    if (A == 0 || discriminant < 0)
        intersectedWhen = [0 0.0];
        return
    end
    
    intersectionTimes = roots([A B C]);
    intersectedWhen = [1 min(intersectionTimes)];
end
