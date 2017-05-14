

/*** High Performace Computing Course 2016-2017 ***
 *
 * Part 2: MPI Implementation for 0-1 Knapsack problem 
 * MPI Dynamic Programming based solution 
 * By Oscar Ujaque and Mariano Garralda
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <string.h>

// A utility function that returns maximum of two integers
int max(int a, int b) { return (a > b)? a : b; }


// Returns the maximum value that can be put in a knapsack of capacity W
static int knapSack(long int W, long int N, int wt[], int val[], double* timeSpent)
{
	
	long int i, w;


	// the value for the final result
    int result=0; 
     
 	int size, CPU;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &CPU);
   
    //initial time
	double start=MPI_Wtime();

    /* MASTER*/

	if (CPU==0){
        //define the matrix. We save space by defining in this way
        int K[1][W+3] ;
		i=1;
        int m[W+3];
        // we initialize the matrix to 0
		memset(K[0], 0, (W+3)*sizeof(int));
        //K[0][Ẁ+2] = {{0}};
        int slave;
        // while i<N. We send to and receive from the slaves  
   		while(i<=N){
            //we send a message to all the slaves with K[0] vector
            // we only send N*CPU's messages
         
     		for (slave=1;slave<size ;slave++)  
     			MPI_Send((void*)K[0], W+1, MPI_INT, slave, 1, MPI_COMM_WORLD);
                  
     		int count=1;
            //we recive the 'm' vector and we allocate it in the matrix K
		    //It makes an implicit barrier for all the slaves
     		while(count < size){
                //recive the vector. In the two last positions there are the 'w' indexes from each core
				MPI_Recv((void*)m, W+3, MPI_INT, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //allocate in the matrix. The fastest way of allocating the vector received in 'K'
                memcpy(K[0]+m[W+1], m+m[W+1], sizeof(int) * (m[W+2]-m[W+1])+sizeof(int));
                count++;
                
                } 
			i++;
	     } 
        //we store the final result
        result=K[0][W];
     
	}

    /* SLAVES */

	else{    
        //we define the matrix. We save memory by defining the following matrix for each slave 
        //each computation only needs a column and its predecessor. We can change 'N+1' by '2'
        int  M[2][W+3];
     	int k=1; 
        
        // while k<N
     	while(k<=N){
			
            // fixed i=1. We always only need two column. The 'i' and 'i-1'
        	i=1;

            //receive the message from the master
			MPI_Recv((void*)M[i-1], W+1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            
         	int init, end;
            // we split the data in order to balance the load between each core
        	if (CPU==size-1)
            	end=W;
        	else
         		end =W/(size-1)*CPU;
         	if (CPU==1)
            	init= ((W/(size-1))*(CPU-1));
            else
			    init= ((W/(size-1))*(CPU-1))+1;

            // each core only computes a part of the data. Exactly W/(size-1) data per core
	  		for (w =init; w <= end; w++)
				{

				if (i==0 || w==0)
					M[i][w] = 0;
				else if (wt[k-1] <= w)
					M[i][w] = max(val[k-1] + M[i-1][w-wt[k-1]], M[i-1][w]);
				else
					M[i][w] = M[i-1][w];

                }
            //we store the initial and final 'w' per core in the last positions of the vector
            M[i][W+1]=init;
            M[i][W+2]=end;
            //each core sends a message to the master with K[i] vector and the 'w' initial and final position
            //we only send N*CPU's messages. 
            //The total number of messages sent in the program are '2*N*CPU's'. It does not depend on 'W'
            MPI_Send((void*)M[i], W+3, MPI_INT, 0, 10, MPI_COMM_WORLD);
	   		k++;  
            
	   
        }           
       

	}
    //we compute the time spent in the parallelization
    (*timeSpent)=MPI_Wtime()-start;

    MPI_Finalize();
    

    return result;
     
	
}

int main(int argc, char **argv){

    FILE *test_file;
    double start_time, elapsed_time; /* for time measurement */

    int *val, *wt;   // width and cost values
    long int Nitems; // Number of items
    long int Width;  // Max. load to carry
    long int cont;    // counter
    double tpivot1=0,tpivot2=0,tpivot3=0; //time counting
    struct timeval tim;

	if (argc!=2) {
	  printf("\n\nError, mising parameters:\n");
	  printf("format: test_file \n");
	  /*printf("format: capacity items val_file weight_file allocation_file\n");
	  printf("- capacity: Max. Capacity of the Backpack");
	  printf("- items: Number of items to be evaluated.\n");
	  printf("- val_file: Value data of the items.\n");
	  printf("- weigth_file: Weight values of the items.\n");
  	  printf("- allocation_file: 0/1 (NO/YES) allocation of the item.\n\n");*/
	  return 1;
	} 
	
    //Capture first token time - init execution
    gettimeofday(&tim, NULL);
    tpivot1 = tim.tv_sec+(tim.tv_usec/1000000.0);
    
	if (!(test_file=fopen(argv[1],"r"))) {
	  printf("Error opening Value file: %s\n",argv[1]);
	  return 1;
	}

    //Reading number of items and Maximum width
	fscanf(test_file,"%ld %ld\n",&Nitems, &Width);
    
    //printf("%ld\n",Nitems*sizeof(int));
		  double timeSpent=0;
	val = (int *)malloc(Nitems*sizeof(int)); //values for each element
	 wt = (int *)malloc(Nitems*sizeof(int)); //width  for each element
    
    //Reading value and width for each element
	for (cont=0;cont<Nitems;cont++){
	  fscanf(test_file,"%d,%d\n",&val[cont],&wt[cont]);
	}

     //    printf("\n\n ---- Results TCapacity:items:Tcost:toptim:ttime ----\n\n");
    gettimeofday(&tim, NULL);
    tpivot2 = (tim.tv_sec+(tim.tv_usec/1000000.0));
    //printf("HOLA2\n");
    
    //int K[Nitems+1][Width+1] ;
    int result=  knapSack(Width,Nitems, wt, val,&timeSpent);
      // we only print the result for the core with result greater than 0 (core 0).
      if (result>0){
         printf("%ld:%ld:%d", Width, Nitems, result);

    	gettimeofday(&tim, NULL);

    	tpivot3 = (tim.tv_sec+(tim.tv_usec/1000000.0));
   		 printf(":%.6lf:%.6lf\n", timeSpent,tpivot3-tpivot1);
   }

	free(val);
	free(wt);
	
	fclose(test_file);

	return 0;
}
