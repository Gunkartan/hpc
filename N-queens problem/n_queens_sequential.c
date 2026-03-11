#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void write_solution(FILE *out, int board[], int n) {
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

void solve_n_queens(int board[], int col, int n, FILE *out, long long *solutions) {
    if (col == n) {
        (*solutions)++;
        write_solution(out, board, n);

        return;
    }

    for (int row = 0; row < n; row++) {
        if (is_safe(board, col, row)) {
            board[col] = row;
            solve_n_queens(board, col + 1, n, out, solutions);
        }
    }
}

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    if (input == NULL || output == NULL) {
        printf("File error\n");

        return 1;
    }

    int n;
    long long solutions = 0;

    if (fscanf(input, "%d", &n) != 1) {
        printf("Invalid input\n");

        return 1;
    }

    int *board = malloc(n * sizeof(int));

    if (board == NULL) {
        printf("Memory allocation failed\n");

        return 1;
    }

    double start = omp_get_wtime();
    solve_n_queens(board, 0, n, output, &solutions);
    double end = omp_get_wtime();
    printf("The total number of solutions with N being %d is %lld\n", n, solutions);
    printf("The total runtime is %f seconds\n", end - start);
    fprintf(output, "The total number of solutions with N being %d is %lld\n", n, solutions);
    free(board);
    fclose(input);
    fclose(output);

    return 0;
}