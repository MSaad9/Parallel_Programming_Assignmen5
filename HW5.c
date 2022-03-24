#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define BLOCKSIZE 32 //change this according to the test cases

void sample_error(int error, char *string)
{
    fprintf(stderr, "Error %d in %s\n", error, string);
    MPI_Finalize();
    exit(-1);
}

int main(int argc, char **argv)
{
    int myRank, totalRanks; //holds the current rank and total number of ranks
    long long offset;
    MPI_File fh;
    MPI_Status status;
    int bufsize, nints;
    int result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);
    // bufsize = BLOCKSIZE / totalRanks;
    // nints = bufsize / sizeof(int);
    // printf("Bufsize: %d\n", bufsize);

    // create data array of bufsize for each rank
    // Initialize array with 1s
    char *writeData = calloc(BLOCKSIZE, sizeof(char));
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        writeData[i] = 1;
    }
    MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    for (int i = 0; i < 4; i++)
    {
        offset = (BLOCKSIZE * totalRanks * i) + (myRank * BLOCKSIZE);
        // printf("Offset: %d\n", offset);
        result = MPI_File_write_at(fh, offset, writeData, BLOCKSIZE, MPI_BYTE, &status);
        if (result != MPI_SUCCESS)
            sample_error(result, "MPI_File_write_at");
    }
    MPI_File_close(&fh);

    MPI_Barrier(MPI_COMM_WORLD);
    // Read Data from the file

    char *readData = calloc(BLOCKSIZE, sizeof(char));
    MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    for (int i = 0; i < 4; i++)
    {
        offset = (BLOCKSIZE * totalRanks * i) + (myRank * BLOCKSIZE);
        MPI_File_read_at(fh, offset, readData, BLOCKSIZE, MPI_BYTE, &status);
    }
    MPI_File_close(&fh);

    for (int i = 0; i < BLOCKSIZE; i++)
    {
        printf("MPI RANK: %d Data: %d ", myRank, readData[i]);
    }
    printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}