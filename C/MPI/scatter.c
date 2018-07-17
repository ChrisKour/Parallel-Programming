#include <stdio.h> 
#include <stdlib.h> 
#include "mpi.h"
#define N 128
#define base 0

int main (int argc, char *argv[]) 
{	
	FILE *pFile;
	long file_size; //Συνολικό μέγεθος αχείου
	char * buffer;  //Πίνακας του master για αποθήκευση αρχείου 
	char * microbuffer; //Πίνακας για κάθε άλλη διεργασία για την αποθήκευση μέρους του αρχείου
	char * filename;
	size_t result;
	int i, j, freq[N];
	int numtasks,rank,rc,chunk,totalfreq[N]; //Μεταβλητές MPI
	double start,stop; //Μεταβλητές χρόνου

	rc = MPI_Init(&argc,&argv); 	//Εκκίνηση MPI
   	if (rc != MPI_SUCCESS) 
	{	//Έλεγχος σωστής εκκίνησης MPI
     		printf ("Error starting MPI program. Terminating.\n");
     		MPI_Abort(MPI_COMM_WORLD, rc);
     	}

   	MPI_Comm_size(MPI_COMM_WORLD,&numtasks); //Προσδιορισμός αριθμού διεργασιών
   	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 //Προσδιορισμός αναγνωριστικού διεργασιών

	if(rank == 0) //Το ακόλουθο κομμάτι εκτελείται μόνο από τη master διεργασία
	{
		start = MPI_Wtime(); //Εκκίνηση χρόνου

		filename = "E.coli";
		pFile = fopen ( filename , "rb" );
		if (pFile==NULL) 
		{
			printf ("File error\n"); 
			exit (1);
		}

		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		file_size = ftell (pFile);
		rewind (pFile);
		printf("file size is %ld\n", file_size);
	
		// allocate memory to contain the file:
		buffer = (char*) malloc (sizeof(char)*file_size);
		if (buffer == NULL) 
		{
			printf ("Memory error\n");
			exit (2);
		}

		// copy the slice of the file into the buffer:
		result = fread (buffer,1,file_size,pFile);
		if (result != file_size) 
		{
			printf ("Reading error\n"); 
			exit (3);
		} 
	}

	MPI_Bcast(&file_size,1,MPI_LONG,0,MPI_COMM_WORLD); //Η master διεργασία κάνει broadcast το συνολικό μέγεθος του αρχείου

	chunk = file_size/numtasks; //Υπολογισμός chunk
	
	microbuffer = (char*) malloc (sizeof(char)*chunk); //Δημιουργία δυναμικών πινάκων σε κάθε διεργασία

	MPI_Scatter(buffer,chunk,MPI_CHAR,microbuffer,chunk,MPI_CHAR,0,MPI_COMM_WORLD); //Κάνουμε scatter τα περιεχόμενα του buffer στα microbuffers

	for (j=0; j<N; j++) //Αρχικοποίηση τοπικού πίνακα freq
		freq[j]=0;

	for (i=0; i<chunk; i++) //Υπολογισμός συχνότητας από κάθε διεργασία
		freq[microbuffer[i] - base]++;

	if(rank == 0) //Αρχικοποίηση totalfreq από τη master διεργασία
		for (j=0; j<N; j++)
			totalfreq[j]=0;	

	MPI_Reduce(freq,totalfreq,N,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD); //Κάνουμε reduce τα περιεχόμενα των πινάκων freq στον totalfreq της master διεργασίας

	if(rank==0) //Εκτύπωση αποτελεσμάτων από τη master διεργασία
		for (j=0; j<N; j++)
			printf("%d = %d\n", j+base, totalfreq[j]);	

	if(rank==0) //Κλείσιμο του αρχείου από τη master διεργασία και εκτύπωση χρόνου
	{
		fclose (pFile);
		free (buffer);
		stop = MPI_Wtime();
		printf("Total Time elapsed: %f\n",stop-start);
	}

	MPI_Finalize();
	return 0;
}
