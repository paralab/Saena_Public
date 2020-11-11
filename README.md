# Saena

## Dependencies
There are some dependencies which are installed automatically by running the `install.sh` file.
Some of those dependencies are `ZFP` and `SuperLU`. `SuperLU` also depends on `ParMETIS`.
The other ones that may be needed to be installed before compiling Saena are the following.

#### 1- Boost:
For `Linux` this may help:

`sudo apt-get install libboost-all-dev`

#### 2- Intel MKL:
If you don't already have `Intel MKL` on your machine, it can be installed in a couple of ways explained on its website:

`https://software.intel.com/en-us/get-started-with-mkl-for-linux`

The easiest way may be installing the standalone version:

`https://software.seek.intel.com/performance-libraries`

## Installation instructions to use Saena as a stand-alone solver

1- The CMakeLists.txt is written in a way to use Saena inside Nektar++. To use it as a stand-alone solver remove CMakeLists.txt and rename CMakeLists_standalone.txt to CMakeLists.txt.

2- run install_standalone.sh to compile the solver and its dependencies.


## Example

There is an example showing how to use Saena in src/main.cpp. Most functions are explained there.

Run this command in the build folder to run the example:

./Saena ../data/81s4x8o1mu1.bin

Pass any matrix (A) in binary format as the argument to solve the system: Ax = rhs. The matrix should be in COO format with column-major order (the same format as the Florida Matrix Collection). rhs is generated randomly in this example. It is exaplained how to also pass rhs in the example.

## Nektar++

To use Saena inside Nektar++ follow the following commands: \
1- First clone this repositoy inside Nektar++/library/MultiRegions/ \
2- run install.sh to install the solver's dependencies, before compiling Nektar++.

## Use Saena in another library

The following commands can be used to use Saena in a library:

#include "saena.hpp"

// *************************** AMG - Setup ****************************

saena::matrix A(comm); // comm: MPI_Communicator

A.add_duplicates(true); // in case of duplicates add the values. For replacing duplicates remove this line or set it to false.

A.set(I, J, V, size); // size: size of I (or J or V, they obviously should have the same size)

A.assemble();

saena::options opts; // use the default options.

// or the following command can be used, instead of the above command, to pass an xml options file.

// saena::options opts((char*)"options001.xml"); // choices for the smoorher: "jacobi", "chebyshev"

saena::amg solver;

solver.set_matrix(&A);

solver.set_rhs(rhs); // rhs should be std::vector double

// *************************** AMG - Solve ****************************

// u is the initial guess. after calling the solve function, it will be solution. it should be std::vector double

// there are two options for solving the system: 1- solve and 2- solve_pcg
  
// solver.solve(u, &opts); // AMG as a solver

solver.solve_pcg(u, &opts); // AMG as a preconditioner. The solver is preconditioned conjugate gradient (pcg).

// *************************** Destroy ****************************

A.destroy();

solver.destroy();

## Print Functions

print matrix A of type saena::matrix:

A.print_entry(cpu_rank); // print matrix entries on processor with rank "cpu_rank"

A.print_entry(-1); // print matrix entries on all processors 0, 1, 2, ..., respectively.

print a vector:

cpu_rank can be rank of a processor or -1 to print on all processors

print_vector(vector, cpu_rank, "print message", MPI communicator);

## Some Other Functions

solver.solve_pcg_update1(u, &opts, &B):

If solver is made based on a matrix, let's call it A, and there is an updated version of A, let's call it B, and one wants to solve *Bx = rhs*, instead of *Ax = rhs*, then solve_pcg_update() can be used and B can be passed as the third argument. LHS will be updated with B. For complete description check "Strategy 1" of the paper.

solver.solve_pcg_update2(u, &opts, &B):

Similar to solve_pcg_update1(), but all coarse matrices are being updated. The previous-made prolongation and restriction matrices are being kept untouched. For complete description check "Strategy 2" of the paper.

solver.solve_pcg_update3(u, &opts, &B):

Similar to solve_pcg_update2(), but only diagonal parts of all coarse matrices are being updated. For complete description check "Strategy 3" of the paper.
