#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    long long int total_points = 1000000;

    if (argc > 1) {
        total_points = atoll(argv[1]);
    }

    int rank, size;
    long long int local_points, local_inside = 0, global_inside = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("Rank %d running on %s\n", rank, hostname);
    local_points = total_points / size;

    if (rank == size - 1) {
        local_points += total_points % size;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    unsigned int seed = time(NULL) + rank;

    for (long long int i = 0; i < local_points; i++) {
        double x = rand_r(&seed) / (double)RAND_MAX;
        double y = rand_r(&seed) / (double)RAND_MAX;

        if (x * x + y * y <= 1.0) {
            local_inside++;
        }
    }

    MPI_Reduce(&local_inside, &global_inside, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("The total points are %lld\n", total_points);
        double pi_estimate = 4.0 * global_inside / total_points;
        printf("The estimated pi is %.10f\n", pi_estimate);
        printf("The execution time is %.6f seconds\n", end - start);
    }

    MPI_Finalize();

    return 0;
}