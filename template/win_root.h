
//root
//
//
#include <mpi.h>

void win_root(){
     // Get the local address of the memory region attached to that window

	int size =10, arr[size];

	MPI_Win win;
	MPI_Win_create(&arr,size*sizeof(int) , sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&win );
        MPI_Win_fence(0,win);

	for (int i=0;i<size;i++)
		arr[i] = i*2;

        // Get the address of that window and send it to MPI process 1
        MPI_Put(&arr,size,MPI_INT,1,0,size,MPI_INT,win);

        MPI_Win_fence(0, win);

       	MPI_Get(&arr,size,MPI_INT,1,0,size,MPI_INT,win);

        MPI_Win_fence(0, win);

	for (int i=0;i<size;i++)
		printf("%d ",arr[i]);
	
	printf("\n");
        printf("[MPI process 0] send arr\n");
}
