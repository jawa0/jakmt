jakmt - Ideal Gas Simulation using Kinetic Molecular Theory (KMT)
===

Overview
-

* Simulate a gas by simulating the individual molecules.
* KMT Assumptions
    * The molecules are modeled as spheres that bounce off each other, and off the walls of the container, undergoing fully elastic collisions.
    * Other than collisions, the molecules have no other interactions with each other, or the walls of the container. For instance, no electronic attraction or repulsion.

Why not use a continuous media fluid-simulation approach, like they do in computer graphics? What about Navier-Stokes? Why simulate individual molecules?

* I want the fluid to be compressible
* I want to be able to simulate shock-waves by e.g. sending a sphere at high speed through the gas.

Challenges
-
There's lots of molecules. LOTS.

There are Na = 6.02*10^23 molecules of gas in a mole. A mole is like a dozen, but for molecules. It's just a unit of counting. That's a 6 followed by 23 zeroes: 600,000,000,000,000,000,000,000. At Standard Temperature and Pressure (STP), which is 0 degrees C and 1 bar of pressure, one mole of ideal gas has a volume of 22.7L.

How many molecules are there in one cubic millimetre?
1 mm^3 = 10^-9 m^3
22.7 L/mol ~= 0.02 m^3/mol <=> 50 mol/m^3

__1mm^3 contains 3*10^16 molecules! That's 30,000,000,000,000,000__

So, in 2019 that's a bit beyond my ability to simulate. How about a cubic micro-metre?

1 cubic micron contains 3E7 molecules

That's 30 million. __Okay, so let's try to simulate 30 million molecules bouncing around. Go!__
