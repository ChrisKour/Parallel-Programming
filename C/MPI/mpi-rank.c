#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define N 1000

int main(int argc, char *argv[])
{
   	int x[N], y[N];
   	int i, j, my_num, my_place;
	int numtasks,rank,rc,chunk; //Μεταβλητές MPI
	double start,stop; //Μεταβλητές χρόνου
	int buflen = 512;
	char name[buflen];
   
	rc = MPI_Init(&argc,&argv); //Εκκίνηση MPI
   	if (rc != MPI_SUCCESS) 
	{	//Έλεγχος σωστής εκκίνησης MPI
     		printf ("Error starting MPI program. Terminating.\n");
     		MPI_Abort(MPI_COMM_WORLD, rc);
     	}

   	MPI_Comm_size(MPI_COMM_WORLD,&numtasks); //Προσδιορισμός αριθμού διεργασιών
   	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 //Προσδιορισμός αναγνωριστικού διεργασιών

	if(rank == 0)
	{ 	//Αρχικοποίηση πίνακα από τη master διεργασία
		start = MPI_Wtime();
   		for (i=0; i<N; i++)
			x[i] = N - i;
	}
	
	MPI_Bcast(x,N,MPI_INT,0,MPI_COMM_WORLD);//Κάνουμε broadcast τον πίνακα χ

	for(i=0;i<N;i++) //Αρχικοποίηση y σε όλες τις διεργασίες
		y[i]=0;

	chunk = N/numtasks; //Υπολογισμός chunk
	
   	for (j=(rank*chunk); j<((rank*chunk)+chunk); j++) //Ταξινόμηση του ανάλογου chunk
	{
		my_num = x[j];
     		my_place = 0;
     		for (i=0; i<N; i++)
			if ( my_num > x[i] ) 
				my_place++;
     		y[my_place] = my_num;
   	}  

	MPI_Reduce(y,x,N,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD); //Κάνουμε reduce τα περιεχόμενα των πινάκων y στον χ του master
	
   	/*if(rank ==0) //Εκτύπωση αποτελεσμάτων
   		for (i=0; i<N; i++) 
			printf("%d\n", x[i]);*/
	
	gethostname(name,buflen);

	printf("Hello, world. I am %d of %d on %s\n", rank,numtasks,name);

	if(rank==0)
	{
		stop = MPI_Wtime();
		printf("Time elapsed %f\n",stop-start);
	}
	
	MPI_Finalize();	
   	return 0;
}
