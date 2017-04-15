#!/bin/bash

#we remove the older files
rm outSerial.txt
rm outParallel.txt

# we compile the both programs
gcc  knapsack_dyn_serial.c -o serial
gcc -fopenmp knapsack_dyn_parallel.c -o parallel

#we execute the serial program for all testbed files
for i in testbed/t*; 

do ./serial $i >> outSerial.txt ;

done


# we execute the parallel program for all the testbed files
for i in testbed/t*; 

do ./parallel $i >> outParallel.txt ;

done

# we compile the checkOutputs program
gcc check_outputs.c -o check

#we execute the checkOutputs program 
./check outSerial.txt  outParallel.txt


