#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define BLOCKSIZE 128000 //change this according to the test cases

int main(int argc, char **argv)
{
    int myRank, totalRanks; //holds the current rank and total number of ranks
    MPI_File fh;
    MPI_Status status;
    int bufsize, nints;
    // int buf[BLOCKSIZE];
    bufsize = BLOCKSIZE / totalRanks;
    // nints = bufsize / sizeof(int);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);

    // create data array of bufsize for each rank
    // Initialize array with 1s
    int *writeData = calloc(bufsize, sizeof(int));
    for (int i = 0; i < bufsize; i++)
    {
        writeData[i] = 1;
    }

    for (int i = 0; i < 32; i++)
    {
        MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
        MPI_File_write_at(fh, (BLOCKSIZE * i) + (myRank * bufsize), writeData, bufsize, MPI_INT, &status);
        MPI_File_close(&fh);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Read Data from the file

    int *readData = calloc(bufsize, sizeof(int));

    for (int i = 0; i < 32; i++)
    {
        MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
        MPI_File_read_at(fh, (BLOCKSIZE * i) + (myRank * bufsize), readData, bufsize, MPI_INT, &status);
        MPI_File_close(&fh);
    }

    for (int i = 0; i < bufsize; i++)
    {
        printf("%s,%d,%d", "MPI RANK: ", myRank, readData[i]);
    }
    printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);
}