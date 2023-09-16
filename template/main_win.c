/**
 * @author RookieHPC
 * @brief Original source code at https://rookiehpc.org/mpi/docs/mpi_win_create_dynamic/index.html
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "win_root.h"
#include "win_worker.h"
int size =10;
/**
 * @brief Illustrate how to create a window dynamically.
 * @details This application consits of 2 MPI processes. They create a window
 * dynamically, then MPI process 0 attaches a region to its window and send its
 * address to MPI process 1. Finally, MPI process 1 uses that address as part of
 * an MPI_Put to write data into MPI process 0 window, which prints its value at
 * the end.
 **/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Check that only 2 MPI processes are spawn
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if(comm_size != 2)
    {
        printf("This application is meant to be run with 2 MPI processes, not %d.\n", comm_size);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Create the window
    MPI_Win window;
    MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &window);
    MPI_Win_fence(0, window);

    if(my_rank == 1) //worker
    {
	win_worker();    
    }
    else //root
    {
	win_root();
   
    }

    // Destroy the window
    
    MPI_Win_free(&window);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
