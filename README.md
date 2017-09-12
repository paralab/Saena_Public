# Saena_Public

#include "/include/saena.hpp"

// *************************** AMG - Setup ****************************

saena::matrix A(num_rows_global, comm); // num_rows_global: size of A. comm: MPI_Communicator

A.set(I, J, V, size); // size: size of I (or J or V, they have the same size)

A.assemble();


saena::options opts((char*)"options001.xml"); // pass the name of the options file

saena::amg solver(&A);

// *************************** AMG - Solve ****************************

// u is the initial guess. after calling the solve function, it will be solution.

solver.solve(u, rhs, &opts); 

// *************************** Destroy ****************************

A.destroy();

solver.destroy();
