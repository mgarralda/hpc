#!/bin/bash

#we remove the older files
rm outSerial.txt
rm outParallel.txt

# we compile the both programs
gcc  knapsackDYN_serial.c -o serial
mpicc $1 -o parallel



#we execute the serial program for all testbed files
for i in testbed/t*; 

do ./serial $i >> outSerial.txt ;

done


# we execute the parallel program for all the testbed files
for i in testbed/t*; 

do mpiexec -n $2  parallel $i >> outParallel.txt ;

done

sleep 0s
# we compile the checkOutputs program
gcc check_outputs.c -o check

#we execute the checkOutputs program 
./check outSerial.txt  outParallel.txt


