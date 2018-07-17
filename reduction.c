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
	int i, j, freq[N],chunk;
	int suma,sumc,sumg,sumt; // δήλωση μεταβλητών που θα κρατάνε το σύνολο των φορών που εμφανίζεται ο κάθε χαρακτήρας
	double start_time,stop_time; //δήλωση μεταβλητών που θα κρατάνε τον χρόνο

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

	chunk=10;
	suma=0; //Μηδενισμός των μεταβλητών που θα χρησιμοποιηθούν για να κρατήσουν τον αριθμό εμφάνισης
	sumc=0; //κάθε γράμματος
	sumg=0;
	sumt=0;

	start_time = omp_get_wtime(); //starting the timer
 
	#pragma omp parallel for default(shared) private(i) schedule(static,chunk) reduction(+:suma,sumc,sumg,sumt) 
	for (i=0; i<file_size; i++){ //Η χρήση της reduction για c δεν υποστηρίζει πίνακες και εφόσον 
		if(buffer[i] == 'a') //ο αριθμός των γραμμάτων που χρησιμοποιούνται είναι τόσο μικρός
			suma++;      //σε αυτό το αρχείο χρησιμοποιώ 4 μεταβλητές για κάθε περίπτωση
		if(buffer[i] == 'c') //εμφάνισης ενός γράμματος
 			sumc++;
		if(buffer[i] == 'g')
			sumg++;
		if(buffer[i] == 't')
			sumt++;
	}
				
	stop_time = omp_get_wtime();//end of timer
	
	printf("(a) appears: %d times\n",suma);//Εκτύπωση αποτελεσμάτων
	printf("(c) appears: %d times\n",sumc);
	printf("(g) appears: %d times\n",sumg);
	printf("(t) appears: %d times\n",sumt);
	
	printf("Time :%f\n", stop_time-start_time);//Εκτύπωση χρόνου

	fclose (pFile);
	free (buffer);

	return 0;
}
