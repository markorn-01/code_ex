#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>

#define BASE 256           // 8-bit chunks
#define BITS 64
#define PASSES (BITS / 8)  // 8 passes for 64-bit integers
#define MAX_THREADS 8

// Generate random non-negative 64-bit integers
void generate_random_array(int64_t *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = ((int64_t)rand() << 32) | rand();
    }
}

// Check if array is sorted
int is_sorted(int64_t *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return 0;
    }
    return 1;
}

// Radix sort for 64-bit integers using OpenMP and 8-bit digit buckets
void radix_sort_parallel(int64_t *arr, int n) {
    int64_t *output = malloc(n * sizeof(int64_t));
    int num_threads = MAX_THREADS;

    for (int pass = 0; pass < PASSES; pass++) {
        int shift = pass * 8;

        // Step 1: Create local histograms per thread
        int local_hist[MAX_THREADS][BASE];
        memset(local_hist, 0, sizeof(local_hist));

        #pragma omp parallel num_threads(num_threads)
        {
            int tid = omp_get_thread_num();
            int start = tid * n / num_threads;
            int end = (tid + 1) * n / num_threads;

            for (int i = start; i < end; i++) {
                int digit = (arr[i] >> shift) & (BASE - 1);
                local_hist[tid][digit]++;
            }
        }

        // Step 2: Combine histograms into global prefix sum
        int global_hist[BASE] = {0};
        for (int d = 0; d < BASE; d++) {
            for (int t = 0; t < num_threads; t++) {
                global_hist[d] += local_hist[t][d];
            }
        }

        int prefix_sum[BASE];
        prefix_sum[0] = 0;
        for (int i = 1; i < BASE; i++) {
            prefix_sum[i] = prefix_sum[i - 1] + global_hist[i - 1];
        }

        // Step 3: Calculate local offsets from prefix sums
        int local_offsets[MAX_THREADS][BASE];
        for (int t = 0; t < num_threads; t++) {
            for (int d = 0; d < BASE; d++) {
                local_offsets[t][d] = prefix_sum[d];
                for (int tt = 0; tt < t; tt++) {
                    local_offsets[t][d] += local_hist[tt][d];
                }
            }
        }

        // Step 4: Place elements in output array
        #pragma omp parallel num_threads(num_threads)
        {
            int tid = omp_get_thread_num();
            int start = tid * n / num_threads;
            int end = (tid + 1) * n / num_threads;
            int *offset = local_offsets[tid];

            for (int i = start; i < end; i++) {
                int digit = (arr[i] >> shift) & (BASE - 1);
                output[offset[digit]++] = arr[i];
            }
        }

        // Step 5: Copy output back to input for next pass
        memcpy(arr, output, n * sizeof(int64_t));
    }

    free(output);
}

int main() {
    const int sizes[] = {1 << 16, 1 << 18, 1 << 20, 1 << 22};  // 64K to 4M
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int64_t *arr = malloc(n * sizeof(int64_t));
        generate_random_array(arr, n);

        double start = omp_get_wtime();
        radix_sort_parallel(arr, n);
        double end = omp_get_wtime();

        printf("Size: %d, Sorted: %s, Time: %.6f s\n", n,
               is_sorted(arr, n) ? "Yes" : "No", end - start);

        free(arr);
    }

    return 0;
}
