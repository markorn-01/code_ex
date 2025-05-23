#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <time.h>

#define P 8  // Adjust based on p=2^k
#define D 3  // Depth

void swap(int64_t *a, int64_t *b) {
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int64_t arr[], int low, int high) {
    int64_t pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(int64_t arr[], int low, int high, int depth) {
    if (low < high) {
        int pi = partition(arr, low, high);

        if (depth > 0) {
            #pragma omp parallel sections
            {
                #pragma omp section
                quicksort(arr, low, pi - 1, depth - 1);

                #pragma omp section
                quicksort(arr, pi + 1, high, depth - 1);
            }
        } else {
            quicksort(arr, low, pi - 1, depth);
            quicksort(arr, pi + 1, high, depth);
        }
    }
}

void generate_random_array(int64_t *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = ((int64_t)rand() << 32) | rand();
    }
}

int is_sorted(int64_t *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return 0;
    }
    return 1;
}

int main() {
    srand(time(NULL));
    omp_set_num_threads(P);
    // Test with different sizes
    // 2^16, 2^18, 2^20, 2^22
    // 65536, 262144, 1048576, 4194304
    const int sizes[] = {1 << 16, 1 << 18, 1 << 20, 1 << 22};
    for (int i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        int n = sizes[i];
        //Check if size is divisible by P and if not, skip the size.
        //Apparently, in this case, all the sizes of the array are divisible by P.
        //However, this is just to show the check that is required in the exercise.
        if (n % P != 0) continue;

        int64_t *arr = malloc(n * sizeof(int64_t));
        generate_random_array(arr, n);

        double start = omp_get_wtime();
        quicksort(arr, 0, n - 1, D);  // 2^3 = 8 threads
        double end = omp_get_wtime();

        printf("Size: %d, Sorted: %s, Time taken: %f seconds\n", n, is_sorted(arr, n) ? "Yes" : "No", end - start);
        free(arr);
    }
    return 0;
}
