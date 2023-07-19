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

    //beginning of blockingSend
    std::pair<float, float> blockingSend() override {
            //ROOT RANK IS ALWAYS ZERO 

            MPI_Status status;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_FLOAT, &messageSize);

            // Resize vectors
            v1.resize(messageSize);
            v2.resize(messageSize);
	    //receiving vectors from root process
		   //data  //count     //typ//src rank//tag //comm.    //status            
            MPI_Recv(&v1[0], messageSize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&v2[0], messageSize, MPI_FLOAT, 0 ,0 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	    //summation of v1 and v2
            for(int i = 0; i < v1.size(); i++){
                    v1[i] += v2[i];
            }

	    //sending to the root the result
            MPI_Send(&v1[0], v1.size(), MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
    //end of blockingSend

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

	    std::vector<float> dummy;

	    std::cout<<"Before Receiving in worker  blocking sendrecv\n";
	   
	    //receiving v1
	   MPI_Sendrecv(
		/*sending buf*/ &dummy,
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

	    std::cout<<"After Receiving v1 in worker  blocking sendrecv\n";
	    //receiving v2
	   MPI_Sendrecv(
		/*sending buf*/ &dummy,
		/*send count*/  0 ,
		/*send type*/   MPI_FLOAT,
		/*dst rank*/    0,/*worker*/
		/*send tag*/    0,/*root tag*/
		/*recv buf*/    &v2[0],
		/*recv count*/  messageSize,
		/*recv type*/   MPI_FLOAT,
		/*src rank*/    0, /*root*/
		/*recv tag*/    0,/*worker tag*/
		/*Comm.*/       MPI_COMM_WORLD,
		/*status*/      MPI_STATUS_IGNORE
	
		);
    
	    std::cout<<"After Receiving v2 in worker  blocking sendrecv\n";

            //adding the two vectors and the result is in v1
	    for(int i = 0; i < v1.size(); i++){
                    v1[i] += v2[i];
            }

	    std::cout<<"After addition  in worker  blocking sendrecv\n";
            /*MPI_Send(&v1[0], v1.size(), MPI_FLOAT, 0, 0, MPI_COMM_WORLD);*/

	    //sending back the result of the summation
	    MPI_Sendrecv(
		/*sending buf*/ &v1[0],
		/*send count*/  v1.size() ,
		/*send type*/   MPI_FLOAT,
		/*dst*/         0,/*root*/
		/*send tag*/    0,/*worker tag*/
		/*recv buf*/    &dummy,
		/*recv count*/  0, 
		/*recv type*/   MPI_FLOAT,
		/*src*/         rank_,/*worker rank_*/
		/*recv tag*/    0,/*root tag*/
		/*Comm.*/       MPI_COMM_WORLD,
		/*status*/      MPI_STATUS_IGNORE

		);

	    std::cout<<"After sending results in worker  blocking sendrecv\n";
    }
   //end of blockingSend

private:
    int size_;
    int rank_;
    int messageSize;           //size for the worker vects
    std::vector<float> v1;
    std::vector<float> v2;
};
