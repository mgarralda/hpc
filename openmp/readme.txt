<b>run_all_pc.sh</b>: a bash program which only can be executed in our own PCs. It compiles the serial and the parallel programs and compares the outputs by calling another program (checkOutputs.c program) which compares the both outputs.

run_all_moore.sh: the same bash program than the explained before but capable of being executed in Moore cluster (instead of our own computers) by using qsub command.

run-simple-omp.sh: the sh file to be launched by qsub command. It has a modification in the executing part in order to achieve an output format like the serial format. The modification consists on a loop which throws the parallel program so many times as testbed files and taking the different testbed files as a parameter.

check_outputs.c: C program that compares two files; the output of the serial program and the output of parallel program. It presents a description of the results by showing if there are differences between them. First print if the results are the same or not and the improvement time.

knapsack_DYN_serial.c: the initial program

knapsack_DYN_PARAL.c: the parallelizable program with the pragma sentences
