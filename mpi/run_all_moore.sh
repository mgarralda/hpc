#!/bin/bash

#we remove the older files
rm outSerial.txt
rm outParallel.txt

# we compile the both programs
gcc  knapsack_dyn_serial.c -o serial
mpicc $1 -o parallelMPI2



#we execute the serial program for all testbed files
for i in testbed/t*; 

do ./serial $i >> outSerial.txt ;

done


# we execute the parallel program for all the testbed files
qsub run_extended.sh

# sleep for a while waiting for qsub finishes
sleep 1000s
# we compile the checkOutputs program
gcc check_outputs.c -o check

#we execute the checkOutputs program 
./check outSerial.txt  outParallel.txt


