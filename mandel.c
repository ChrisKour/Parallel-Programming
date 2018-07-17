#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define         X_RESN  1000       /* x resolution */
#define         Y_RESN  1000       /* y resolution */
#define         X_MIN   -2.0
#define         X_MAX    2.0
#define         Y_MIN   -2.0
#define         Y_MAX    2.0
#define         GRADES  100
#define 	NUMBER_OF_THREADS 4 //how many threads we want running
#define 	CHUNKSIZE 10	//the size of the chunk we want to use

typedef struct complextype
        {
        float real, imag;
        } Compl;

int main ( int argc, char* argv[])
{

       /* Mandlebrot variables */
        int i, j, k, l,chunk;
        Compl   z, c;
        float   lengthsq, temp;
        int maxIterations;
        int res[X_RESN][Y_RESN]; 
        int gray[GRADES];
	double start_time,stop_time; //dhlwsh metablhtwn xronou


        /* Calculate and draw points */
        maxIterations = atoi( argv[1] );
        for (l=0; l<GRADES; l++) 
        	gray[l] = (l+1)*maxIterations/GRADES;

	chunk = CHUNKSIZE; //giving chunk it's size

	omp_set_num_threads(NUMBER_OF_THREADS); //setting the number of threads

	start_time = omp_get_wtime(); //starting the timer
     
	#pragma omp parallel shared(gray,res) private(i,j,z,c,temp,k,lengthsq,l)//beginning of parallel programming
  	{

  		#pragma omp for schedule(dynamic,chunk) nowait//here we set the type of schedule static/dynamic/guided and if we want to set manual chunk or not, nowait ensures that the threads will not wait each other to finish
        	for(i=0; i < Y_RESN; i++) 
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
	}

	stop_time = omp_get_wtime();//end of timer

        printf("\n");
        for(i=0; i < Y_RESN; i++) {
          	for(j=0; j < X_RESN; j++) 
            		printf("%3d", res[i][j]);
        	printf("\n");
        }

	printf("Time with dynamic (chunk: %d) omp for: %f, resolution %d and %d iterations\n", chunk,stop_time-start_time,X_RESN,maxIterations);//printing the results
        

        /* Program Finished */
        return 0;
}
