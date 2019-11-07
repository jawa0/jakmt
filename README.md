jakmt - Ideal Gas Simulation using Kinetic Molecular Theory (KMT)
===

Overview
-

* Simulate a gas by simulating the individual molecules as idealized balls bouncing off each other.
* Kinetic Molecular Theory (KMT) Assumptions
    * The molecules are modeled as spheres that bounce off each other, and off the walls of the container, undergoing fully elastic collisions.
    * Other than collisions, the molecules have no other interactions with each other, or the walls of the container. For instance, no electronic attraction or repulsion.

Why not use a continuous media fluid-simulation approach, like they do in computer graphics? What about Navier-Stokes? Why simulate individual molecules?

* I want the fluid to be compressible
	* I want to be able to simulate shock-waves by e.g. sending a sphere at high speed through the gas.
* I'm not mathed-up enough yet to implement Navier-Stokes. See: Bridson. Fluid Simulation for Computer Graphics.

See [Wikipedia's page on the Kinetic Theory of Gases](https://en.wikipedia.org/wiki/Kinetic_theory_of_gases). Or [this summary of kinetic theory on HyperPhysics](http://hyperphysics.phy-astr.gsu.edu/hbase/Kinetic/kinthe.html).

Building / Running
-
To build jakmt, you'll need cmake. Running cmake generates the Makefile for your system, which you then build.

Assuming that you're in the jakmt repository directory:

	mkdir build
	cd build
	cmake ..
	make
	./jakmt

Challenges
-
There's lots of molecules. LOTS.

There are Na = 6.02E23 molecules of gas in a mole. A mole is like a dozen, but for molecules. It's just a unit of counting. That's a 6 followed by 23 zeroes: 600,000,000,000,000,000,000,000. At Standard Temperature and Pressure (STP), which is 0 degrees C and 1 bar of pressure, one mole of ideal gas has a volume of 22.7L.

How many molecules are there in one cubic millimetre?
1 mm^3 = 10^-9 m^3
22.7 L/mol ~= 0.02 m^3/mol <=> 50 mol/m^3

__1mm^3 contains 3E16 molecules! That's 30,000,000,000,000,000__

So, in 2019 that's a bit beyond my ability to simulate. How about a cubic micro-metre?

1 cubic micron contains 3E7 molecules

That's 30 million. __Okay, so let's try to simulate 30 million molecules bouncing around. Go!__

Current Performance
-
Doing O(n^2) collision detection on 30M particles will take about 34 days on my 2017 laptop. :/


TODO
-
* Implement a collision query acceleration data structure, so we're not doing O(n^2) collision detection. I'm going to try Thatcher Ulrich's Loose Octrees.
* Multithreading
* GPU acceleration
* Distribute across multiple machines

Current Performance
--
My 2017 Mac Laptop
-
	=====================================================
	jaKMT - Kinetic Molecular Theory Ideal Gas Simulation
	=====================================================
	Number of hardware threads supported: 4
	  NOTE: Currently only using one thread.

	C++ high-resolution clock period: 1 / 1000000000
	=====================================================

	Allocating memory for 2 buffers of 30000000 particles...
	Finished allocating memory.
	Allocation took 1.98674 seconds.

	Simulation time step: 1e-09 s
	Simulation box side: 1e-06 m
	Molecule kinetic diameter: 3.64e-10 m
		=> 0.000182 box widths
	Initial speed coordinate: 500 m/s
		=> 0.5 box widths / step
	Initializing particle positions and velocities...
	Num particles per lattice edge: 311
	(-0.498392, -0.498392, -0.498392)
	Stepping particles...
	Particle step took 1.63687 seconds.
	Finding collisions...
	0...
	Checked 0 out of 4.5e+14 pairs.
	Percent done:   0.0000 %
	100...
	Checked 2.99999e+09 out of 4.5e+14 pairs.
	Percent done:   0.0007 %
	Pairs processed since last update: 2.99999e+09
	Took 19 seconds.
	Collision pairs processed per second: 1.57894e+08
	Remaining time: 791.663 hours.
	First collision time: 0.00358602
	200...
	Checked 5.99998e+09 out of 4.5e+14 pairs.
	Percent done:   0.0013 %
	Pairs processed since last update: 2.99998e+09
	Took 19 seconds.
	Collision pairs processed per second: 1.57894e+08
	Remaining time: 791.66 hours.
	First collision time: 0.00358602
	300...
	Checked 8.99995e+09 out of 4.5e+14 pairs.
	Percent done:   0.0020 %
	Pairs processed since last update: 2.99997e+09
	Took 18 seconds.
	Collision pairs processed per second: 1.66665e+08
	Remaining time: 749.991 hours.
	First collision time: 0.00358602
	400...
	Checked 1.19999e+10 out of 4.5e+14 pairs.
	Percent done:   0.0027 %
	Pairs processed since last update: 2.99996e+09
	Took 20 seconds.
	Collision pairs processed per second: 1.49998e+08
	Remaining time: 833.321 hours.
	First collision time: 0.00358602
	500...
	Checked 1.49999e+10 out of 4.5e+14 pairs.
	Percent done:   0.0033 %
	Pairs processed since last update: 2.99995e+09
	Took 18 seconds.
	Collision pairs processed per second: 1.66664e+08
	Remaining time: 749.986 hours.
	First collision time: 0.00326668


