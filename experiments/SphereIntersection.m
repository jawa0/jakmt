% MATLAB R2018b

intersectSweptSpheres(0.5, 0.5, [0 0 0]', [0 0 0]', [0 0 0]', [0 0 0]') % yes

intersectSweptSpheres(0.5, 0.5, [1 0 0]', [1 0 0]', [0 0 0]', [0 0 0]') % no
intersectSweptSpheres(0.5, 0.5, [1.001 0 0]', [1.001 0 0]', [0 0 0]', [0 0 0]') % no

% YES
intersectSweptSpheres(0.5, 0.5, [-1 0 0]', [0 0 0]', [1 0 0]', [0 0 0]')


