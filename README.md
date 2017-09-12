# Saena_Public

// *************************** AMG - Setup ****************************

saena::matrix A(num_rows_global, comm);
A.set(I, J, V, size);
A.assemble();

saena::options opts((char*)"options001.xml");
saena::amg solver(&A);

// *************************** AMG - Solve ****************************

solver.solve(u, rhs, &opts);

// *************************** Finalize ****************************

A.destroy();
solver.destroy();
