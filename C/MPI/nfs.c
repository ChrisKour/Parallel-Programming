#include <stdio.h> 
#include <stdlib.h>
#include "mpi.h" 
#define N 128
#define base 0

int main (int argc, char *argv[]) 
{
	FILE *pFile;
	long file_size; //Συνολικό μέγεθος αχείου
	char * buffer;	//Πίνακας για αποθήκευση μέρους του αρχείου 
	char * filename;
	size_t result;
	int i, j, freq[N];
	int numtasks,rank,rc,chunk,totalfreq[N]; //Μεταβλητές MPI
	double start,stop; //Μεταβλητές χρόνου

	rc = MPI_Init(&argc,&argv); //Εκκίνηση MPI
   	if (rc != MPI_SUCCESS) 
	{	//Έλεγχος σωστής εκκίνησης MPI
     		printf ("Error starting MPI program. Terminating.\n");
     		MPI_Abort(MPI_COMM_WORLD, rc);
     	}

   	MPI_Comm_size(MPI_COMM_WORLD,&numtasks); //Προσδιορισμός αριθμού διεργασιών
   	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 //Προσδιορισμός αναγνωριστικού διεργασιών

	if(rank ==0) //Εκκίνηση χρόνου από τη master διεργασία
		start = MPI_Wtime();

	filename = "E.coli"; //Όλες οι διεργασίες ανοίγουν το αρχείο
	pFile = fopen ( filename , "rb" );
	if (pFile==NULL) 
	{
		printf ("File error\n"); 
		exit (1);
	}

	if(rank ==0) 
	{	//Η master ελέγχει το συνολικό μέγεθος του αρχείου
		fseek (pFile , 0 , SEEK_END);
		file_size = ftell (pFile);
		rewind (pFile);
		printf("Total file size is %ld\n", file_size);
	}

	MPI_Bcast(&file_size,1,MPI_LONG,0,MPI_COMM_WORLD); //Η master διεργασία κάνει broadcast το συνολικό μέγεθος του αρχείου

	chunk = file_size/numtasks; //Υπολογισμός chunk

	fseek (pFile , rank*chunk , SEEK_SET); //Η κάθε διεργασία αποκτάει διαφορετικό σημείο εκκίνησης για εγγραφή στο buffer

	//Η κάθε διεργασία δημιουργεί πίνακα buffer για αποθήκευση μέρους του αρχείου
	buffer = (char*) malloc (sizeof(char)*chunk);
	if (buffer == NULL) 
	{
		printf ("Memory error\n"); 
		exit (2);
	}

	// copy the slice of the file into the buffer:
	result = fread (buffer,1,chunk,pFile);
	if (result != chunk) 
	{
		printf ("Reading error\n"); 
		exit (3);
	} 
	
	for (j=0; j<N; j++) //Αρχικοποίηση τοπικού πίνακα freq
		freq[j]=0;
	
	for (i=0; i<chunk; i++) //Υπολογισμός συχνότητας από κάθε διεργασία
		freq[buffer[i] - base]++;		

	if(rank == 0) //Αρχικοποίηση totalfreq από τη master διεργασία
		for (j=0; j<N; j++)
			totalfreq[j]=0;

	MPI_Reduce(freq,totalfreq,N,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD); //Κάνουμε reduce τα περιεχόμενα των πινάκων freq στον totalfreq της master διεργασίας

	if(rank==0) //Εκτύπωση αποτελεσμάτων από τη master διεργασία
		for (j=0; j<N; j++)
			printf("%d = %d\n", j+base, totalfreq[j]);	

	fclose (pFile); //Κλείσιμο αρχείου και απελευθέρωση πινάκων buffer από κάθε διεργασία
	free (buffer);

	if(rank==0)
	{	//Τερματισμός χρόνου και εκτύπωσή του από τη master διεργασία
		stop = MPI_Wtime();
		printf("Total Time elapsed: %f\n",stop-start);
	}

	MPI_Finalize();
	return 0;
}
