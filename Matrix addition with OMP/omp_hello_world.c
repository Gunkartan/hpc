#include <stdio.h> 
#include <omp.h> 

int main (int argc, char *argv[]) { 
	int nt, rank;
	
	//without this, it will use max_thread as number of thread
	int num_thread;
	sscanf(argv[1], "%d", &num_thread);
	omp_set_num_threads(num_thread);
	#pragma omp parallel private(nt, rank) 
	{ 
    		rank = omp_get_thread_num(); 
    		printf("Hello World from thread = %d\n", rank); 

    		if (rank == 0) { 
        		nt = omp_get_num_threads(); 
        		printf("Number of threads = %d\n", nt); 
    		} 
	} 
} 
