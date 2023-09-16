#include <stdio.h>
#include <mpi.h>

#define ARRAY_SIZE 5

int main(int argc, char* argv[]) {
    int rank, size;
    int my_array[ARRAY_SIZE];
    int remote_array[ARRAY_SIZE];
    int remote_array2[ARRAY_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        printf("This example requires exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    // Initialize local arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        my_array[i] = rank * ARRAY_SIZE + i; // Process 0: [0, 1, 2, 3, 4], Process 1: [5, 6, 7, 8, 9]
    }

    MPI_Win win;
    MPI_Win_create(&remote_array, ARRAY_SIZE * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    // One-sided communication using MPI_Put
    if (rank == 0) {
        // Process 0 updates values in Process 1's memory
        int new_values1[ARRAY_SIZE] = {10, 20, 30, 40, 50};
        int new_values2[ARRAY_SIZE] = {100, 200, 300, 400, 500};

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
        MPI_Put(new_values1, ARRAY_SIZE, MPI_INT, 1, 0, ARRAY_SIZE, MPI_INT, win);
        MPI_Put(new_values2, ARRAY_SIZE, MPI_INT, 1, ARRAY_SIZE, ARRAY_SIZE, MPI_INT, win);
        MPI_Win_unlock(1, win);
    } else if (rank == 1) {
        // Process 1 retrieves values from Process 0's memory
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Get(remote_array, ARRAY_SIZE, MPI_INT, 0, 0, ARRAY_SIZE, MPI_INT, win);
        //MPI_Get(remote_array2, ARRAY_SIZE, MPI_INT, 0, 0, ARRAY_SIZE, MPI_INT, win);
        MPI_Win_unlock(0, win);

        // Display values received from Process 0
        printf("Process %d received values: ", rank);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", remote_array[i]);
        }
        printf("\nand\n");
    
   	for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", remote_array2[i]);
        }
        printf("\n");
    
    }

    MPI_Win_free(&win);
    MPI_Finalize();

    return 0;
}

