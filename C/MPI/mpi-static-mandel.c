#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

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
	int	my_rank,		/* rank of process */
		num_procs,		/* number of processes */
		i,offset,count,
		tag_type,	
		num_workers,		/* number of workers */
		slice,			/* number of normal items for workers */
		extra;			/* number of extra items for workers */
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

		/* Calculate slice size and adjust for remainder */
		num_workers = num_procs - 1;   
                slice = Y_RESN / num_workers;  
                extra = Y_RESN % num_workers;
                offset = 0;
                tag_type = FROM_MASTER;

		/* Calculate and draw points */
        	maxIterations = atoi( argv[1] );
        	for (l=0; l<GRADES; l++) 
			gray[l] = (l+1)*maxIterations/GRADES;
	}

	MPI_Bcast(&maxIterations,1,MPI_INT,0,MPI_COMM_WORLD);		//δίνουμε τις τιμές maxIterations στους workers
	MPI_Bcast(gray,GRADES,MPI_INT,0,MPI_COMM_WORLD);		//και τον πίνακα gray ώστε να γίνουν οι υπολογισμοί

	if (my_rank == MASTER) {
      
                for (i = 1; i <= num_workers; i++) {    
			count = (i <= extra) ? slice+1 : slice;
                        printf("   sending %d numbers to process %d\n",count,i);
                         
                        MPI_Send(&offset, 1, MPI_INT, i, tag_type, MPI_COMM_WORLD);
                        MPI_Send(&count, 1, MPI_INT, i, tag_type, MPI_COMM_WORLD);
                         
                        offset = offset + count;
		}

		/* wait for results from all worker tasks */
      		tag_type = FROM_WORKER;
                     
                for (i = 1; i <= num_workers; i++) {
			MPI_Recv(&offset, 1, MPI_INT, i, tag_type, MPI_COMM_WORLD, &status);
                        MPI_Recv(&count, 1, MPI_INT, i, tag_type, MPI_COMM_WORLD, &status);
                        MPI_Recv(&res[offset], count*X_RESN, MPI_INT, i, tag_type, MPI_COMM_WORLD, &status);	//λαμβάνουμε πίνακα 2 διαστάσεων γραμμές*ολόκληρο το x
		} 
                    
                /* Print results */
		printf("\n");
        	for(i=0; i < Y_RESN; i++) {
          		for(j=0; j < X_RESN; j++) 
            			printf("%3d", res[i][j]);
        		printf("\n");
        	}
	} 
	/* worker code */
	else {
		tag_type = FROM_MASTER;
      			
      		MPI_Recv(&offset, 1, MPI_INT, MASTER, tag_type, MPI_COMM_WORLD, &status);
      		MPI_Recv(&count, 1, MPI_INT, MASTER, tag_type, MPI_COMM_WORLD, &status);

                for(i=offset; i < count+offset; i++) 
          		for(j=0; j < X_RESN; j++) {
            			z.real = z.imag = 0.0;
            			c.real = X_MIN + j * (X_MAX - X_MIN)/X_RESN;
            			c.imag = Y_MAX - i * (Y_MAX - Y_MIN)/Y_RESN;
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
               					res[i][j] = l;
               					break;
            				}
        		}
                
		tag_type = FROM_WORKER;
      
      		MPI_Send(&offset, 1, MPI_INT, MASTER, tag_type, MPI_COMM_WORLD);
      		MPI_Send(&count, 1, MPI_INT, MASTER, tag_type, MPI_COMM_WORLD);
      		MPI_Send(&res[offset], count*X_RESN, MPI_INT, MASTER, tag_type, MPI_COMM_WORLD);
        }			
	
	/* Shut down MPI */
	MPI_Finalize();
	return 0;
}

