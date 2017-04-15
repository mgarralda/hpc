!/bin/bash

# we store in a variable the all the outputs of qsub operation in order to be removed
pattern1='outParallel*'

# we store in a variable only the output file in order to be executed below.
pattern2='outParallel.o*'

# we clean older files
rm outSerial.txt
rm $pattern1


# we compile the serial and parallel (with pragmas) programs
gcc  knapsack_dyn_serial.c -o serial
gcc -fopenmp knapsack_dyn_parallel.c -o parallel


# we execute the serial programs with the available tests
for i in testbed/t*; 

do ./serial $i >> outSerial.txt ;

done

# we execute the program into moore cluster
qsub run-simple-omp.sh

sleep 40s

echo "waiting for the results... Please wait!!"

# we compile and execute the program which checks the both outputs
gcc checkOutputs.c -o check
./check outSerial.txt  $pattern2


