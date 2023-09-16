#include "head.h"


int main(int argc , char** argv){

	MPI_Init(NULL,NULL);

	int world_size, world_rank;
	int data;

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);


//	printf("World Size = %d \n",world_size);

	if (world_size < 2 )
	{
		printf("This program should run on minimum 2 processes \n ");
		exit(1);
	}

	if (world_rank == 0)
	{ //this is the root [not necessarly]
	
		int i=110;
		//for(i = 0; i< 3 ; i++)
		//{
			MPI_Send(&i, 1, MPI_INT,1, 0, MPI_COMM_WORLD);
		//}
		printf("Proc %d has sent  messages \n",world_rank);
	}
	else if(world_rank == 1){

		MPI_Recv(&data,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("Proc %d has received %d from Proc 0 \n", world_rank,data);
	
		MPI_Send(&data, 1,MPI_INT, 2,0,MPI_COMM_WORLD);
	
	
	}
	else{

		MPI_Recv(&data,1,MPI_INT,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		printf("Proc %d has received %d from Proc 1 \n", world_rank,data);
	
	}

	MPI_Finalize();
}
