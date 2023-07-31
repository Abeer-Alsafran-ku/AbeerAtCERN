//initializing the libraries needed
//WORKER//
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <utility>
#include <mpi.h>
#include <unistd.h>
#include <cmath>  // for abs() from <cmath>
#include "processInterface.h"

class WorkerProcess : public MPIBase {

public:
    WorkerProcess(){

            MPI_Comm_size(MPI_COMM_WORLD, &size_);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
            //assert(rank_ != 0  && size_ >= 2); 

    }


        std::pair<float, float> blockingSend() override {
            //ROOT RANK IS ALWAYS ZERO

            MPI_Status status;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_FLOAT, &messageSize);

            // Resize vectors
            v1.resize(messageSize);
            v2.resize(messageSize);

            MPI_Recv(&v1[0], messageSize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&v2[0], messageSize, MPI_FLOAT, 0 ,0 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < v1.size(); i++){
                    v1[i] += v2[i];
            }

            MPI_Send(&v1[0], v1.size(), MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    //beginning of nonBlockingSend
    std::pair<float, float> nonBlockingSend() override {
        MPI_Request requestSend;
        MPI_Request requestRecv[1];
        MPI_Status status;


        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_FLOAT, &messageSize);

        // Resize vectors
        v1.resize(messageSize);
        v2.resize(messageSize);

        MPI_Irecv(&v1[0], messageSize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &requestRecv[0]);
        MPI_Irecv(&v2[0], messageSize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &requestRecv[1]);

        MPI_Waitall(2, requestRecv, MPI_STATUS_IGNORE);

        for (size_t i = 0; i < v1.size(); i++) {
            v1[i] += v2[i];
        }

        MPI_Issend(&v1[0], v1.size(),MPI_FLOAT,0,0,MPI_COMM_WORLD, &requestSend);

    }
    //end of nonBlockingSend

