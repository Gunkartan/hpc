#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    FILE *fa = fopen("large_matrix_a.txt", "r");
    FILE *fb = fopen("large_matrix_b.txt", "r");
    FILE *fc = fopen("large_result.txt", "w");

    if (fa == NULL || fb == NULL || fc == NULL) {
        printf("Error opening the files\n");

        return 1;
    }

    int rows, cols;
    fscanf(fa, "%d %d", &rows, &cols);
    fscanf(fb, "%d %d", &rows, &cols);
    double **a = malloc(rows * sizeof(double *));
    double **b = malloc(rows * sizeof(double *));
    double **c = malloc(rows * sizeof(double *));

    for (int i = 0; i < rows; i++) {
        a[i] = malloc(cols * sizeof(double));
        b[i] = malloc(cols * sizeof(double));
        c[i] = malloc(cols * sizeof(double));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(fa, "%lf", &a[i][j]);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(fb, "%lf", &b[i][j]);
        }
    }

    double start = omp_get_wtime();

    #pragma omp parallel for collapse(2)

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }

    double end = omp_get_wtime();
    printf("The execution time is %f seconds\n", end - start);
    fprintf(fc, "%d %d\n", rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fc, "%.3f ", c[i][j]);
        }

        fprintf(fc, "\n");
    }

    fclose(fa);
    fclose(fb);
    fclose(fc);

    for (int i = 0; i < rows; i++) {
        free(a[i]);
        free(b[i]);
        free(c[i]);
    }

    free(a);
    free(b);
    free(c);

    return 0;
}