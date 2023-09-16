/**
 * @author RookieHPC
 * @brief Original source code at https://rookiehpc.org/mpi/docs/mpi_win_create_dynamic/index.html
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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
    int my_rank, size =10;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Create the window
    MPI_Win window;
    MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &window);
    MPI_Win_fence(0, window);
    printf("[MPI process %d] Window created dynamically.\n", my_rank);


    if(my_rank == 1) //worker
    {
    

    
	int window_buffer[size];
	    // Allocate and attach the memory region to the window on that target MPI process
        MPI_Win_attach(window, &window_buffer, size*sizeof(int));
        printf("[MPI Process 1] Memory region attached.\n");

        // Get the address of that window and send it to MPI process 1
        MPI_Aint window_buffer_address;
        MPI_Get_address(&window_buffer, &window_buffer_address);
        MPI_Send(&window_buffer_address, 1, MPI_AINT, 0, 0, MPI_COMM_WORLD);
        printf("[MPI process 1] I send the local address of my memory region to MPI process 0.\n");
    
    
       
    
    
    }
    else //root
    {
        // Get the local address of the memory region attached to that window
        MPI_Aint remote_window_buffer_address;
        MPI_Recv(&remote_window_buffer_address, 1, MPI_AINT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[MPI process 0] Local address of the memory region attached to the window on MPI process 0 received. I can now use that in MPI_Put.\n");

        // Put the data into into that window
        
	int value[size];
	for(int i=0;i<size ;i++)
		value[i] = i*2;
        
	MPI_Put(&value, size, MPI_INT, 1, remote_window_buffer_address, size, MPI_INT, window);
        printf("[MPI Process 0] I put value %d in MPI Process 1 window.\n", value[0]);
    }

    // Destroy the window
    //MPI_Win_fence(0, window);
    	 MPI_Win_fence(0, window);
	 if(my_rank == 1)
	    { //worker 
		printf("[MPI process 1] Value in my window: %d.\n", window_buffer[0]);
		for (int i =0;i<size ; i++)
			printf("%d ", window_buffer[i]);
		printf("\n");

		MPI_Win_detach(window, &window_buffer);

		printf("[MPI process 1] Memory region detached.\n");
	    }
	MPI_Win_free(&window);
  	printf("[MPI process 1] Window destroyed.\n");


    MPI_Finalize();

    return EXIT_SUCCESS;
}