    //beginning of blockingScatter
    std::pair<float, float> blockingScatter() override {
        MPI_Recv(&messageSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Resize vectors
        v1.resize(messageSize);
        v2.resize(messageSize);

        MPI_Scatterv(
            NULL, NULL, NULL,
            MPI_FLOAT,
            &v1[0],
            messageSize,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD
        );

        MPI_Scatterv(
            NULL, NULL, NULL,
            MPI_FLOAT,
            &v2[0],
            messageSize,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD
        );	 

        for (size_t i = 0; i < v1.size(); i++) {
            v1[i] += v2[i];
        }

        MPI_Gatherv(
            &v1[0], 
            v1.size(), 
            MPI_FLOAT, 
            NULL, NULL, NULL, 
            MPI_FLOAT, 
            0, 
            MPI_COMM_WORLD
        );
    }
    //end of blockingScatter

    //begging of nonBlockingScatter
    std::pair<float, float> nonBlockingScatter() override{
        MPI_Recv(&messageSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Resize vectors
        v1.resize(messageSize);
        v2.resize(messageSize);

        MPI_Request requestScatter[2];
        MPI_Request requestGather;
        

        MPI_Iscatterv(
            NULL, NULL, NULL,
            MPI_FLOAT,
            &v1[0],
            messageSize,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD,
            &requestScatter[0]
        );

        MPI_Iscatterv(
            NULL, NULL, NULL,
            MPI_FLOAT,
            &v2[0],
            messageSize,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD,
            &requestScatter[1]
        );	 

        MPI_Waitall(2, requestScatter, MPI_STATUS_IGNORE);  // Wait for scatter operations to complete.

        for (size_t i = 0; i < v1.size(); i++) {
            v1[i] += v2[i];
        }

        MPI_Igatherv(
            &v1[0], 
            v1.size(), 
            MPI_FLOAT, 
            NULL, 
            NULL, 
            NULL, 
            MPI_FLOAT, 
            0, 
            MPI_COMM_WORLD,
            &requestGather
        );
    }
    //end of nonBlockingScatter


    //beginning of blockingSendRecv WORKER
    std::pair<float, float> blockingSendRecv() override {
            //ROOT RANK IS ALWAYS ZERO 

            MPI_Status status;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_FLOAT, &messageSize);

            // Resize vectors
            v1.resize(messageSize);
            v2.resize(messageSize);

	    //receiving v1
	   MPI_Sendrecv(
		/*sending buf*/ NULL,
		/*send count*/  0 ,
		/*send type*/   MPI_FLOAT,
		/*dst rank*/    0,/*worker*/
		/*send tag*/    0,/*root tag*/
		
		/*recv buf*/    &v1[0],
		/*recv count*/  messageSize,
		/*recv type*/   MPI_FLOAT,
		/*src rank*/    0, /*root*/
		/*recv tag*/    0,/*worker tag*/
		/*Comm.*/       MPI_COMM_WORLD,
		/*status*/      MPI_STATUS_IGNORE
		); 

	    //receiving v2
	   MPI_Sendrecv(
		/*sending buf*/ NULL,
		/*send count*/  0 ,
		/*send type*/   MPI_FLOAT,
		/*dst rank*/    0,/*worker*/
		/*send tag*/    1,/*root tag*/

		/*recv buf*/    &v2[0],
		/*recv count*/  messageSize,
		/*recv type*/   MPI_FLOAT,
		/*src rank*/    0, /*root*/
		/*recv tag*/    1,/*worker tag*/
		/*Comm.*/       MPI_COMM_WORLD,
		/*status*/      MPI_STATUS_IGNORE
	
		);
    

            //adding the two vectors and the result is in v1
	    for(int i = 0; i < v1.size(); i++){
                    v1[i] += v2[i];
            }

	    //sending back the result of the summation
	    MPI_Sendrecv(
		/*sending buf*/ &v1[0],
		/*send count*/  v1.size() ,
		/*send type*/   MPI_FLOAT,
		/*dst*/         0,/*root*/
		/*send tag*/    2,/*worker tag*/
		
		/*recv buf*/    NULL,
		/*recv count*/  0, 
		/*recv type*/   MPI_FLOAT,
		/*src*/         0,/*worker rank_*/
		/*recv tag*/    2,/*root tag*/
		/*Comm.*/       MPI_COMM_WORLD,
		/*status*/      MPI_STATUS_IGNORE

		);

	
    }
   //end of blockingSend


	//beggining of one sided communincation 
        std::pair<float, float> oneSidedComm() override {
            //ROOT RANK IS ALWAYS ZERO
	    printf("in worker\n");
	    MPI_Win win;
            MPI_Status status;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_FLOAT, &messageSize);
	    int vec_size;

	    //receiving the size of vectors  
	    MPI_Recv(&vec_size,messageSize ,MPI_INT,0,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    // Resize vectors
	    v1.resize(vec_size);
            v2.resize(vec_size);
	    float *win_buff = (float *)malloc (vec_size * sizeof(float));
	    
	    //creating window
	    MPI_Win_create(&win_buff,vec_size*sizeof(float), sizeof(float),MPI_INFO_NULL,MPI_COMM_WORLD,&win);
	    //fence 1
	    MPI_Win_fence(0,win);

	    //receiving the vectors using mpi_Get()

	    //getting v1 
	    MPI_Get(&win_buff,vec_size, MPI_FLOAT, 0,0,vec_size,MPI_FLOAT,win);
	
	    //fence 2 
	    MPI_Win_fence(0,win);
   
	    printf("win buff in worker \n");
	    for(int i=0;i<vec_size ;i++){
		   printf("%d ",win_buff[i]);
	    }
	    printf("\n"); 
	    printf("arr 1 in worker %d %d %d \n",v1[0],v1[1],v1[2]);
	    for (int i=0;i<vec_size ;i++){
		    //copying from window to v1 
		    v1[i] = win_buff[i];
		    printf("%d \n ",win_buff[i]);
	    }
	    
	    printf("after getting 1 in worker \n");
	    //getting v2 
	    //MPI_Get(&win_buff,vec_size, MPI_FLOAT, 0,0,vec_size,MPI_FLOAT,win);

	    //fence 3 
	    //MPI_Win_fence(0,win);

	    printf("arr 2 in worker %d %d %d\n",v2[0],v2[1],v2[2]);
	    for (int i=0;i<vec_size ;i++){
		    //copying from window to v2 
		    v2[i] = win_buff[i];
		    printf("%d \n ",win_buff[i]);
	    }


	    printf("after getting 2 in worker \n");
	    //calculating the summation of 2 vectors
	    for(int i=0;i<vec_size;i++){
		    v1[i] = v1[i] +  v2[i];
	    }

	    printf("result in worker %d \n",v1[0]);
	    for (int i=0;i<vec_size;i++){
		    printf("%d ",v1[i]);
		    win_buff[i]=v1[i];
	    }
	    printf("\n");

	    //sending back the results
	   // MPI_Put(&win_buff,v1.size(),MPI_FLOAT, 0,0,v1.size(),MPI_FLOAT,win);
	    MPI_Put(&v1,v1.size(),MPI_FLOAT, 1,0,v1.size(),MPI_FLOAT,win);

	    MPI_Win_fence(0,win);

	    MPI_Win_free(&win);
	    free(win_buff);
	}


private:
    int size_;
    int rank_;
    int messageSize;           //size for the worker vects
    std::vector<float> v1;
    std::vector<float> v2;
};
