#include "head.h"


int main(int argc , char**argv)
{
	MPI_Init(NULL,NULL);

	int world_rank, world_size;

	MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&world_size);


	int data = 4;
	if (world_rank == 0 ){
		//this is the first proc [could be the root]


			//data //count //type //dst //tag  //comm.
		MPI_Send(&data, 1, MPI_INT,   1,   0, MPI_COMM_WORLD   );
	}
	else{
		// this is the other process/es
		// [it could be min of 2 procs]
		
			//data   //count //type//src //tag //comm.  //status
		MPI_Recv( &data , 1,MPI_INT,    0, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE); 

		printf("Proc %d received data %d from Proc 0 \n",world_rank,data);
	}
	MPI_Finalize();
}
