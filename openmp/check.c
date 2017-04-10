#include <stdio.h>
#include <stdlib.h>
int main( int argc, const char* argv[] )
{
	int* x,*y,*z;
	int i;
        float* t,*w;
        FILE *test_file;
        test_file=fopen(argv[1],"r");

           x=(int*)malloc(15*sizeof(int));
           y=(int*)malloc(15*sizeof(int));
           z=(int*)malloc(15*sizeof(int));
           t=(float*)malloc(15*sizeof(float));
           w=(float*)malloc(15*sizeof(float));
        for (i =0;i<15;i++)

             fscanf(test_file,"%d:%d:%d:%f:%f\n",&x[i],&y[i],&z[i],&t[i],&w[i]);
   
        int* x2,*y2,*z2;
        float* t2,*w2;
        FILE *test_file2;
        test_file2=fopen(argv[2],"r");

           x2=(int*)malloc(15*sizeof(int));
           y2=(int*)malloc(15*sizeof(int));
           z2=(int*)malloc(15*sizeof(int));
           t2=(float*)malloc(15*sizeof(float));
           w2=(float*)malloc(15*sizeof(float));
        for (i =0;i<15;i++)

             fscanf(test_file2,"%d:%d:%d:%f:%f\n",&x2[i],&y2[i],&z2[i],&t2[i],&w2[i]);
        

        for (i=0;i<15;i++){
             int correct=0;
             if (x[i]==x2[i] && y[i]==y2[i] && z[i]==z2[i]){
                printf("\n OK test_%d_%d --> \t",y[i],x[i]);
                correct=1;
}
             else
                printf("\n WRONG TEST !!!! test_%d_%d \t\n",y[i],x[i]);
             if (t[i] < t2[i] && correct )
                 printf ("NO improvement. Better with serial thread in %f\n", t2[i]-t[i]);
             else if (correct)
                 printf ("IMPROVEMENT. Better with 4 threads in %f\n", t[i]-t2[i]);
                  
        }
}

