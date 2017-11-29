# Saena_Public
Instructions:

1- extract external.tar.gz. to the same directory. There will be a folder called external, which includes three external libraries.

2- mkdir build and cd into build

3- cmake ..

Note: Elemental is an external library inside Saena and will be installed inside system folders. To avoid that, the following argument can be passed to the cmake command to specify the installation path:

cmake -D CMAKE_INSTALL_PREFIX="./elemental_install" ..

In the above example, Elemental will be installed inside a directory called "elemental_install" inside the build directory.

4- make

--------------------------------------------------------------------------------------------------

To use Saena inside a library the following commands can be used:

#include "saena.hpp"

// *************************** AMG - Setup ****************************

saena::matrix A(comm); // comm: MPI_Communicator

A.add_duplicates(true); // in case of duplicates add the values. For removing duplicates remove this line.

A.set(I, J, V, size); // size: size of I (or J or V, they obviously should have the same size)

A.assemble();

saena::options opts; // use the default options.

// or the following command can be used, instead of the above command, to pass an xml options file.

// saena::options opts((char*)"options001.xml"); // choices for the smoorher: "jacobi", "chebyshev"

saena::amg solver;

// solver.set_multigrid_max_level(0); // 0 means only use direct solver, so no multigrid will be used. otherwise remove this line.

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

--------------------------------------------------------------------------------------------------

Some other documentation:

solver.solve_pcg_update(u, &opts, &B)
if solver is made based of a matrix, let's call it A, and there is an updated version of A, let's call it B, and one wants to solve B*x = rhs, instead of A*x = rhs, then solve_pcg_update() can be used and B can be passed as the third argument.
