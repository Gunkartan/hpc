#include <stdio.h> 
#include <omp.h> 
#include <mpi.h>

int main (int argc, char *argv[]) { 
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
            processor_name, myrank, nprocs);

	int nt, rank;
    int num_thread;
	sscanf(argv[1], "%d", &num_thread);
	omp_set_num_threads(num_thread); 
	#pragma omp parallel private(nt, rank) 
	{ 
    		rank = omp_get_thread_num(); 
    		printf("Hello World from thread = %d of %s (rank %d)\n", rank, processor_name, myrank); 

    		if (rank == 0) { 
        		nt = omp_get_num_threads(); 
        		printf("Number of threads = %d\n", nt); 
    		} 
	} 

    MPI_Finalize();
    return 0;
} 
