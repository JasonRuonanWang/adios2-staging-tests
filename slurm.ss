#!/bin/bash
#SBATCH -A csc303
#SBATCH -J adios2_ssc
#SBATCH -o ssc.out
#SBATCH -t 00:05:00
#SBATCH -p batch
#SBATCH -N 17

srun -N16 -n1024 ./writer : -N1 -n1 ./reader
