//initializing the libraries needed
//ROOT//

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

class RootProcess: public MPIBase {
	public:

		RootProcess(int vectorSize){ //int comType, int vectorSize, int avg){
			MPI_Comm_size(MPI_COMM_WORLD, &size_);
			MPI_Comm_rank(MPI_COMM_WORLD, &rank_);

			generateRandomData(vectorSize);
		}

		//beginning of blockingSend 
		std::pair<float, float> blockingSend() override {
			// Send input data from root process to worker processes.

			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			float startTime = MPI_Wtime();

			int curIdx = 0;
			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch
				//data       //count   //type //dst rank//tag //comm. 	
				MPI_Send(  &v1_[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
				MPI_Send(  &v2_[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
				curIdx += curSize;
			}

			float endTime = MPI_Wtime();

			sendDuration = (endTime - startTime) * 1000;

			result.resize(v1_.size()) ;

			startTime = MPI_Wtime();

			curIdx = 0;
			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch
				//dat            //count  //type //src rank//tag //comm.    //status
				MPI_Recv(&result[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				curIdx += curSize;
			}

			endTime = MPI_Wtime();
			recvDuration = (endTime - startTime)*1000 ;

			checkResult(result);
			return std::pair<float, float>(sendDuration, recvDuration);

		}//end of blockingSend


		//beginning of nonBlockingSend 
		std::pair<float, float> nonBlockingSend() override {

			MPI_Request requestSend[2*(size_ -1)]; //two for each process (one for sending v1 and the other for sending v2)
			MPI_Request requestRecv[size_ -1];

			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			float startTime = MPI_Wtime();

			int curIdx = 0;

			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch
				MPI_Issend(  &v1_[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &requestSend[(i-1)*2]);
				MPI_Issend(  &v2_[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &requestSend[(i-1)*2 + 1]);
				curIdx += curSize;
			}
			MPI_Waitall( 2*(size_ - 1), requestSend, MPI_STATUS_IGNORE);

			float endTime = MPI_Wtime();
			sendDuration = (endTime - startTime)*1000;

			result.resize(v1_.size()) ;

			startTime = MPI_Wtime();

			curIdx = 0;
			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch
				MPI_Irecv(&result[curIdx], curSize, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &requestRecv[i-1]);
				curIdx += curSize;
			}

			//wait for all receive processes 
			for(int i=1 ; i<size_ ;i++)
			{
				MPI_Wait(&requestRecv[i-1],MPI_STATUS_IGNORE);
			}

			endTime = MPI_Wtime();
			recvDuration = (endTime - startTime)*1000 ;

			checkResult(result);
			return std::pair<float, float>(sendDuration, recvDuration);

		}
		//end of nonBlockingSend 


		// beginning of blockingScatter 
		std::pair<float, float> blockingScatter() override {
			std::vector<int> numDataPerProcess_ (size_, 0);
			std::vector<int> displacementIndices_ (size_, 0);

			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			int curIdx = 0;
			for(int i = 1; i < size_; i++){
				numDataPerProcess_[i] = batchSize + ((extraBatches >= i)? 1 : 0) ;
				displacementIndices_[i] = curIdx;
				curIdx += batchSize + ((extraBatches >= i)? 1 : 0) ;
			}

			for (int i = 1; i < size_; i++) {
				int sizeToSend = batchSize + ((extraBatches >= i) ? 1 : 0);
				MPI_Send(&sizeToSend, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			float startTime = MPI_Wtime();
			// Start scattering.
			MPI_Scatterv(
					&v1_[0],
					&numDataPerProcess_[0],
					&displacementIndices_[0],
					MPI_FLOAT,
					NULL,
					0,
					MPI_FLOAT,
					0,
					MPI_COMM_WORLD
				    );
			MPI_Scatterv(
					&v2_[0],
					&numDataPerProcess_[0],
					&displacementIndices_[0],
					MPI_FLOAT,
					NULL,
					0,
					MPI_FLOAT,
					0,
					MPI_COMM_WORLD
				    );

			float endTime = MPI_Wtime();
			result.resize(v1_.size()) ;

			sendDuration = (endTime - startTime) * 1000;

			startTime = MPI_Wtime();

			MPI_Gatherv(
					NULL, 
					0, 
					MPI_FLOAT, 
					&result[0], 
					&numDataPerProcess_[0], 
					&displacementIndices_[0], 
					MPI_FLOAT, 
					0, 
					MPI_COMM_WORLD
				   );
			endTime = MPI_Wtime();
			recvDuration = (endTime - startTime)*1000 ;

			checkResult(result);

			return std::pair<float, float>(sendDuration, recvDuration);

		}//end of blockingScatter


		//beginnig of nonBlockingScatter 
		std::pair<float, float> nonBlockingScatter() override{

			std::vector<int> numDataPerProcess_ (size_, 0);
			std::vector<int> displacementIndices_ (size_, 0);

			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			MPI_Request requestScatter[2];
			MPI_Request requestGather;

			int curIdx = 0;
			for(int i = 1; i < size_; i++){
				numDataPerProcess_[i] = batchSize + ((extraBatches >= i)? 1 : 0) ;
				displacementIndices_[i] = curIdx;
				curIdx += batchSize + ((extraBatches >= i)? 1 : 0) ;
			}

			for (int i = 1; i < size_; i++) {
				int sizeToSend = batchSize + ((extraBatches >= i) ? 1 : 0);
				MPI_Send(&sizeToSend, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			float startTime = MPI_Wtime();  // Get the start time before scattering.

			MPI_Iscatterv(
					&v1_[0],
					&numDataPerProcess_[0],
					&displacementIndices_[0],
					MPI_FLOAT,
					NULL,
					0,
					MPI_FLOAT,
					0,
					MPI_COMM_WORLD,
					&requestScatter[0]
				     );

			MPI_Iscatterv(
					&v2_[0],
					&numDataPerProcess_[0],
					&displacementIndices_[0],
					MPI_FLOAT,
					NULL,
					0,
					MPI_FLOAT,
					0,
					MPI_COMM_WORLD,
					&requestScatter[1]
				     );


			float endTime = MPI_Wtime();
			result.resize(v1_.size()) ;

			sendDuration = (endTime - startTime) * 1000;

			startTime = MPI_Wtime();
			MPI_Igatherv(
					NULL, 
					0, 
					MPI_FLOAT, 
					&result[0], 
					&numDataPerProcess_[0], 
					&displacementIndices_[0], 
					MPI_FLOAT, 
					0, 
					MPI_COMM_WORLD,
					&requestGather
				    );

			MPI_Wait(&requestGather, MPI_STATUS_IGNORE);  // Wait for gather operation to complete.

			endTime = MPI_Wtime();

			recvDuration = (endTime - startTime)*1000 ;

			checkResult(result);

			return std::pair<float, float>(sendDuration, recvDuration);

		}//end of nonBlockingScatter 

		//beginning of blockingSendRecv ROOT
		std::pair<float, float> blockingSendRecv() override {
			// Send input data from root process to worker processes.
			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			float startTime = MPI_Wtime();

			int curIdx = 0;
			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch


				MPI_Sendrecv( //sending v1
						/*sending buf*/ &v1_[curIdx],
						/*send count*/  curSize ,		
						/*send type*/  	MPI_FLOAT,
						/*dst rank*/	i,/*worker*/
						/*send tag*/	0,/*root tag*/	

						/*recv buf*/	NULL,/*not receiving*/	
						/*recv count*/  0,	
						/*recv type*/	MPI_FLOAT,	
						/*src rank*/    i, /*root*/		
						/*recv tag*/	0, /*worker tag*/	
						/*Comm.*/	MPI_COMM_WORLD,	
						/*status*/	MPI_STATUS_IGNORE	

					    );

				MPI_Sendrecv( //sending v2
						/*sending buf*/ &v2_[curIdx],
						/*send count*/  curSize ,		
						/*send type*/  	MPI_FLOAT,
						/*dst*/		i,/*worker*/
						/*send tag*/	1,

						/*recv buf*/	NULL,	
						/*recv count*/	0,	
						/*recv type*/	MPI_FLOAT,	
						/*src*/         i,/*root*/		
						/*recv tag*/	1,	
						/*Comm.*/	MPI_COMM_WORLD,	
						/*status*/	MPI_STATUS_IGNORE	

					    );

				curIdx += curSize;
			}


			float endTime = MPI_Wtime();

			sendDuration = (endTime - startTime) * 1000;

			result.resize(v1_.size()) ;

			startTime = MPI_Wtime();

			curIdx = 0;
			for (int i = 1; i < size_; i++) {
				int curSize = batchSize + ((extraBatches >= i)? 1 : 0) ; //size of cur Batch

				//receiving the result of adding v1 to v2
				MPI_Sendrecv( 
						/*sending buf*/ NULL,
						/*send count*/  0 , 		
						/*send type*/  	MPI_FLOAT,
						/*dst*/		i, /*root*/    
						/*send tag*/	2,/*worker tag*/

						/*recv buf*/	&result[curIdx],	
						/*recv count*/	curSize,	
						/*recv type*/	MPI_FLOAT,	
						/*src*/         i,/*worker*/	
						/*recv tag*/	2,/*root tag*/
						/*Comm.*/	MPI_COMM_WORLD,	
						/*status*/	MPI_STATUS_IGNORE	

					    );

				curIdx += curSize;
			}


			endTime = MPI_Wtime();
			recvDuration = (endTime - startTime)*1000 ;

			checkResult(result);
			return std::pair<float, float>(sendDuration, recvDuration);

		}//end of blockingSendRecv

		//beginning of oneSidedComm 
		std::pair<float, float> oneSidedComm() override {
			// Send input data from root process to worker processes.

			printf("in root\n");
			int batchSize = v1_.size() / (size_ - 1); //the size for each process execluding root
			int extraBatches = v1_.size() % (size_ - 1); //the size for the batches thatll get the extra (%) execluding root

			MPI_Win win;
			float *win_buff = (float *)malloc(v1_.size()*sizeof(float));

			//sending vec size
			int vec_size = v1_.size();

			MPI_Send(&vec_size,1,MPI_INT,1,0,MPI_COMM_WORLD);

			//creating a window
			MPI_Win_create(&win_buff[0],v1_.size()*sizeof(float),sizeof(float),MPI_INFO_NULL,MPI_COMM_WORLD,&win);
			//fence 1
			MPI_Win_fence(0,win);

			float startTime = MPI_Wtime();
			int curIdx = 0;
			//sending v1 
			printf("win buff1 in root \n");	
			for(int i=0;i<vec_size ;i++){
				win_buff[i] = v1_[i];
				printf("%d ",win_buff[i]);
			}
			printf("\n");
			//fence 2
			MPI_Win_fence(0,win);
			//put 1 
			MPI_Put(&win_buff,vec_size,MPI_FLOAT, 1,0,vec_size, MPI_FLOAT,win);
			//fence 3
			MPI_Win_fence(0,win);
			
			//sending v2
			for(int i=0;i<vec_size ;i++)
			//copying v2 to win buff
				win_buff[i] = v2_[i];

			printf("win buff2 in root \n");	
			for(int i=0;i<vec_size ;i++)
				printf("%d ",v2_[i]);
			printf("\n");
			//fence 4
			MPI_Win_fence(0,win);
			//put 2
			MPI_Put(&win_buff,vec_size,MPI_FLOAT, 1,0,vec_size, MPI_FLOAT,win);
			//fence 5
			MPI_Win_fence(0,win);
			
			float endTime = MPI_Wtime();
			sendDuration = (endTime - startTime) * 1000;
			result.resize(v1_.size()) ;
			startTime = MPI_Wtime();
			
			//getting the result from worker	
			//fence 6
			MPI_Win_fence(0,win);
			endTime = MPI_Wtime();
			recvDuration = (endTime - startTime)*1000 ;
			printf("res in root \n");
			for (int i =0;i<vec_size;i++)
			{
				//copying from window to result buff
				result[i] = win_buff[i];
			
				printf("%d ",win_buff[i]);
			}
		
			MPI_Win_free(&win);
			free(win_buff);
			checkResult(result);
			return std::pair<float, float>(sendDuration, recvDuration);

		}//end of oneSidedComm

	private:
		const int precisionFactor = 4;
		std::vector<float> v1_;
		std::vector<float> v2_;
		std::vector<float> result ;
		int size_;          //num of process
		int rank_;          //rank of process - HERE its always zero
		float sendDuration;
		float recvDuration;

		void generateRandomData(int vectorSize) {
			std::random_device rand;  // Random device used to seed the random engine.
			std::default_random_engine gener(rand());  // Default random engine.
			std::uniform_real_distribution<> dis(0., 1.);  // Uniform distribution from 0 to 1.
			// Generate a random number and assign it to the vector element.
			for (int i = 0; i < vectorSize; i++) {
				v1_.push_back(dis(gener));
				v2_.push_back(dis(gener));
				//validationReference_.push_back(mainInput1_[i] + mainInput2_[i]);
			}
		}

		void checkResult(std::vector<float> resultVect){

			float totalError{0.0};  // Variable to store the total error.


			// Calculate the percentage difference and accumulate the total error.
			for (int i = 0; i < resultVect.size(); i++) {
				float s = v1_[i] + v2_[i];
				totalError += ((s - resultVect[i]) / s) * 100.0;
			}

			// If there is a non-zero total error, print the results and error analysis.    
			if ( totalError == 0.0) {
				return ; // No error Found;
			}

			std::cout << "\n-------------------------------------------------------\n";
			std::cout << "| RootSum | WorksSum | Error   | Error %  | Process # |";
			std::cout << "\n-------------------------------------------------------\n";
			std::cout.precision(precisionFactor);



			int batchSize = v1_.size() / (size_ - 1);  //execluding root
			int extraBatches = v1_.size() % (size_ - 1); //execluding root
			int curBatchSize = batchSize + (extraBatches > 0)? 1 : 0;
			int workerRank = 0;
			for (int i = 0; i < resultVect.size(); i++) {
				float correct = v1_[i] + v2_[i];
				float error = correct-resultVect[i] ;
				if(error != 0.0) {
					float errorPercent = (error/correct)*100.0 ;
					std::cout << "| " << correct << "  | " << resultVect[i] << "  |"
						<< std::setw(9) << error << " |"
						<< std::setw(9) << errorPercent << " |"
						<< std::setw(9) << workerRank << " |\n";
				}
				if(i > curBatchSize){
					workerRank += 1;
					curBatchSize = batchSize + (extraBatches - workerRank > 0)? 1 : 0;
				}

			}

			std::cout << "-------------------------------------------------------\n";
			std::cout << "Total Error = " << totalError << std::endl;

		}
};
