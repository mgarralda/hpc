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
#define WORKTAG    1
#define DIETAG     2

// A utility function that returns maximum of two integers
int max(int a, int b) { return (a > b)? a : b; }

// Returns the maximum value that can be put in a knapsack of capacity W
static int knapSack( int W,  int N, int wt[], int val[]){			
	int i, w, p, m, r;	
	int cores, rank,j, core;		
	
	// Structure declaration to use move data between processors
	struct {
		int start;			/* Start index of array results for each slave(task)*/
		int end;			/* End index of buffer results for each slave(task) */			
		int row;			/* Index row processing from master */		
		int results[W+1];	/* Results array from each chunk size computed */
		int lastrow[W+1];  	/* Last row computed */ 
	} data;	
	
	// MPI structure needs
	MPI_Datatype dataStruct;	
	MPI_Aint indexes[5];
	MPI_Datatype MPItypes[5];
	int blocks[5];	

	// Initialization MPI
	MPI_Init(NULL, NULL);
	MPI_Request request;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Time counter
	double start = MPI_Wtime();
		
	/* One value of each type */	
	blocks[0] = 1;
	blocks[1] = 1;
	blocks[2] = 1; 
	blocks[3] = W+1; 
	blocks[4] = W+1; 	
	/* The locations of each element */
	MPI_Get_address( &data.start, &indexes[0] );
	MPI_Get_address( &data.end, &indexes[1] );
	MPI_Get_address( &data.row, &indexes[2] );	
	MPI_Get_address( &data.results, &indexes[3] );
	MPI_Get_address( &data.lastrow, &indexes[4] );
	/* Get base address */
	MPI_Aint baseaddres = indexes[0];	
	for (i=0; i<5; i++) {			
		MPItypes[i] = MPI_INT;			/* The base types */		
		indexes[i] = indexes[i] - baseaddres; 	/* Make relative address */
	}
	MPI_Type_create_struct( 5, blocks, indexes, MPItypes, &dataStruct );
	MPI_Type_commit( &dataStruct );	
		
	/*
	 *  Skeleton for load/balance master/slave application
	 */
	if (rank == 0) {	/*** Master Task ***/
	/* Master:
		Data decomposition based of number's cores
		Statical mapping strategy with load balance because all proccessor have the same computer complexity
		Send data decomposition (task) to each slave
		Loop until all jobs has been computed via blocking way, so barrier is guaranteed
	*/				
		// This solution only need a file to shaved results (not full matrix N*W)
		int K[W+1];								
        memset(K,0,sizeof(int)*(W+1));
		for (i = 1; i<=N; i++){ 	/*** Sequentially for each row ***/			
			
			/* Initialize values for the new row to compute */			
			data.row = i;		
		
			// Copy values from last row computed necessary send to all slaves (task)
            memcpy(data.lastrow,K,(W+1)*sizeof(int));				
			/*for (r=0; r<=W; r++) {								
				data.lastrow[r] = K[r];	
			}*/

			// Send to all slave last row proccesed				
			//MPI_Bcast(&K, W, MPI_INT, rank, MPI_COMM_WORLD); 			
		
			/* Send message task to slave
			 * Is safe because structure (type_data_derived) sent is shared only a part of results array.
			 * It means that each slave only modified his part (data decompostion task)
			*/				
			for (core=1; core<cores; core++) {   									
				MPI_Send(&data, 1, dataStruct, core, WORKTAG, MPI_COMM_WORLD);
				// MPI_Isend(&data, 1, dataStruct, core, WORKTAG, MPI_COMM_WORLD);
			}								
		
			/*
			  Receive results task from slaves(any_source) until all task were finished in synchronous (safe mode)
			  Also ther is the Implicit barrier need for each row (serial part)
			*/							
			core = 1; // Reset core variable to use according to tasks, because are the same task=cores-1
			while (core < cores) {				
				//printf("\nMaster waiting result from any slave, p);								
				MPI_Recv(&data, 1, dataStruct, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
				// Update resulst matrix computed by slaves
                memcpy(K+data.start,data.results+data.start,(data.end-data.start)*sizeof(int)+sizeof(int));
				/*for (m=data.start; m<=data.end; m++) {
					K[m] = data.results[m];					
				}*/
				core++;
			}			
		}
				
		// Result is calculated		
		computeResultTime( K[W], W, N, MPI_Wtime()- start);
		
		/*
		 * Tell all the slaves to exit.
		 */
		for (rank = 1; rank < cores; ++rank) {
			MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
		}		
		
	} else {	/*** Worker Tasks ***/
	/* Slaves:
		Alwayis waiting for a job from the master with a blocking receive way
		Computed assigned part of data to computed
		Send the result with a blocking way
	*/					
		MPI_Status          status;
		for (;;) {	/* Always waiting a job */						
						    
			MPI_Recv(&data, 1, dataStruct, 0,  MPI_ANY_TAG, MPI_COMM_WORLD, &status);				
		
			/* Check the tag of the received message to control finish */
			if (status.MPI_TAG == DIETAG) {			 				
				return 0;
			}
			
            		/* Split the data in order to balance the load between each core */
	        	if (rank== cores-1)
            			data.end=W;
	        	else
        	 		data.end =W/(cores-1)*rank;
        	 	if (rank==1)
        		    	data.start= ((W/(cores-1))*(rank-1));
        		else
			    	data.start= ((W/(cores-1))*(rank-1))+1;			
			
			int k;
			/* Each core only computes a part of the data. Exactly W/(size-1) data per core */			
			for (k=data.start; k<=data.end; k++) {
				if (data.row ==0 || k==0)
					data.results[k] = 0;
				else if (wt[data.row-1] <= k)					
					data.results[k] = max(val[data.row-1] + data.lastrow[k-wt[data.row-1]], data.lastrow[k]);
				else
					data.results[k] = data.lastrow[k];
			}
		
			// Send computed results to master
			MPI_Send(&data, 1, dataStruct, 0, 0, MPI_COMM_WORLD);//, &request);
			// MPI_Isend(&data, 1, dataStruct, core, WORKTAG, MPI_COMM_WORLD);			
		}	
	}	
	
	MPI_Type_free(&dataStruct);
	MPI_Finalize();	
   
    return 0;	
}

// Compute time to get result
int computeResultTime(int result, int W, int N, double time)
{
	printf("\n%i:%i:%i:%.6lf\n", W, N, result, time);	
	return 0;
}

// Main function program
int main(int argc, char **argv){

    FILE *test_file;
    
    int *val, *wt;   // width and cost values
    long int Nitems; // Number of items
    long int Width;  // Max. load to carry
    long int cont;    // counter
    double tpivot1=0,tpivot2=0,tpivot3=0; //time counting
    struct timeval tim;

	if (argc!=2) {
	  printf("\n\nError, mising parameters:\n");
	  printf("format: test_file \n\n");
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
		
	val = (int *)malloc(Nitems*sizeof(int)); //values for each element
	wt = (int *)malloc(Nitems*sizeof(int)); //width  for each element
    
    //Reading value and width for each element
	for (cont=0;cont<Nitems;cont++){
	  fscanf(test_file,"%d,%d\n",&val[cont],&wt[cont]);
	}
	
    gettimeofday(&tim, NULL);
    tpivot2 = (tim.tv_sec+(tim.tv_usec/1000000.0));
    int result = knapSack(Width,Nitems, wt, val);
    tpivot3 = (tim.tv_sec+(tim.tv_usec/1000000.0));    
  
    
	free(val);
	free(wt);
	
	fclose(test_file);
	
	return 0;
}

