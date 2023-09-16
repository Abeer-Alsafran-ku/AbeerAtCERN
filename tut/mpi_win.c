// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// MPI_Send, MPI_Recv example. Communicates the number -1 from process 0
// to process 1.
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	// Initialize the MPI environment
	MPI_Init(NULL, NULL);
	// Find out rank, size
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// We are assuming at least 2 processes for this task
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	MPI_Win win ;
	MPI_Win_create_dynamic(MPI_INFO_NULL,MPI_COMM_WORLD,&win);
	MPI_Win_fence(0,win);

	int number;
	char ch;
	if (world_rank == 0) {
		// If we are rank 0, set the number to -1 and send it to process 1
		number = -1;
		ch = 'a';
		int size =10;
		int arr[size];
		arr[0] = 2 ; arr[1] = 4;

		MPI_Win_attach(win ,arr,10*sizeof(int));


		MPI_Aint win_buff_addr;
		MPI_Get_address(&arr,&win_buff_addr);

		MPI_Send(&win_buff_addr,1,MPI_AINT,1,0,MPI_COMM_WORLD);

		
		MPI_Put(
				&arr,
				size,
				MPI_INT,
				1,
				win_buff_addr,
				size,
				MPI_INT,
				win
				);

		//mpi fence
		
		printf("DONE! from 0  \n");

	} else if (world_rank == 1) {

		int size =10, received_array[size];
		MPI_Aint win_addr;
		int disp_unit;
		int new_Arr[size];
		void* baseptr;



		MPI_Recv(&win_addr,1,MPI_AINT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		MPI_Get(
				new_Arr,
				size,
				MPI_INT,
				0,
				win_addr,
				size,
				MPI_INT,
				win
				);

		printf("DONE! %d\n",new_Arr);

		MPI_Free_mem(baseptr);


	}
	if (world_rank == 0 ){

		printf("send and recv done \n");
		//MPI_Win_detach(win,win_buff);


	}
	MPI_Win_free(&win);

	printf("DONE!\n\n");
	MPI_Finalize();
}
