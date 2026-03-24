#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rows, cols, rows_b, cols_b;
    double *a = NULL, *b = NULL, *c = NULL;

    if (rank == 0) {
        FILE *fa = fopen("matAlarge.txt", "r");
        FILE *fb = fopen("matBlarge.txt", "r");

        if (fa == NULL || fb == NULL) {
            printf("Error opening input files\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fscanf(fa, "%d %d", &rows, &cols);
        fscanf(fb, "%d %d", &rows_b, &cols_b);

        if (rows != rows_b || cols != cols_b) {
            printf("Matrix dimensions do not match\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        a = malloc(rows * cols * sizeof(double));
        b = malloc(rows * cols * sizeof(double));
        c = malloc(rows * cols * sizeof(double));

        for (int i = 0; i < rows * cols; i++) {
            fscanf(fa, "%lf", &a[i]);
            fscanf(fb, "%lf", &b[i]);
        }

        fclose(fa);
        fclose(fb);
    }

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    int base = rows / size;
    int extra = rows % size;
    int local_rows = (rank < extra) ? base + 1 : base;
    double *a_local, *b_local, *c_local;

    if (rank == 0) {
        a_local = malloc(local_rows * cols * sizeof(double));
        b_local = malloc(local_rows * cols * sizeof(double));
        c_local = malloc(local_rows * cols * sizeof(double));
        MPI_Request *reqs = malloc(3 * size * sizeof(MPI_Request));
        int req_count = 0;
        int offset = 0;

        for (int p = 0; p < size; p++) {
            int rows_p = (p < extra) ? base + 1 : base;

            if (p == 0) {
                for (int i = 0; i < rows_p * cols; i++) {
                    a_local[i] = a[offset + i];
                    b_local[i] = b[offset + i];
                }
            } else {
                MPI_Isend(&rows_p, 1, MPI_INT, p, 0, MPI_COMM_WORLD, &reqs[req_count++]);
                MPI_Isend(a + offset, rows_p * cols, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &reqs[req_count++]);
                MPI_Isend(b + offset, rows_p * cols, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &reqs[req_count++]);
            }

            offset += rows_p * cols;
        }

        MPI_Waitall(req_count, reqs, MPI_STATUSES_IGNORE);
        free(reqs);
    } else {
        MPI_Request reqs[3];
        MPI_Irecv(&local_rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Wait(&reqs[0], MPI_STATUS_IGNORE);
        a_local = malloc(local_rows * cols * sizeof(double));
        b_local = malloc(local_rows * cols * sizeof(double));
        c_local = malloc(local_rows * cols * sizeof(double));
        MPI_Irecv(a_local, local_rows * cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &reqs[1]);
        MPI_Irecv(b_local, local_rows * cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &reqs[2]);
        MPI_Waitall(2, &reqs[1], MPI_STATUSES_IGNORE);
    }

    for (int i = 0; i < local_rows * cols; i++) {
        c_local[i] = a_local[i] + b_local[i];
    }

    if (rank == 0) {
        MPI_Request *reqs = malloc(size * sizeof(MPI_Request));
        int req_count = 0;
        int offset = 0;

        for (int p = 0; p < size; p++) {
            int rows_p = (p < extra) ? base + 1 : base;

            if (p == 0) {
                for (int i = 0; i < rows_p * cols; i++) {
                    c[offset + i] = c_local[i];
                }
            } else {
                MPI_Irecv(c + offset, rows_p * cols, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &reqs[req_count++]);
            }

            offset += rows_p * cols;
        }

        MPI_Waitall(req_count, reqs, MPI_STATUSES_IGNORE);
        free(reqs);
    } else {
        MPI_Request req;
        MPI_Isend(c_local, local_rows * cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("The program ran for %f seconds\n", end - start);
    }

    if (rank == 0) {
        FILE *fc = fopen("result.txt", "w");

        if (fc == NULL) {
            printf("Error opening output file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fprintf(fc, "%d %d\n", rows, cols);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                fprintf(fc, "%.1f", c[i * cols + j]);

                if (j < cols - 1) {
                    fprintf(fc, " ");
                }
            }

            fprintf(fc, "\n");
        }

        fclose(fc);
        free(a);
        free(b);
        free(c);
    }

    free(a_local);
    free(b_local);
    free(c_local);
    MPI_Finalize();

    return 0;
}