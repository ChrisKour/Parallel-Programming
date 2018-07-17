#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000 //mege8os problimatos
#define NUMBER_OF_THREADS 2 //ari8mos threads

int main()
{
  	int x[N], y[N]; //dhlwsh metablhtwn synarthshs
  	int i, j, my_num, my_place, num_thread;
	double start_time,stop_time; //dhlwsh metablhtwn xronou
   
	omp_set_num_threads(NUMBER_OF_THREADS); //orismos ari8mou thread

	start_time = omp_get_wtime();

	#pragma omp parallel shared(x) private(i) //parallhlopoihsh ths arxikopoihshs
  	{
		#pragma omp for
		for (i=0; i<N; i++) //basikos kwdikas arxikopoihshs
			x[i] = N - i;
	}
	
	stop_time = omp_get_wtime();
	
	printf("Initialization time = %f for N = %d and %d thread(s)\n", stop_time-start_time, N, NUMBER_OF_THREADS); //ektypwsh xronou arxikopoihshs

	start_time = omp_get_wtime();//ekinhsh xronou gia to sort

	#pragma omp parallel shared(x,y) private(i,my_num,my_place) //parallhlopoihsh toy sort
  	{
  		#pragma omp for
  		for (j=0; j<N; j++) { //basikos kwdikas rank sort
     			my_num = x[j];
     			my_place = 0;
     			for (i=0; i<N; i++)
				if ( my_num > x[i] ) 
					my_place++;
				else if(my_num == x[i] && j > i)//ginetai elegxos gia isous arithmous kai an autoi briskontai se megalyterh 8esh etsi wste o ka8enas na lambanei ypopsh mono osous perasan
					my_place++;
     			y[my_place] = my_num;
   		}  
  	}
   
	stop_time = omp_get_wtime();

	printf("Sorting time = %f for N = %d and %d thread(s)\n", stop_time-start_time, N, NUMBER_OF_THREADS);
			
   	return 0;
}
