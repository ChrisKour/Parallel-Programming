#include <stdio.h>
#include <unistd.h>
#include <omp.h>

#define SIZE 5
#define NUMITER 20
#define NUMBER_OF_THREADS 4 

char buffer[SIZE];
int nextin = 0;
int nextout = 0;
int count = 0;
int empty = 1;
int full = 0;

void put(char item)
{
	while(full == 1){			//Εδώ γίνεται έλεγχος ώστε το put να μην μπορεί να προσθέτει αντικείμενα εφόσον το 
		sleep(rand() % 5);		//count == SIZE (δλδ να είναι full) έτσι τα thread 
		#pragma omp flush (full)	//θα πρέπει να περιμένουν μέχρι να αδειάσει κάποια θέση
	}
	buffer[nextin] = item;
	nextin = (nextin + 1) % SIZE;
	count++;
	if(count == SIZE)
		full = 1;
	if(count == 1) // buffer was empty
		empty = 0;
}

void producer()
{
	int i;
	char item;

	for(i = 0; i < NUMITER; i++)
	{
		item = 'A' + (i % 26);
		put(item);
		printf("Producing %c ...\n",item);
		sleep(rand() % 5);
	}
}

char get()
{
	char item;

	while(empty == 1){				//Εδώ γίνεται έλεγχος ώστε το get να μην μπορεί να βγει αντικείμενο εφόσον το 
		sleep(rand() % 5);			//count == 0 (δλδ να είναι empty) έτσι τα thread 
		#pragma omp flush (empty)		//θα πρέπει να περιμένουν μέχρι να μπει κάποιο αντικείμενο από την put
	}
	item = buffer[nextout];
	nextout = (nextout + 1) % SIZE;
	count--;
	if(count == 0) // buffer is empty
		empty = 1;
	if(count == (SIZE-1)) // buffer was full
		full = 0;
	return item;
}

void consumer()
{
	int i;
	char item;

	for(i = 0; i < NUMITER; i++)
	{
		item = get();
		printf("...Consuming %c\n",item);
		sleep(rand() % 5);
	}
}

main()
{
	
	omp_set_num_threads(NUMBER_OF_THREADS);

	#pragma omp parallel shared(full,empty,count,buffer) private(nextin,nextout)
	{
		#pragma omp sections nowait		//εδώ γίνεται χρήση sections και λέμε στο πρόγραμμα ποια θα είναι τα 2 κομμάτια
		{					//που θέλουμε να εκτελεστούν παράλληλα (producer,consumer)
			#pragma omp section			
			producer();

			#pragma omp section
			consumer();
		}
	}
}
