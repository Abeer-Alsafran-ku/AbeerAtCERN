//initializing the libraries needed
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <utility>
#include <mpi.h>
#include <unistd.h>
#include "workerProcess.h"
#include "rootProcess.h"
#include "processInterface.h"
#include <cassert>
#include<tuple>


//Implement MPI_sendrecv

const int MPI_METHODS_COUNT = 7; 



std::tuple<int, std::vector<int>, int> parseCommands(int argc, char* argv[]){

	enum INPUT_OPTIONS { VECTOR_SIZE = 's', COMMUNICATION_METHOD = 'f', ITERATIONS = 'i'};

	//default values
	int vecSize = 100; 
	int iterations = 10; 
	std::vector<int> commMethods; 


	int input;   // Parsing command-line arguments

	while ((input = getopt(argc, argv, "s:f:i:")) != -1) {              
		switch (input) {
			case VECTOR_SIZE:
				try {
					vecSize = std::stoll(optarg, nullptr, 0);
				} catch (std::exception& err) {
					std::cout << "\n\tError: Argument s must be an integer!";
					std::cout << "\n\t" << err.what() << std::endl;
					abort();
				}
				break;
			case COMMUNICATION_METHOD:
				try {
					// Sending Methods selected by user (e.g. 34 user selected methods blocking and nonblocking)
					int inputNum = std::stoll(optarg, nullptr, 0); 
					while(inputNum > 0 ){
						int digit = inputNum % 10; 
						if (digit > MPI_METHODS_COUNT) {
							//FIXME: Raise an exception here
							std::cout << "\n\tError: Argument must be an integer <= " << MPI_METHODS_COUNT << std::endl;
							abort(); 
						}
						commMethods.push_back(digit); 
						inputNum = inputNum / 10; 
					}
				} catch (std::exception& err) {
					std::cout << "\n\tError: Argument r must be an integer!";
					std::cout << "\n\t" << err.what() << std::endl;
					abort();
				}
				break;
			case ITERATIONS:
				try {
					// Set the average run count based on the command-line argument.
					iterations = std::stoll(optarg, nullptr, 0);  
				} catch (std::exception& err) {
					std::cout << "\n\tError: Argument n must be an integer!";
					std::cout << "\n\t" << err.what() << std::endl;
					abort();
				}
				break;
			default:
				std::cerr<<"\n\t WRONGE INPUT ****** ABORT! "<<input<<"\n"; 
				abort();
		}
	}
	return std::make_tuple(vecSize, commMethods, iterations); 
}


// printing to a csv file 
void printCSV(const std::vector<std::tuple<int, float, float>> executionTimes, int iterations) {
	/*Printing to the file*/
	const std::string  COMM_METHOD_NAMES[] = {"NONBLOCKING SCATTER", "BLOCKING SCATTER", "BLOCKING SEND/RECV", "NONBLOCKING SEND/RECV"," ", "BLOCKING SENDRECV","ONE SIDED"};
	std::fstream fd;
	fd.open("secondry_result.csv", std::fstream::in | std::fstream::out | std::fstream::app);
	if( !fd ){ //file cannot be opened 
		std::cout<<"File Cannot be opened1\n";
		exit(0);
	}
	else{ //file is opened 
		//write to the file the results
		fd.seekg(0, std::ios::end);
		int file_size = fd.tellg();

		if (file_size == 0) //first time to open and write to a file 
		{

			fd <<"Communication Method"<<","
			   <<"Scatter/Send"<< ","
		           <<"Gather/Receive"<<","
			   <<"Iterations"<<","
			   << "\n";
			// Print the execution times and related information
			for (int i = 0; i < executionTimes.size(); ++i) {
				auto [commMethod, avgSendTime, avgRecvTime] = executionTimes[i];
				fd << COMM_METHOD_NAMES[commMethod-1] << ","
				<< avgSendTime << ","
				<< avgRecvTime << ", "
				<< iterations<< ",";
			}
			fd << "\n";
			fd.close();
		} 
		else{ //not the first time i.e. data exisit in the file 
			// Print the execution times and related information
                        for (int i = 0; i < executionTimes.size(); ++i) {
                                auto [commMethod, avgSendTime, avgRecvTime] = executionTimes[i];
                                fd << COMM_METHOD_NAMES[commMethod-1] << ","
                                << avgSendTime << ","
                                << avgRecvTime << ", "
                                << iterations<< ",";
                        }
                        fd << "\n";
                        fd.close();
		}
	}
}//end of printCSV 


