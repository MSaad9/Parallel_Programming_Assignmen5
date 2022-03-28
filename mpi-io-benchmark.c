#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define BLOCKSIZE 16777216  //change this according to the test cases

unsigned long long aimos_clock_read(void)
{
  unsigned int tbl, tbu0, tbu1;
  do {
    __asm__ __volatile__ ("mftbu %0" : "=r"(tbu0));
    __asm__ __volatile__ ("mftb %0" : "=r"(tbl));
    __asm__ __volatile__ ("mftbu %0" : "=r"(tbu1));
  } while (tbu0 != tbu1);
  return (((unsigned long long)tbu0) << 32) | tbl;
}


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
    unsigned long long start;
    unsigned long long end;

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
    start = aimos_clock_read();
    MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    for (int i = 0; i < 32; i++)
    {
        bufsize = BLOCKSIZE/totalRanks;
        offset = BLOCKSIZE*i +  myRank * bufsize;
        // printf("Offset: %d\n", offset);
        result = MPI_File_write_at(fh, offset, writeData, bufsize, MPI_BYTE, &status);
        if (result != MPI_SUCCESS)
            sample_error(result, "MPI_File_write_at");
    }
    MPI_File_close(&fh);

    MPI_Barrier(MPI_COMM_WORLD);
    end = aimos_clock_read();
    // Read Data from the file
    if(myRank == 0){
        printf("time used writing to files: %llu\n", end-start);
    }
    char *readData = calloc(BLOCKSIZE, sizeof(char));
    start = aimos_clock_read();
    MPI_File_open(MPI_COMM_WORLD, "datafile", MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    for (int i = 0; i < 32; i++)
    {
        bufsize = BLOCKSIZE/totalRanks;
        offset =  BLOCKSIZE*i + myRank * bufsize;
        MPI_File_read_at(fh, offset, readData, bufsize, MPI_BYTE, &status);
    }
    MPI_File_close(&fh);

    MPI_Barrier(MPI_COMM_WORLD);
    end = aimos_clock_read();
    if(myRank == 0){
      printf("time used reading to files: %llu\n", end-start);
    }
    MPI_Finalize();

    return 0;
}
