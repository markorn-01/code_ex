#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define N 1024  // Matrix size (n x n)
#define NUM_LAYOUTS 4

typedef enum { ROW_MAJOR, COL_MAJOR } Layout;

const char *layout_str[] = {
    "Row", "Column"
};

void generate_matrix(double *matrix, int n) {
    for (int i = 0; i < n * n; i++) {
        matrix[i] = (double)(rand() % 10);
    }
}

double checksum(double *matrix, int n) {
    double sum = 0.0;
    for (int i = 0; i < n * n; i++) {
        sum += matrix[i];
    }
    return sum;
}

void matmul_parallel(double *A, double *B, double *C, int n, Layout la, Layout lb, Layout lc) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                double a_val = (la == ROW_MAJOR) ? A[i * n + k] : A[k * n + i];
                double b_val = (lb == ROW_MAJOR) ? B[k * n + j] : B[j * n + k];
                sum += a_val * b_val;
            }
            if (lc == ROW_MAJOR)
                C[i * n + j] = sum;
            else
                C[j * n + i] = sum;
        }
    }
}

int main() {
    srand(time(NULL));
    int n = N;
    int threads[] = {1, 2, 4, 8};
    int num_threads = sizeof(threads) / sizeof(threads[0]);

    double *A = malloc(n * n * sizeof(double));
    double *B = malloc(n * n * sizeof(double));
    double *C = malloc(n * n * sizeof(double));
    double *C_base = malloc(n * n * sizeof(double));

    generate_matrix(A, n);
    generate_matrix(B, n);

    for (int la = 0; la <= 1; la++) {
        for (int lb = 0; lb <= 1; lb++) {
            for (int lc = 0; lc <= 1; lc++) {
                printf("Layout: A = %s, B = %s, C = %s\n", layout_str[la], layout_str[lb], layout_str[lc]);
                double base_time = -1;

                for (int t = 0; t < num_threads; t++) {
                    omp_set_num_threads(threads[t]);
                    memset(C, 0, n * n * sizeof(double));

                    double start = omp_get_wtime();
                    matmul_parallel(A, B, C, n, la, lb, lc);
                    double end = omp_get_wtime();

                    double time = end - start;

                    if (threads[t] == 1) {
                        memcpy(C_base, C, n * n * sizeof(double));
                        base_time = time;
                    } else {
                        // Check correctness
                        for (int i = 0; i < n * n; i++) {
                            if (fabs(C[i] - C_base[i]) > 1e-6) {
                                printf("Error: mismatch in results with %d threads\n", threads[t]);
                                break;
                            }
                        }
                    }

                    double speedup = base_time / time;
                    double efficiency = speedup / threads[t];

                    printf("  Threads: %2d | Time: %8.4f s | Speedup: %6.2f | Efficiency: %5.2f%%\n",
                           threads[t], time, speedup, efficiency * 100);
                }
                printf("\n");
            }
        }
    }

    free(A);
    free(B);
    free(C);
    free(C_base);
    return 0;
}
