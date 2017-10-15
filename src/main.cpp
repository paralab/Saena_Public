
#include <iostream>
#include <algorithm>
#include "mpi.h"

#include "grid.h"
#include "saena.hpp"


using namespace std;

int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int nprocs, rank;
    MPI_Comm_size(comm, &nprocs);
    MPI_Comm_rank(comm, &rank);

    unsigned long i;
//    int assert1, assert2, assert3;

    if(argc != 3)
    {
        if(rank == 0)
        {
            cout << "Usage: ./Saena <MatrixA> <rhs_vec>" << endl;
            cout << "Matrix file should be in triples format." << endl;
        }
        MPI_Finalize();
        return -1;
    }

    // *************************** initialize the matrix ****************************

    char* file_name(argv[1]);
    saena::matrix A (file_name, comm);
    A.assemble();

    unsigned int num_local_row = A.get_num_local_rows();

    // *************************** read the vector and set rhs ****************************

    MPI_Status status;
    MPI_File fh;
    MPI_Offset offset;

    char* Vname(argv[2]);
    int mpiopen = MPI_File_open(comm, Vname, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if(mpiopen){
        if (rank==0) cout << "Unable to open the rhs vector file!" << endl;
        MPI_Finalize();
        return -1;
    }

    // define the size of v as the local number of rows on each process
    std::vector <double> rhs(num_local_row);
    double* vp = &(*(rhs.begin()));

    // vector should have the following format: first line shows the value in row 0, second line shows the value in row 1
    offset = A.get_internal_matrix()->split[rank] * 8; // value(double=8)
    MPI_File_read_at(fh, offset, vp, num_local_row, MPI_DOUBLE, &status);

//    int count;
//    MPI_Get_count(&status, MPI_UNSIGNED_LONG, &count);
    //printf("process %d read %d lines of triples\n", rank, count);
    MPI_File_close(&fh);

    // *************************** set u0 ****************************

    std::vector<double> u(num_local_row);
    u.assign(num_local_row, 0); // initial guess = 0

    // *************************** AMG - Setup ****************************
    // There are 3 ways to set options:
    // 1- set them one by one
//    int vcycle_num            = 10;
//    double relative_tolerance = 1e-8;
//    std::string smoother      = "jacobi";
//    int preSmooth             = 2;
//    int postSmooth            = 2;
//    saena::options opts(vcycle_num, relative_tolerance, smoother, preSmooth, postSmooth);

    // 2- read the options from file
//    saena::options opts((char*)"options001.xml");

    // 3- use the default options
    saena::options opts;
    saena::amg solver(&A);

    // *************************** AMG - Solve ****************************

    solver.solve(u, rhs, &opts);

    // *************************** Destroy ****************************

    A.destroy();
    solver.destroy();
    MPI_Finalize();
    return 0;
}