// printing to a text file 
void printFile(const std::vector<std::tuple<int, float, float>> executionTimes, int iterations) {
	/*Printing to the file*/
	/*Printing to the output screen*/	
	const std::string  COMM_METHOD_NAMES[] = {"NONBLOCKING SCATTER", "BLOCKING SCATTER", "BLOCKING SEND/RECV", "NONBLOCKING SEND/RECV"," ", "BLOCKING SENDRECV","ONE SIDED"};
	const auto COL1 = 25, COL2 = 15, COL3 = 15, COL4 = 11;
	std::string ROW    = "================================================================================";
	std::string DASHES = "--------------------------------------------------------------------------------";
	std::cout.flags(std::ios::fixed | std::ios::showpoint);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(4);


	std::fstream fd;
	fd.open("result_file.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if( !fd ){ //file cannot be opened 
		std::cout<<"File Cannot be opened1\n";
		exit(0);
	}
	else{ //file is opened 

		//write to the file the results

		fd << "\n\n\t"<<ROW;
		fd << "\n\t|| "<<std::left
			<<std::setw(COL1)<<"Communication Method"<<"|| "
			<<std::setw(COL2)<<"Scatter/Send"<<"|| "
			<<std::setw(COL3)<<"Gather/Receive"<<"|| "
			<<std::setw(COL4)<<"Iterations"<<"||"
			<< "\n\t"<<ROW;

		// Print the execution times and related information
		for (int i = 0; i < executionTimes.size(); ++i) {
			if(i > 0) fd << "\n\t"<<DASHES;

			auto [commMethod, avgSendTime, avgRecvTime] = executionTimes[i];

			fd << "\n\t|| " <<std::left
				<< std::setw(COL1) << COMM_METHOD_NAMES[commMethod-1] << "|| "
				<< std::setw(COL2) << avgSendTime << "|| "
				<< std::setw(COL3) << avgRecvTime << "|| "
				<< std::setw(COL4) << iterations<< "||";

		}

		fd << "\n\t"<<ROW<<"\n\n";
		fd.close();
	}  

}//end of printFile 


// print to the standared output 
void printResults(const std::vector<std::tuple<int, float, float>> executionTimes, int iterations) {

	/*Printing to the output screen*/	
	const std::string  COMM_METHOD_NAMES[] = {"NONBLOCKING SCATTER", "BLOCKING SCATTER", "BLOCKING SEND/RECV", "NONBLOCKING SEND/RECV"," ", "BLOCKING SENDRECV","ONE SIDED"};
	const auto COL1 = 25, COL2 = 15, COL3 = 15, COL4 = 11;
	std::string ROW    = "================================================================================";
	std::string DASHES = "--------------------------------------------------------------------------------";
	std::cout.flags(std::ios::fixed | std::ios::showpoint);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(4);


	std::cout << "\n\n\t"<<ROW;
	std::cout << "\n\t|| "<<std::left
		<<std::setw(COL1)<<"Communication Method"<<"|| "
		<<std::setw(COL2)<<"Scatter/Send"<<"|| "
		<<std::setw(COL3)<<"Gather/Receive"<<"|| "
		<<std::setw(COL4)<<"Iterations"<<"||"
		<< "\n\t"<<ROW;



	// Print the execution times and related information
	for (int i = 0; i < executionTimes.size(); ++i) {
		if(i > 0) std::cout << "\n\t"<<DASHES;    

		auto [commMethod, avgSendTime, avgRecvTime] = executionTimes[i]; 

		std::cout << "\n\t|| " <<std::left
			<< std::setw(COL1) << COMM_METHOD_NAMES[commMethod-1] << "|| "
			<< std::setw(COL2) << avgSendTime << "|| "
			<< std::setw(COL3) << avgRecvTime << "|| "
			<< std::setw(COL4) << iterations<< "||";

	}

	std::cout << "\n\t"<<ROW<<"\n\n";    
}//end of printResults


// mian function
int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv); 

	int rank;
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	assert(size >= 2);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	auto [vecSize, commMethods, iterations] = parseCommands(argc,argv);

	std::unique_ptr<MPIBase> MPIObject ;

	if(rank != 0){ //worker
		MPIObject = std::make_unique<WorkerProcess>();
	}else{ //root 
		MPIObject = std::make_unique<RootProcess>(vecSize);
	}

	//tuple<commMethod, avgSendTime, avgRecvTime> 
	std::vector<std::tuple<int, float, float>> results; 


	for (auto i = 0; i < commMethods.size(); ++i) {
		auto [avgSendTime, avgRecvTime] =  MPIObject->calculateAverageTime(commMethods[i], iterations);
		results.push_back(std::make_tuple(commMethods[i], avgSendTime, avgRecvTime));
	}

	if (rank == 0){ //root 
		printResults(results, iterations); //print to std output
		//printCSV(results, iterations); //print to a csv file 
	}

	MPI_Finalize(); 

	return 0;
}

