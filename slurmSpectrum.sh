#!/bin/bash -x

if [ "x$SLURM_NPROCS" = "x" ]
then
  if [ "x$SLURM_NTASKS_PER_NODE" = "x" ]
  then
    SLURM_NTASKS_PER_NODE=1
  fi
  SLURM_NPROCS=`expr $SLURM_JOB_NUM_NODES \* $SLURM_NTASKS_PER_NODE`
else
  if [ "x$SLURM_NTASKS_PER_NODE" = "x" ]
  then
    SLURM_NTASKS_PER_NODE=`expr $SLURM_NPROCS / $SLURM_JOB_NUM_NODES`
  fi
fi

srun hostname -s | sort -u > /tmp/hosts.$SLURM_JOB_ID
grep -q 'release 7\.' /etc/redhat-release
if [ $? -eq 0 ]; then
  net_suffix=-ib
fi
awk "{ print \$0 \"$net_suffix slots=$SLURM_NTASKS_PER_NODE\"; }" /tmp/hosts.$SLURM_JOB_ID >/tmp/tmp.$SLURM_JOB_ID
mv /tmp/tmp.$SLURM_JOB_ID /tmp/hosts.$SLURM_JOB_ID

module load spectrum-mpi
taskset --cpu-list 0,4 mpirun -np 2 ./mpi-io-benchmark
rm datafile
taskset --cpu-list 0,4,8,12 mpirun -np 4 ./mpi-io-benchmark
rm datafile
taskset --cpu-list 0,4,8,12,16,20,24,28 mpirun -np 8 ./mpi-io-benchmark
rm datafile
taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60 mpirun -np 16 ./mpi-io-benchmark
rm datfile
taskset --cpu-list 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124 mpirun -np 32 ./mpi-io-benchmark
rm datafile
#kokkos users should add: --kokkos-num-devices=N
#where N is the number of gpus being used on each node

rm /tmp/hosts.$SLURM_JOB_Ie
