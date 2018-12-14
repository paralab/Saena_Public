# Saena

## Installation instructions to use Saena as a stand-alone solver

1- The CMakeLists.txt is written in a way to use Saena inside Nektar++. To use it as a stand-alone solver remove CMakeLists.txt. rename CMakeLists_standalone.txt to CMakeLists.txt.

2- copy paste the following block of commands:

mkdir build && cd build; \
make config prefix=\`pwd\` -C ../external/parmetis-4.0.3/metis; \\\
cd build_metis; \\\
make install; \\\
cd ..; \\\
make config prefix=\`pwd\` -C ../external/parmetis-4.0.3; \\\
cd build_parmetis; \\\
make install; \\\
cd ..; \\\
mkdir build_zfp && cd build_zfp; \\\
cmake ../../external/zfp-0.5.3; \\\
make -j28; \\\
cd ..; \\\
mkdir build_superlu && cd build_superlu; \\\
cmake ../../external/SuperLU_DIST_5.4.0 \\\
-DXSDK_INDEX_SIZE=64 \\\
-Denable_blaslib=OFF \\\
-DCMAKE_INSTALL_PREFIX=. ;\\\
make -j28 install; \\\
cd ..; \\\
cmake ..; \\\
make -j28


## Example

There is an example showing how to use Saena in src/main.cpp. Most functions are explained there.

Run the following command in the build folder to run an example of 3DLaplacian. It accepts 3 arguments as size of x, y and z axes sized.

./Saena 9 9 9

Pass any matrix (A) in binary format as the argument to solve the system: Ax = rhs.
The matrix should be in COO format with column-major order (the same format as the Florida Matrix Collection).
rhs is generated randomly in this example. It is exaplained how to also pass rhs in the example.

## Nektar++

To use Saena inside Nektar++ clone this repositoy inside Nektar++/library/MultiRegions/

## Use Saena in another library

The following commands can be used to use Saena in a library:

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

solver.solve_pcg(u, &opts); // AMG as a preconditioner. The solver is preconditioned conjugate gradient (pcg).

// *************************** Destroy ****************************

A.destroy();

solver.destroy();

## Print Functions

print matrix A of type saena::matrix:

A.print(cpu_rank); // print matrix entries on processor with rank "cpu_rank"

A.print(-1); // print matrix entries on all processors 0, 1, 2, ..., respectively.

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
