# Saena_Public

The main.cpp file in src folder is an example how to use Saena as a stand-alone solver. It reads the matrix and rhs from file. This is one example how to execute it:
mpirun -np 4 ./Saena ../data/3DMed_sorted.bin ../data/v2089.bin


To use Saena inside a library the following commands can be used:

#include "saena.hpp"

// *************************** AMG - Setup ****************************

saena::matrix A(comm); // comm: MPI_Communicator
A.set(I, J, V, size); // size: size of I (or J or V, they obviously should have the same size)
A.assemble();

// saena::options opts((char*)"options001.xml"); // pass the name of the options file
saena::options opts; // use the default options.

saena::amg solver(&A);

// *************************** AMG - Solve ****************************

// u is the initial guess. after calling the solve function, it will be solution.

solver.solve(u, rhs, &opts); 

// *************************** Destroy ****************************

A.destroy();
solver.destroy();
