#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[])
{
    	int rank, size, i, j;
	double x, my_x;
    	int tag=100;
    	double *b, *a;
    	double rbufb;
        
    	MPI_Status status;
    	MPI_Init(&argc,&argv);
    	MPI_Comm_size(MPI_COMM_WORLD,&size);
    	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	double rbufa[size];	//o πίνακας rbufa παίρνει το μέγεθος του size
    
    	/* Data initialization */
    	if (rank == 0) {
		a = malloc(size * size * sizeof(double));	//οι πίνακες a,b παίρνουν το μέγεθός τους ανάλογα με το size
    		b = malloc(size * sizeof(double));

    		for (i=0; i<size; i++)
    			for(j = 0; j<size; j++)
				if (i == j) 
					a[i*size+j] = 1.0;	//γέμισμα του πίνακα a
				else if ( i > j)
					a[i*size+j] = i+j+1;
				else 	
					a[i*size+j] = 0.0;

        	for (i=0; i<size; i++){				//εκτύπωση του πίνακα a
    			for(j = 0; j<size; j++)
				printf("%f \t", a[i*size+j]);
			printf("\n");
        	}
				
		for(j = 0; j<size; j++)				//γέμισμα του πίνακα b
        		b[j] = 2*j+1;
    	}

    	/* Data distribution */
    	MPI_Scatter(a, size, MPI_DOUBLE, rbufa, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    	MPI_Scatter(b, 1, MPI_DOUBLE, &rbufb, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 

    	/* Source of pipeline */
    	if (rank == 0){
        	my_x = rbufb/rbufa[0];
        	MPI_Send(&my_x, 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
        	printf("Sending my_x = %f to process 1\n", my_x);   
    	} 
           
    	/* Main pipeline */    
    	if ((rank != 0) && (rank != (size-1))) {
        	my_x = 0;
        	for (i=0; i<rank; i++){
        		MPI_Recv(&x, 1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD,&status);
        		MPI_Send(&x, 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
        		printf("Receiving - Sending x = %f from - to process %i - %i\n", x, rank-1, rank+1);
                	my_x = my_x + x*rbufa[i];       
        	}
        	my_x = (rbufb - my_x)/rbufa[rank];    
        	MPI_Send(&my_x, 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
        	printf("Sending my_x = %f to process %i\n", my_x, rank+1);
    	}
    
    	/* Sink of pipeline */    
    	if (rank == (size-1)) {
        	my_x = 0;
        	for (i=0; i<rank; i++){
        		MPI_Recv(&x,1,MPI_DOUBLE,rank-1,tag,MPI_COMM_WORLD,&status);
        		printf("Receiving x = %f from process %i\n", x, rank-1);
        		my_x = my_x + x*rbufa[i];
		} 
		my_x = (rbufb - my_x)/rbufa[rank]; 
        	printf("The final my_x of %i processess is %f\n",size, my_x);
    	}
           
  	MPI_Finalize();
  	return 0;
}

