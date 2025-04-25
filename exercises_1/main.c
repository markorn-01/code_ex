// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> // OpenMP header

float squaredDistance(float x1, float y1, float x2, float y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

// Problem 1
// int main() {
//     int m = 0;
//     int n = 1e7+1;
//     // Seed the random number generator with the current time
//     srand((unsigned int) time(NULL));
//     for (int i = 0; i < n; i++){

//         // Generate random (x, y) in the unit square [0, 1] x [0, 1]
//         float x = (float) rand() / RAND_MAX;
//         float y = (float) rand() / RAND_MAX;
//         if (squaredDistance(x, y, 0, 0) < 1) {
//             m++;
//         }
//     }
//     printf("Estimated pi ≈ %f\n", 4.0 * m / n);
//     return 0;
// }

// Problem 2
int main(int argc, char* argv[]) {
    int m = 0;
    int n = 1e9+1;

    // Parallel region with reduction
    #pragma omp parallel
    {
        // Create a private counter for each thread
        unsigned int seed = time(NULL) ^ omp_get_thread_num(); // unique seed
        int local_m = 0;
        #pragma omp for
        for (int i = 0; i < n; i++) {
            float x = (float) rand_r(&seed) / RAND_MAX;
            float y = (float) rand_r(&seed) / RAND_MAX;

            if (squaredDistance(x, y, 0, 0) < 1) {
                local_m++;
            }
        }

        // Atomically add local count to global count
        #pragma omp atomic
        m += local_m;
        
    }

    printf("Estimated pi ≈ %f\n", 4.0 * m / n);
    return 0;
}