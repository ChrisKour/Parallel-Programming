#include <stdio.h> 
#include <stdlib.h>
#include <omp.h>

#define N 128 //μέγεθος πίνακα
#define base 0 //με βάση 32 εμφανίζονται μόνο οι χαρακτήρες που εκτυπώνονται στην οθόνη
#define NUMBER_OF_THREADS 1 //αριθμός των thread που θα χρησιμοποιεί το omp

int main (int argc, char *argv[]) {
	
	FILE *pFile;
	long file_size;
	char * buffer;
	char * filename;
	size_t result;
	int i, j, local_freq[N],freq[N],chunk;
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
	chunk = 10;
	start_time = omp_get_wtime(); //starting the timer
 
	#pragma omp parallel shared(buffer,file_size,freq,chunk) private (i,local_freq)//Δηλώνουμε την local_freq σαν private ώστε να προσομοιώσουμε την εντολή reduction
	{
		for (i=0; i<N; i++) //Δημιουργούμε μέσα στην parallel τον πίνακα που θα κρατάει τοπικά τις μεταβλητές που αργότερα θα προστεθούν στον κύριο πίνακα freq
			local_freq[i]=0;
		
		#pragma omp for schedule(static,chunk)
		for (i=0; i<file_size; i++){
			local_freq[buffer[i] - base]++;
		}
		
		#pragma omp critical //Χρησιμοποιούμε την critical ώστε να περαστούν τα αποτελέσματα των ιδιωτικών πινάκων ένα ένα μέσα στον κύριο πίνακα freq
        	for(i=0; i<N; i++) {	
            		freq[i] += local_freq[i];
    		}
	}
				
	stop_time = omp_get_wtime();//end of timer
	
	for (j=0; j<N; j++){
		if(freq[j]!=0)
		printf("%d = %d\n", j+base, freq[j]);}
	
	printf("Time :%f and %d\n", stop_time-start_time,NUMBER_OF_THREADS);//printing the results

	fclose (pFile);
	free (buffer);

	return 0;
}
