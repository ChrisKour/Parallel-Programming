#include <stdio.h> 
#include <stdlib.h>
#include <omp.h>

#define N 128 //μέγεθος πίνακα
#define base 32 //με βάση 32 εμφανίζονται μόνο οι χαρακτήρες που εκτυπώνονται στην οθόνη
#define NUMBER_OF_THREADS 4 //αριθμός των thread που θα χρησιμοποιεί το omp

int main (int argc, char *argv[]) {
	
	FILE *pFile;
	long file_size;
	char * buffer;
	char * filename;
	size_t result;
	int i, j, freq[N];
	double start_time,stop_time;

	filename = "E.coli";
	pFile = fopen (filename,"rb");
	if (pFile==NULL){
		printf ("File error\n");//,stderr); To stderr χρησιμοποιείται μόνο σε fprintf
		exit (1);
	}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	file_size = ftell (pFile);
	rewind (pFile);
	printf("File size is %ld\n", file_size);
	

	// allocate memory to contain the file:
	buffer = (char*) malloc (sizeof(char)*file_size);
	if (buffer == NULL){
		printf ("Memory error\n");//,stderr);
 		exit (2);
	}

	// copy the slice of the file into the buffer:
	result = fread (buffer,1,file_size,pFile);
	if (result != file_size){
		printf ("Reading error\n");//,stderr); 
		exit (3);
	} 
	
	for (j=0; j<N; j++){
		freq[j]=0;
	}

	omp_set_num_threads(NUMBER_OF_THREADS); //setting the number of threads

	start_time = omp_get_wtime(); //starting the timer
 
	#pragma omp parallel shared(freq,buffer,file_size) private (i)
	{
		#pragma omp for
		for (i=0; i<file_size; i++){
			#pragma omp critical
			freq[buffer[i] - base]++;
		}
	}
				
	stop_time = omp_get_wtime();//end of timer
	
	for (j=0; j<N; j++)
		printf("%d = %d\n", j+base, freq[j]);
	
	printf("Time :%f\n", stop_time-start_time);//printing the results

	fclose (pFile);
	free (buffer);

	return 0;
}
