#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    FILE *fa = fopen("small_matrix_a.txt", "r");
    FILE *fb = fopen("small_matrix_b.txt", "r");
    FILE *fc = fopen("small_result.txt", "w");

    if (fa == NULL || fb == NULL || fc == NULL) {
        printf("Error opening the files\n");

        return 1;
    }
    
    int rows, cols;
    fscanf(fa, "%d %d", &rows, &cols);
    fscanf(fb, "%d %d", &rows, &cols);
    double a[rows][cols];
    double b[rows][cols];
    double c[rows][cols];

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

    #pragma omp parallel for collapse(2)

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }

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

    return 0;
}