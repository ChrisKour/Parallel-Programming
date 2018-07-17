#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define MASTER 0
#define WORKTAG 1
#define DIETAG 2
#define TRUE 1

#define X_RESN  100       /* x resolution */
#define Y_RESN  100       /* y resolution */
#define X_MIN   -2.0
#define X_MAX    2.0
#define Y_MIN   -2.0
#define Y_MAX    2.0
#define GRADES  100

typedef struct complextype
        {
        float real, imag;
        } Compl;

int main (int argc, char *argv[])
{
	int	my_rank,	/* rank of process */
		num_procs,	/* number of processes */
		i, 		
		sender,		/* sender */
		active,		/* number of active processes */
		index,
		number;		/* current index of array */
	MPI_Status status;

	/* Mandlebrot variables */
        int j, k, l;
        Compl   z, c;
        float   lengthsq, temp;
        int maxIterations;
        int res[X_RESN][Y_RESN]; 
        int gray[GRADES];

	/* Start up MPI */
	MPI_Init(&argc,&argv);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD,&num_procs);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

        /* master code */
	if (my_rank == MASTER) {	
			
		/* Calculate and draw points */
        	maxIterations = atoi( argv[1] );
        	for (l=0; l<GRADES; l++) 
		gray[l] = (l+1)*maxIterations/GRADES;

	}

	MPI_Bcast(&maxIterations,1,MPI_INT,0,MPI_COMM_WORLD);		//δίνουμε τις τιμές maxIterations στους workers
	MPI_Bcast(gray,GRADES,MPI_INT,0,MPI_COMM_WORLD);		//και τον πίνακα gray ώστε να γίνουν οι υπολογισμοί

	if (my_rank == MASTER) {
			
		/* Initialize number of active processes and array pointer */
		active = index = 0;

		/* Send the number of a row to each worker */ 		
		for(i = 1; i <= num_procs - 1; i++) {	
			MPI_Send(&index,1,MPI_INT,i,WORKTAG,MPI_COMM_WORLD);
			active++; /* Increment number of active processes */ 
			index++;  /* Increment array pointer */ 	 	
		} 		

		/* Receive result from any worker and send next number */
		do {
			
			MPI_Recv(&res[index], 1*X_RESN,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			active--; /* Decrement the number of active processes */

			/* Determine who sent the result */
			sender =(status.MPI_SOURCE); 

			/* Check if there are other numbers */ 			
			if (index < Y_RESN) {
				MPI_Send(&index,1,MPI_INT,sender,WORKTAG,MPI_COMM_WORLD);
				active++; /* Increment the mumber of active processes */	 		   	 			
				index++; /* Increment the pointer of array */ 
	   		} else   	 	 	
				/* Tell the worker to exit or die */
				MPI_Send(0,0,MPI_INT,sender,DIETAG,MPI_COMM_WORLD); 
					  		
		} while (active>0); 

		printf("\n");		//Εκτύπωση αποτελεσμάτων
        	for(i=0; i < Y_RESN; i++) {
          		for(j=0; j < X_RESN; j++) 
            			printf("%3d", res[i][j]);
        		printf("\n");
        	}
 	
	} 
	/* worker code */
	else { 
		while(TRUE) { 
					
			/* Receive the current number */
			MPI_Recv(&number,1,MPI_INT,MASTER,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			
			/* Check the tag of the received message */
			if (status.MPI_TAG == DIETAG) break;
				
			for(j=0; j < X_RESN; j++) {
            			z.real = z.imag = 0.0;
            			c.real = X_MIN + j * (X_MAX - X_MIN)/X_RESN;
            			c.imag = Y_MAX - number * (Y_MAX - Y_MIN)/Y_RESN;
            			k = 0;

            			do  {    /* iterate for pixel color */

              				temp = z.real*z.real - z.imag*z.imag + c.real;
              				z.imag = 2.0*z.real*z.imag + c.imag;
              				z.real = temp;
              				lengthsq = z.real*z.real+z.imag*z.imag;
              				k++;

            			} while (lengthsq < 4.0 && k < maxIterations);

           			for (l=0; l<GRADES; l++)  
            				if (k <= gray[l]) {
               					res[number][j] = l;
               					break;
            				}
        		}
				
			/* Send the result to the master */
			MPI_Send(&res[number], 1*X_RESN,MPI_INT,MASTER,0,MPI_COMM_WORLD); 
				
		 }
 	}			
	
	/* Shut down MPI */
	MPI_Finalize();
	return 0;
}
