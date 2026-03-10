#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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

void solve_n_queens(int board[], int col, int n, long long *solutions) {
    if (col == n) {
        (*solutions)++;

        return;
    }

    for (int row = 0; row < n; row++) {
        if (is_safe(board, col, row)) {
            board[col] = row;
            solve_n_queens(board, col + 1, n, solutions);
        }
    }
}

int main() {
    int n;
    printf("Enter N: ");
    scanf("%d", &n);
    int *board = malloc(n * sizeof(int));

    if (board == NULL) {
        printf("Memory allocation failed\n");

        return 1;
    }

    long long solutions = 0;
    double start = omp_get_wtime();
    solve_n_queens(board, 0, n, &solutions);
    double end = omp_get_wtime();
    printf("The total number of solutions with N being %d is %lld\n", n, solutions);
    printf("The total runtime is %f seconds\n", end - start);
    free(board);

    return 0;
}