
//worker
//
//

#include <mpi.h> 
void win_worker(){
	// Allocate the window on that target MPI process
        int size =10;
	int arr[size];
	MPI_Win win;
	MPI_Win_create(&arr,size*sizeof(int) , sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&win );
	MPI_Win_fence(0,win);

        // Get the address of that window and send it to MPI process 1
        MPI_Get(&arr,size,MPI_INT,0,0,size,MPI_INT,win);
    
	MPI_Win_fence(0, win);


        printf("[MPI process 1] Value in my window: %d.\n", arr[0]);
	
	for (int i =0;i<size ; i++)
		printf("%d ", arr[i]);
	printf("\n");


	for (int i =0;i<size ; i++)
		arr[i] = arr[i] * 2 ;
	printf("\n");
	
	MPI_Put(&arr, size , MPI_INT, 0,0,size,MPI_INT,win);
	MPI_Win_fence(0, win);

        printf("[MPI process 1] Memory region detached.\n");
}
