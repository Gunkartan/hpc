#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

void write_solution(int board[], int n, FILE *out) {
    #pragma omp critical

    {
        fprintf(out, "[");

        for (int row = 0; row < n; row++) {
            fprintf(out, "\"");

            for (int col = 0; col < n; col++) {
                if (board[col] == row) {
                    fprintf(out, "Q");
                } else {
                    fprintf(out, ".");
                }
            }

            fprintf(out, "\"");

            if (row < n - 1) {
                fprintf(out, ",");
            }
        }

        fprintf(out, "]\n");
    }
}

int is_safe(int board[], int col, int row) {
    for (int i = 0; i < col; i++) {
        if (board[i] == row) {
            return 0;
        }

        if (abs(col - i) == abs(row - board[i])) {
            return 0;
        }
    }

    return 1;
}

void solve_n_queens(int board[], int col, int n, int rank, FILE *out, long long *solutions) {
    if (col == n) {
        (*solutions)++;
        write_solution(board, n, out);

        return;
    }

    for (int row = 0; row < n; row++) {
        if (is_safe(board, col, row)) {
            board[col] = row;
            solve_n_queens(board, col + 1, n, rank, out, solutions);
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n;

    if (rank == 0) {
        FILE *input = fopen("input.txt", "r");

        if (input == NULL) {
            printf("Input file error\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (fscanf(input, "%d", &n) != 1) {
            printf("Invalid input\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fclose(input);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    long long local_solutions = 0;
    FILE *output;
    char filename[64];
    sprintf(filename, "output_%d.txt", rank);
    output = fopen(filename, "w");

    if (output == NULL) {
        printf("Output file error\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    #pragma omp parallel reduction(+:local_solutions)

    {
        int *board = malloc(n * sizeof(int));

        #pragma omp for schedule(dynamic)

        for (int row = rank; row < n; row += size) {
            board[0] = row;
            solve_n_queens(board, 1, n, rank, output, &local_solutions);
        }

        free(board);
    }
    long long total_solutions = 0;
    MPI_Reduce(&local_solutions, &total_solutions, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("The total number of solutions with N being %d is %lld\n", n, total_solutions);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    fclose(output);
    double end = MPI_Wtime();

    if (rank == 0) {
        FILE *final = fopen("output.txt", "w");

        if (final == NULL) {
            printf("Final file error\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        char buffer[1024];

        for (int r = 0; r < size; r++) {
            char filename[64];
            sprintf(filename, "output_%d.txt", r);
            FILE *part = fopen(filename, "r");

            if (part == NULL) {
                printf("Part file error\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            while (fgets(buffer, sizeof(buffer), part) != NULL) {
                fputs(buffer, final);
            }

            fclose(part);
            remove(filename);
        }

        fclose(final);
    }

    if (rank == 0) {
        printf("The total runtime is %f seconds\n", end - start);
    }

    MPI_Finalize();

    return 0;
}