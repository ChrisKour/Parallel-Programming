#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000 //Μέγεθος πινάκων
#define NUMBER_OF_THREADS 4 //Αριθμός threads που θα χρησιμοποιήσουμε

int main(int argc, char *argv[])
{
	int i,j,flag[N]; //Δήλωση πίνακα flag
	double sum,start,stop;
	double x[N],b[N],a[N][N],temp; //Δήλωση μεταβλητής temp

	for(i=0;i<N;i++)
	{
		flag[i]=0; //Αρχικοποίηση πίνακα flag
		x[i]=0.0;
		b[i]=i*3;
		a[i][i]=2*i+1;
		for(j=0;j<i;j++)
			a[i][j]=i+j+1;
	}

	start=omp_get_wtime();
	omp_set_num_threads(NUMBER_OF_THREADS);

	#pragma omp parallel for schedule(static) private(i,j,sum)
	for(i=0;i<N;i++)
	{
		if(i!=0) //Ελέγχουμε αν είναι η πρώτη φορά
		{
			while(flag[i-1] != 1){ //Αν δεν είναι το thread ελέγχει αν το προηγούμενο flag έχει αλλάξει, δλδ να είναι 1. Αν δεν είναι σημαίνει ότι η μεταβλητή που χρειάζεται για να βγάλει σωστά αποτελέσματα ακόμα δεν έχει αλλάξει, οπότε περιμένει μέχρι να συμβεί αυτό.
				sleep(rand() % 5); //Η εντολή sleep μπορεί να παραληφθεί και έτσι να μειώσουμε δραστικά τον χρόνο εκτέλεσης
			}
		}
		temp=x[i]; //Περνάμε την αρική τιμή της χ[ι] (0) σε μια μεταβλητή temp ώστε να μπορούμε να κάνουμε σύγκριση αργότερα
		sum=0;
		for(j=0;j<i;j++)
			sum=sum+a[i][j]*x[j];
		x[i]=(b[i]-sum)/a[i][i];
		if((i == 0) ||  x[i] != temp)
		{ //Ελέγχουμε αν είναι η πρώτη φορά όπου δεν γίνεται κάποια αλλαγή στο χ[0] (παραμένει 0), ή το χ[ι] έχει αλλάξει σε σχέση με το temp οπότε μπορούμε να δώσουμε σήμα στα επόμενα threads να συνεχίσουν με τις εργασίες τους
			flag[i]=1;
			#pragma omp flush (flag)
		}
	}

	stop=omp_get_wtime();

	printf("Time: %f\n",stop-start);

	for(i=0;i<N;i++)
		printf("%f \n",x[i]);

	return 0;
}
