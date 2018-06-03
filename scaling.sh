#!/bin/bash
#SBATCH --time=00:15:00
#SBATCH --partition=sandyb
#SBATCH --output=scaling.out
#SBATCH --error=scaling.err
#SBATCH --nodes=8
#SBATCH --exclusive

# module load mvapich2

N=102400
T=10

for i in 1 2 4 8
do
    mpirun -n $i ./nbody $N $T 16
done

for i in 16 32 64 128
do
    mpirun -n $i ./nbody $N $T 1
done
