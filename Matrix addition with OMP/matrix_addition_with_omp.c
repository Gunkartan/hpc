#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    FILE *fa = fopen("small_matrix_a.txt", "r");
    FILE *fb = fopen("small_matrix_b.txt", "r");
    FILE *fc = fopen("small_result.txt", "w");
    int rows, cols;
    fscanf(fa, "%d %d", &rows, &cols);
    fscanf(fb, "%d %d", &rows, &cols);
    int a[rows][cols];
    int b[rows][cols];
    int c[rows][cols];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(fa, "%d", &a[i][j]);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(fb, "%d", &b[i][j]);
        }
    }

    #pragma omp parallel for collapse(2)

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }

    fprintf(fc, "%d %d\n", rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fc, "%d", c[i][j]);
        }

        fprintf(fc, "\n");
    }

    fclose(fa);
    fclose(fb);
    fclose(fc);

    return 0;
}