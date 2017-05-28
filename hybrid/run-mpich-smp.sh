#!/bin/bash
# EXEMPLE PER A 3 PROCESSOS MPI I THREADS
# Recordeu que el nombre total de processos que s'ha de demanar pel procés és
# igual al nombre de processos MPI x 4.
# Per tant, si per exemple si voleu 3 processos MPI -> 3 MPI x 4 = 12

## Specifies the interpreting shell for the job.
#$ -S /bin/bash

## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Execute the job from the current working directory.
#$ -cwd

## Parallel programming environment (mpich-smp) to instantiate and number of computing slots.
#$ -pe mpich-smp 4

## The  name  of  the  job.
#$ -N hybrid_4

## Send an email at the start and the end of the job.
#$ -m be

## The email to send the queue manager notifications. 
#$ -M ujape@yahoo.es

## The folders to save the standard and error outputs.
#$ -o $HOME/prac/hpc-project
#$ -e $HOME/prac/hpc-project

MPICH_MACHINES=$TMPDIR/mpich_machines
cat $PE_HOSTFILE | awk '{print $1":1"}' > $MPICH_MACHINES


## In this line you have to write the command that will execute your application.
for i in testbed/t*;
     do	mpiexec -f $MPICH_MACHINES -n $NSLOTS ./hybrid $i;
done;


rm -rf $MPICH_MACHINES

