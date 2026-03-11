#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

void write_solution(int board[], int n, int rank, FILE *out) {
    char buffer[1024];
    int pos = 0;
    pos += sprintf(buffer + pos, "[");

    for (int row = 0; row < n; row++) {
        pos += sprintf(buffer + pos, "\"");

        for (int col = 0; col < n; col++) {
            if (board[col] == row) {
                pos += sprintf(buffer + pos, "Q");
            } else {
                pos += sprintf(buffer + pos, ".");
            }
        }

        pos += sprintf(buffer + pos, "\"");

        if (row < n - 1) {
            pos += sprintf(buffer + pos, ",");
        }
    }

    pos += sprintf(buffer + pos, "]\n");

    if (rank == 0) {
        fprintf(out, "%s", buffer);
    } else {
        #pragma omp critical

        {
            MPI_Send(buffer, pos + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
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
        write_solution(board, n, rank, out);

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
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (provided < MPI_THREAD_MULTIPLE) {
        if (rank == 0) {
            printf("MPI does not support the required threading level\n");
        }

        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int n;

    if (rank == 0) {
        FILE *input = fopen("input.txt", "r");

        if (input == NULL) {
            printf("File error\n");
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
    FILE *output = NULL;

    if (rank == 0) {
        output = fopen("output.txt", "w");
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    #pragma omp parallel for reduction(+:local_solutions)

    for (int row = rank; row < n; row += size) {
        int *board = malloc(n * sizeof(int));

        if (board == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        board[0] = row;
        long long thread_solutions = 0;
        solve_n_queens(board, 1, n, rank, output, &thread_solutions);
        local_solutions += thread_solutions;

        free(board);
    }

    long long total_solutions = 0;
    MPI_Reduce(&local_solutions, &total_solutions, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank != 0) {
        MPI_Send("", 0, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }
    
    if (rank == 0) {
        char buffer[1024];
        MPI_Status status;

        for (int i = 1; i < size; i++) {
            while (1) {
                MPI_Recv(buffer, sizeof(buffer), MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                if (status.MPI_TAG == 1) {
                    break;
                }

                fprintf(output, "%s", buffer);
            }
        }

        printf("The total number of solutions with N being %d is %lld\n", n, total_solutions);
        fprintf(output, "The total number of solutions with N being %d is %lld\n", n, total_solutions);
        fclose(output);
    }

    double end = MPI_Wtime();

    if (rank == 0) {
        printf("The total runtime is %f seconds\n", end - start);
    }

    MPI_Finalize();

    return 0;
}