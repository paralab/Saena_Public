# Saena_Public

The main.cpp file in src folder is an example how to use Saena as a stand-alone solver. It reads the matrix and rhs from file. This is one example how to execute it:

mpirun -np 4 ./Saena ../data/3DMed_sorted.bin ../data/v2089.bin


To use Saena inside a library the following commands can be used:

#include "saena.hpp"

// *************************** AMG - Setup ****************************

saena::matrix A(comm); // comm: MPI_Communicator

A.add_duplicates(true); // in case of duplicates add the values. For removing duplicates ignore this line.

A.set(I, J, V, size); // size: size of I (or J or V, they obviously should have the same size)

A.assemble();

saena::options opts; // use the default options.

// or the following command can be used, instead of the above command, to pass an xml options file.

// saena::options opts((char*)"options001.xml");

saena::amg solver;

// solver.set_multigrid_max_level(0); // 0 means only use direct solver, so no multigrid will be used. otherwise ignore this line.

solver.set_matrix(&A);

solver.set_rhs(rhs); // rhs should be std::vector double

// *************************** AMG - Solve ****************************

// u is the initial guess. after calling the solve function, it will be solution. it should be std::vector double
// there are two options for solving the system: 1- solve and 2- solve_pcg
  
// solver.solve(u, &opts); // AMG as a solver

solver.solve_pcg(u, &opts); // AMG as a preconditioner

// *************************** Destroy ****************************

A.destroy();

solver.destroy();
