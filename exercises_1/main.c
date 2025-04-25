// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <omp.h> // OpenMP header
#include <mpi.h>
#include <math.h>

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
// int main(int argc, char* argv[]) {
//     int m = 0;
//     int n = 1e9+1;

//     // Parallel region with reduction
//     #pragma omp parallel
//     {
//         // Create a private counter for each thread
//         unsigned int seed = time(NULL) ^ omp_get_thread_num(); // unique seed
//         int local_m = 0;
//         #pragma omp for
//         for (int i = 0; i < n; i++) {
//             float x = (float) rand_r(&seed) / RAND_MAX;
//             float y = (float) rand_r(&seed) / RAND_MAX;

//             if (squaredDistance(x, y, 0, 0) < 1) {
//                 local_m++;
//             }
//         }

//         // Atomically add local count to global count
//         #pragma omp atomic
//         m += local_m;
        
//     }

//     printf("Estimated pi ≈ %f\n", 4.0 * m / n);
//     return 0;
// }

// Problem 3
int main(int argc, char** argv) {
    int rank, size;
    int m_local = 0, m_total = 0;
    int n_total = 1e9+1;

    MPI_Init(&argc, &argv);                 // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // Get this process's rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Get total number of processes

    if (rank == 0) {
        printf("🔢 Running with %d process(es)\n", size);
    }

    int n_per_proc = n_total / size;
    unsigned int seed = time(NULL) + rank * 1234;

    for (int i = 0; i < n_per_proc; i++) {
        float x = (float) rand_r(&seed) / RAND_MAX;
        float y = (float) rand_r(&seed) / RAND_MAX;

        if (squaredDistance(x, y, 0, 0) < 1) {
            m_local++;
        }
    }

    // Reduce all local counts to the total count in rank 0
    MPI_Reduce(&m_local, &m_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        float pi_estimate = 4.0 * m_total / n_total;
        printf("Estimated π ≈ %f\n", pi_estimate);
    }

    MPI_Finalize(); // Clean up
    return 0;
}
