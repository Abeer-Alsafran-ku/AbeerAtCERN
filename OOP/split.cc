/*
 * This file is for splitting the CSV file 
 * into mini CSV files that can be read and 
 * plotted easly
 *
 *
 * */

#include <iostram>
#include <iostream.h>

void printCSV(const std::vector<std::tuple<int, float, float>> executionTimes, int iterations,int vecSize,int size) {
        /*Printing to the file*/
        const std::string  COMM_METHOD_NAMES[] = {"NONBLOCKING SCATTER", "BLOCKING SCATTER", "BLOCKING SEND/RECV", "NONBLOCKING SEND/RECV"," ", "BLOCKING SENDRECV","ONE SIDED"};
        std::fstream fd;
        fd.open("secondry_result.csv", std::fstream::in | std::fstream::out | std::fstream::app);
        if( !fd ){ //file cannot be opened 
                std::cout<<"File Cannot be opened!\n";
                exit(0);
        }
        else{ //file is opened 
                //write to the file the results
                fd.seekg(0, std::ios::end); //seek the end of the file 
                int file_size = fd.tellg();

                if (file_size == 0) //first time to open and write to a file 
                {

                        fd <<"Communication Method"<<","
                           <<"Scatter/Send"<< ","
                           <<"Gather/Receive"<<","
                           <<"Iterations"<<","
                           <<"Vector Size"<<","
                           <<"Processes"
                           << "\n";
                        // Print the execution times and related information
                        for (int i = 0; i < executionTimes.size(); ++i) {
                                auto [commMethod, avgSendTime, avgRecvTime] = executionTimes[i];
                                fd << COMM_METHOD_NAMES[commMethod-1] << ","
                                << avgSendTime << ","
                                << avgRecvTime << ","
                                << iterations << ","
                                << vecSize<< ","
                                <<size ;
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
                                << avgRecvTime << ","
                                << iterations << ","
                                << vecSize<<","
                                << size;
                        }
                        fd << "\n";
                        fd.close();
                }
        }
}//end of printCSV


int main(int argc , char ** argv )
{
	//take the csv file name from cmnd line argument
	if (argc != 2)
	{
		std::cout << "You should provide the CSV file name\n";
		exit(0);
	}

	fd = 



}

