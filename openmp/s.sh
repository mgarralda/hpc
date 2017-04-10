#!/bin/bash

rm outSerial.txt
rm outParallel.txt

gcc  knapsackDYN_serial.c -o serial
gcc -fopenmp knapsackDYN_PARAL.c -o parallel

for i in testbed/t*; 

do ./serial $i >> outSerial.txt ;

done


for i in testbed/t*; 

do ./parallel $i >> outParallel.txt ;

done

gcc check.c -o check
./check outSerial.txt  outParallel.txt


