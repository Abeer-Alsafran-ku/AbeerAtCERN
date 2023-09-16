//initializing the libraries needed

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

//Global Variables Intializing
int mpiFunctionCount_ = 5;                       // Number of MPI function options.
int vectorSize_ = 21;                     // Default size of vectors.
unsigned int avgRunCount_ = 5;                   // Default number of runs for each function to calculate average time.
int decimalPrecision_ = 4;                       // Default decimal precision for printing.
int rootProcess_ = 0;                            // Rank of the root process.
int userChoice_ = 0;                             // User's choice for which function to run.

std::vector<std::pair<float, float>> executionTiming_(
    mpiFunctionCount_, std::make_pair(0, 0));    // Vector to store execution time of scatter/send and gather/receive for each function.


void parseCommands(int argc, char* argv[]);



class MPISetup {
public:

    int numProcesses_{0};                          // Total number of processes.
    int processRank_{0};                           // Rank of the current process.
    std::pair<int, int> taskDistribution_{0, 0};   // Pair representing the distribution of tasks among processes.
    std::vector<float> mainInput1_;                // First input vector.
    std::vector<float> mainInput2_;                // Second input vector.
    std::vector<float> collectedOutput_;           // Vector storing the output collected from worker processes.
    std::vector<float> validationReference_;       // Vector storing reference output to verify results from each process.
    std::vector<float> workerInput1_;              // Subset of 'mainInput1' for worker processes only.
    std::vector<float> workerInput2_;              // Subset of 'mainInput2' for worker processes only.
    std::vector<int> displacementIndices_;         // Vector storing the starting index for each process's data.
    std::vector<int> numDataPerProcess_;           // Number of data elements to be sent for each process.

    std::vector<int> userSelectedFunctions_;   // Vector to store user's function selections.

    MPISetup(int userChoice_, int vectorSize_, int avgRunCount_){
        numProcesses_ = MPI::COMM_WORLD.Get_size();                                                                                   // Rank of the current process.

        processRank_ = MPI::COMM_WORLD.Get_rank();                                                                                    // Rank of the current process.
        taskDistribution_ = distributeTasks(vectorSize_, numProcesses_);                                              // Pair representing the distribution of tasks among processes.

        if (!taskDistribution_.first) {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);                                                                                      // Abort the program if the task distribution is not valid.
        }

        mainInput1_.resize(vectorSize_);                                                                                      // First input vector.
        mainInput2_.resize(vectorSize_);                                                                                     // Second input vector.
        collectedOutput_.resize(vectorSize_);                                                                                // Vector storing the output collected from worker processes.
        validationReference_.resize(vectorSize_);                                                                            // Vector storing reference output to verify results from each process.
        numDataPerProcess_ = calculateNumDataToSend(numProcesses_, taskDistribution_);                                   // Calculate the number of data elements to be sent for each process.
        displacementIndices_ = calculateDisplacementIndices(numProcesses_, taskDistribution_, numDataPerProcess_);       // Vector storing the starting index for each process's data.
        workerInput1_.resize(numDataPerProcess_[processRank_]);                                                              // Subset of 'mainInput1' for worker processes only.
         workerInput2_.resize(numDataPerProcess_[processRank_]);                                                              // Subset of 'mainInput2' for worker processes only.


        userSelectedFunctions_ = convertIntToVector(userChoice_);   // Vector to store user's function selections.

        if (!processRank_) {
            generateRandomNumbers(mainInput1_);  // Generate random floating-point numbers between 0 and 1 in the root process.
            generateRandomNumbers(mainInput2_);
            std::cout << "\n\tNumber of Processes: " << numProcesses_ << std::endl;
            std::cout << "\tTask Distribution First: " << taskDistribution_.first << std::endl;
            std::cout << "\tTask Distribution Second: " << taskDistribution_.second << std::endl;
            for (int j = 0; j < vectorSize_; j++) {
            validationReference_[j] = mainInput1_[j] + mainInput2_[j];  // Calculate the sum for verification.
            }
        }


    } // Constructor - parse arguments, initialize variables and data, etc.
   
private:

    //methods used 

    const std::vector<int> convertIntToVector(int number) {
        std::vector<int> digits;  // Vector to store the individual digits of the input integer.

        int digit = 1;  // Variable to store each digit.

        while (number > 0) {
            digit = number % 10;  // Extract the rightmost digit.
            if (digit > mpiFunctionCount_) {  // Check if the digit is within the valid range.
            std::cout << "\n\tError: Argument must be an integer <= " << mpiFunctionCount_ << std::endl;
            return std::vector<int>();  // Return an empty vector to indicate an error.
            }
            digits.push_back(digit);  // Store the digit in the vector.
            number /= 10;  // Remove the rightmost digit from the input integer.
        }
        std::reverse(digits.begin(), digits.end());  // Reverse the order of digits to match the original input.
        return digits;  // Return the vector containing the individual digits in the correct order.
    }
            
    std::pair<int, int> distributeTasks(int totalTasks, int numProcs) {
        std::pair<int, int> taskDistribution{0, 0};  // Pair to store the task distribution.

        if (numProcs > 1 && numProcs <= totalTasks) {
            taskDistribution.first = totalTasks / (numProcs - 1);   // Number of tasks for each process.
            taskDistribution.second = totalTasks % (numProcs - 1);  // Extra tasks for the process.
        } else {
            std::cout << "\tError: Either no workers are found or number of processes is larger than the task length!\n";
        }

        return taskDistribution;
    }

    const std::vector<int> calculateNumDataToSend(
        int numProcs, std::pair<int, int> taskDist) {
        std::vector<int> numDataToSend(numProcs, taskDist.first);
        numDataToSend[0] = 0;

        for (int i = 1; i < taskDist.second + 1; i++) {
            numDataToSend[i] += 1;  // Extra work for each first process.
        }

        return numDataToSend;
    }


    const std::vector<int> calculateDisplacementIndices(
        int numProcs,
        std::pair<int, int> taskDist,
        const std::vector<int>& numDataToSend){
        std::vector<int> displacementIndices(numProcs, taskDist.first);

        displacementIndices[0] = 0;
        displacementIndices[1] = 0;  // Start Here.

        for (int i = 2; i < numProcs; i++) {  // neglect root
            displacementIndices[i] = numDataToSend[i - 1] + displacementIndices[i - 1];
            // Calculate the starting index for each process's data based on the number of data elements to be sent.
        }

        return displacementIndices;
    }


    void generateRandomNumbers(std::vector<float>& vect) {
        std::random_device rand;  // Random device used to seed the random engine.
        std::default_random_engine gener(rand());  // Default random engine.
        std::uniform_real_distribution<> dis(0., 1.);  // Uniform distribution from 0 to 1.
        int size = vect.size();  // Size of the vector.

        for (int i = 0; i < size; i++) {
            vect.at(i) = dis(gener);  // Generate a random number and assign it to the vector element.
        }
    }

};







class MPICalculation {

public:
    MPICalculation(MPISetup& mpiSetup) 
        : mpiSetup_(mpiSetup) {
        // Initialize other variables/data here as needed
    }

    void performCalculations(){
        // Execute the selected MPI functions and calculate the average execution times
        for (size_t i = 0; i < mpiSetup_.userSelectedFunctions_.size(); ++i) {
            if (mpiSetup_.userSelectedFunctions_[i] == 1) {
            executionTiming_[0] = calculateAverageTime(&MPICalculation::nonBlockingScatter, avgRunCount_);
            } else if (mpiSetup_.userSelectedFunctions_[i] == 2) {
            executionTiming_[1] = calculateAverageTime(&MPICalculation::blockingScatter, avgRunCount_);
            } else if (mpiSetup_.userSelectedFunctions_[i] == 3) {
            executionTiming_[2] = calculateAverageTime(&MPICalculation::nonBlockingSend, avgRunCount_);
            } else if (mpiSetup_.userSelectedFunctions_[i] == 4) {
            executionTiming_[3] = calculateAverageTime(&MPICalculation::blockingSend, avgRunCount_);
            } else if (mpiSetup_.userSelectedFunctions_[i] == 5) {
            executionTiming_[4] = calculateAverageTime(&MPICalculation::multiNonBlockingSend, avgRunCount_);
            } else {
            std::cout << "\n\n\tError: The user has not chosen any function number!\n";
            break;
            }
        }

        if (!mpiSetup_.processRank_) {
            compareExecutionTimes(executionTiming_, mpiFunctionCount_, mpiSetup_.userSelectedFunctions_, avgRunCount_);  // Compare and print the execution times.
        }

    }

private:
    MPISetup& mpiSetup_; // Keep a reference to the setup object


    // Calculate the average execution time for a given MPI function.
    // It takes the MPI function, MPI data, and the run count.

    std::pair<float, float> calculateAverageTime(std::pair<float, float> (MPICalculation::*mpiFunction)(MPISetup&), unsigned int runCount) {
        std::pair<float, float> averageTime;

        // Perform multiple runs of the MPI function and accumulate the timings
        for (long unsigned int i = 0; i < runCount; ++i) {
            auto timing = (this->*mpiFunction)(mpiSetup_);
            averageTime.first += timing.first;
            averageTime.second += timing.second;
        }

        // Calculate the average timings by dividing the accumulated values
        averageTime.first /= runCount;
        averageTime.second /= runCount;

        return averageTime;
    }

    void printResultsAndCheck(
        std::vector<float>& referenceOutput,
        std::vector<float>& collectedOutput,
        std::pair<int, int> taskDist,
        const std::vector<int>& displacementIndices,
        const std::vector<int>& numDataToSend) {

        float percent{0.0};  // Variable to store the percentage difference between reference and output.
        float totalError{0.0};  // Variable to store the total error.
        int processIndex{1};  // Current process index.

        // Calculate the percentage difference and accumulate the total error.
        for (int j = 0; j < vectorSize_; j++) {
            percent = ((referenceOutput[j] - collectedOutput[j]) / referenceOutput[j]) * 100;
            totalError += percent;
        }

        // If there is a non-zero total error, print the results and error analysis.
        if (totalError) {
            std::cout << "\n-------------------------------------------------------\n";
            std::cout << "| RootSum | WorksSum | Error   | Error %  | Process # |";
            std::cout << "\n-------------------------------------------------------\n";
            std::cout.precision(decimalPrecision_);

            // Print the root sum, work sum, error, error percentage, and process number for each data element.
            for (int j = 0; j < vectorSize_; j++) {
            std::cout << "| " << referenceOutput[j] << "  | " << collectedOutput[j] << "  |"
                        << std::setw(9) << referenceOutput[j] - collectedOutput[j] << " |"
                        << std::setw(9) << percent << " |"
                        << std::setw(9) << processIndex << " |\n";

            // Update the current process index if the next displacement index is reached.
            if (j + 1 == displacementIndices[processIndex + 1]) {
                ++processIndex;
            }
            }

            std::cout << "-------------------------------------------------------\n";
            std::cout << "-Total Error is " << totalError << std::endl;

            // Print the number of data elements processed by each process.
            for (long unsigned int j = 1; j < displacementIndices.size(); j++) {
            std::cout << "Process [" << j << "] Worked On " << numDataToSend[j] << " Data\n";
            }
        }
    }





    // Print the results and check if they match the reference output. It takes the reference output,
    // collected output, task distribution, displacement indices, and the number of data elements sent.

    std::pair<float, float> nonBlockingScatter(MPISetup& mpiSetup_) {
        std::pair<float, float> returnValue;

        std::cout.setf(std::ios::fixed, std::ios::floatfield);
        double startTimeScatter = 0;
        double endTimeScatter = 0;
        double startTimeGather = 0;
        double endTimeGather = 0;

        MPI_Request requestRootScatter[2];
        MPI_Request requestRootGather;

        startTimeScatter = MPI_Wtime();  // Get the start time before scattering.

        // Non-Blocking Scatter.
        MPI_Iscatterv(&mpiSetup_.mainInput1_[0],
                        &mpiSetup_.numDataPerProcess_[0],
                        &mpiSetup_.displacementIndices_[0],
                        MPI_FLOAT,
                        &mpiSetup_.workerInput1_[0],
                        mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                        MPI_FLOAT,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootScatter[0]);

        MPI_Iscatterv(&mpiSetup_.mainInput2_[0],
                        &mpiSetup_.numDataPerProcess_[0],
                        &mpiSetup_.displacementIndices_[0],
                        MPI_FLOAT,
                        &mpiSetup_.workerInput2_[0],
                        mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                        MPI_FLOAT,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootScatter[1]);

        MPI_Waitall(2, requestRootScatter, MPI_STATUS_IGNORE);  // Wait for scatter operations to complete.

        endTimeScatter = MPI_Wtime();  // Get the end time after scattering.

        if (mpiSetup_.processRank_ != rootProcess_) {  // Only for worker processes.
            for (long unsigned int i = 0; i < mpiSetup_.workerInput1_.size(); i++) {
            mpiSetup_.workerInput1_[i] += mpiSetup_.workerInput2_[i];
            }
        }

        startTimeGather = MPI_Wtime();  // Get the start time before gathering.

        // Non-Blocking Gathering.
        MPI_Igatherv(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    &mpiSetup_.collectedOutput_[0],
                    &mpiSetup_.numDataPerProcess_[0],
                    &mpiSetup_.displacementIndices_[0],
                    MPI_FLOAT,
                    rootProcess_,
                    MPI_COMM_WORLD,
                    &requestRootGather);

        MPI_Wait(&requestRootGather, MPI_STATUS_IGNORE);  // Wait for gather operation to complete.

        endTimeGather = MPI_Wtime();  // Get the end time after gathering.

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only the root process prints the results.
            printResultsAndCheck(mpiSetup_.validationReference_,
                                mpiSetup_.collectedOutput_,
                                mpiSetup_.taskDistribution_,
                                mpiSetup_.displacementIndices_,
                                mpiSetup_.numDataPerProcess_);

            returnValue.first = (endTimeScatter - startTimeScatter) * 1000;  // Calculate scatter time in milliseconds.
            returnValue.second = (endTimeGather - startTimeGather) * 1000;  // Calculate gather time in milliseconds.
        }

        return returnValue;
    }


    // Perform blocking scatter operation using MPI. Returns the time taken for scatter and gather.
    std::pair<float, float> blockingScatter(MPISetup& mpiSetup_) {
        std::pair<float, float> returnValue;
        std::cout.setf(std::ios::fixed, std::ios::floatfield);

        double startTimeScatter = 0;
        double endTimeScatter = 0;
        double startTimeGather = 0;
        double endTimeGather = 0;

        // Blocking Scattering.
        mpiSetup_.workerInput1_.resize(mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_]);  // Resize each process to receive appropriate data.
        mpiSetup_.workerInput2_.resize(mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_]);

        startTimeScatter = MPI_Wtime();  // Get the start time before scattering.
        MPI_Scatterv(&mpiSetup_.mainInput1_[0],
                    &mpiSetup_.numDataPerProcess_[0],
                    &mpiSetup_.displacementIndices_[0],
                    MPI_FLOAT,
                    &mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    MPI_COMM_WORLD);

        MPI_Scatterv(&mpiSetup_.mainInput2_[0],
                    &mpiSetup_.numDataPerProcess_[0],
                    &mpiSetup_.displacementIndices_[0],
                    MPI_FLOAT,
                    &mpiSetup_.workerInput2_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    MPI_COMM_WORLD);

        endTimeScatter = MPI_Wtime();  // Get the end time after scattering.

        if (mpiSetup_.processRank_ != rootProcess_) {  // Only for worker processes.
            for (size_t i = 0; i < mpiSetup_.workerInput1_.size(); i++) {
            mpiSetup_.workerInput1_[i] += mpiSetup_.workerInput2_[i];
            }
        }

        startTimeGather = MPI_Wtime();  // Get the start time before gathering.

        // Blocking Gathering.
        MPI_Gatherv(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    &mpiSetup_.collectedOutput_[0],
                    &mpiSetup_.numDataPerProcess_[0],
                    &mpiSetup_.displacementIndices_[0],
                    MPI_FLOAT,
                    rootProcess_,
                    MPI_COMM_WORLD);

        endTimeGather = MPI_Wtime();  // Get the end time after gathering.

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only the root process prints the results.
            printResultsAndCheck(mpiSetup_.validationReference_,
                                mpiSetup_.collectedOutput_,
                                mpiSetup_.taskDistribution_,
                                mpiSetup_.displacementIndices_,
                                mpiSetup_.numDataPerProcess_);

            returnValue.first = (endTimeScatter - startTimeScatter) * 1000;  // Calculate scatter time in milliseconds.
            returnValue.second = (endTimeGather - startTimeGather) * 1000;  // Calculate gather time in milliseconds.
        }

        return returnValue;
    }



    // Perform non-blocking send and receive operation using MPI. Returns the time taken for send and receive.

    std::pair<float, float> nonBlockingSend(MPISetup& mpiSetup_) {
        std::pair<float, float> returnValue;

        double startTimeRootSend = 0;
        double endTimeRootSend = 0;
        double startTimeRootRecv = 0;
        double endTimeRootRecv = 0;

        MPI_Request requestRootSend[2];
        MPI_Request requestRootRecv;
        MPI_Request requestWorkerSend;
        MPI_Request requestWorkerRecv[1];

        std::cout.setf(std::ios::fixed, std::ios::floatfield);

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            startTimeRootSend = MPI_Wtime();  // Get the start time before sending.

            // Send input data from root process to worker processes.
            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Issend(&mpiSetup_.mainInput1_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootSend[0]);  // Tag is 0

            MPI_Issend(&mpiSetup_.mainInput2_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootSend[1]);

            MPI_Waitall(2, requestRootSend, MPI_STATUS_IGNORE);
            }

            endTimeRootSend = MPI_Wtime();  // Get the end time after sending.
        }

        if (mpiSetup_.processRank_ != rootProcess_) {  // Only for worker processes.
            // Receive input data from the root process.
            MPI_Irecv(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerRecv[0]);

            MPI_Irecv(&mpiSetup_.workerInput2_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerRecv[1]);

            MPI_Waitall(2, requestWorkerRecv, MPI_STATUS_IGNORE);

            // Perform computation on the received input data.
            for (size_t i = 0; i < mpiSetup_.workerInput1_.size(); i++) {
            mpiSetup_.workerInput1_[i] += mpiSetup_.workerInput2_[i];
            }

            // Send the computed data back to the root process.
            MPI_Issend(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerSend);  // Tag is 0

            MPI_Wait(&requestWorkerSend, MPI_STATUS_IGNORE);
        }

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            startTimeRootRecv = MPI_Wtime();  // Get the start time before receiving.

            // Receive computed data from worker processes.
            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Irecv(&mpiSetup_.collectedOutput_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootRecv);

            MPI_Wait(&requestRootRecv, MPI_STATUS_IGNORE);
            }

            endTimeRootRecv = MPI_Wtime();  // Get the end time after receiving.

            // Print results and check for correctness.
            printResultsAndCheck(mpiSetup_.validationReference_,
                                mpiSetup_.collectedOutput_,
                                mpiSetup_.taskDistribution_,
                                mpiSetup_.displacementIndices_,
                                mpiSetup_.numDataPerProcess_);

            // Calculate the time durations in milliseconds.
            returnValue.first = (endTimeRootSend - startTimeRootSend) * 1000;
            returnValue.second = (endTimeRootRecv - startTimeRootRecv) * 1000;
        }

        return returnValue;
    }




    // Perform blocking send and receive operation using MPI. Returns the time taken for send and receive.

    std::pair<float, float> blockingSend(MPISetup& mpiSetup_) {
        std::pair<float, float> returnValue;

        double startTimeRootSend = 0;
        double endTimeRootSend = 0;
        double startTimeRootRecv = 0;
        double endTimeRootRecv = 0;

        std::cout.setf(std::ios::fixed, std::ios::floatfield);

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            startTimeRootSend = MPI_Wtime();  // Get the start time before sending.

            // Send input data from root process to worker processes.
            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Send(&mpiSetup_.mainInput1_[mpiSetup_.displacementIndices_[i]],
                    mpiSetup_.numDataPerProcess_[i],
                    MPI_FLOAT,
                    i,
                    0,
                    MPI_COMM_WORLD);  // Tag is 0

            MPI_Send(&mpiSetup_.mainInput2_[mpiSetup_.displacementIndices_[i]],
                    mpiSetup_.numDataPerProcess_[i],
                    MPI_FLOAT,
                    i,
                    0,
                    MPI_COMM_WORLD);
            }

            endTimeRootSend = MPI_Wtime();  // Get the end time after sending.
        }

        if (mpiSetup_.processRank_ != rootProcess_) {  // Only for worker processes.
            // Receive input data from the root process.
            MPI_Recv(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

            MPI_Recv(&mpiSetup_.workerInput2_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);

            // Perform computations using the received input data.
            for (size_t i = 0; i < mpiSetup_.workerInput1_.size(); i++) {
            mpiSetup_.workerInput1_[i] += mpiSetup_.workerInput2_[i];
            }

            // Send the computed data back to the root process.
            MPI_Send(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD);  // Tag is 0
        }

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            startTimeRootRecv = MPI_Wtime();  // Get the start time before receiving.

            // Receive computed data from worker processes.
            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Recv(&mpiSetup_.collectedOutput_[mpiSetup_.displacementIndices_[i]],
                    mpiSetup_.numDataPerProcess_[i],
                    MPI_FLOAT,
                    i,
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
            }

            endTimeRootRecv = MPI_Wtime();  // Get the end time after receiving.

            // Print out the results and check if they match the reference output.
            printResultsAndCheck(mpiSetup_.validationReference_,
                                mpiSetup_.collectedOutput_,
                                mpiSetup_.taskDistribution_,
                                mpiSetup_.displacementIndices_,
                                mpiSetup_.numDataPerProcess_);

            returnValue.first = (endTimeRootSend - startTimeRootSend) * 1000;  // Convert to milliseconds.
            returnValue.second = (endTimeRootRecv - startTimeRootRecv) * 1000;  // Convert to milliseconds.
        }

        return returnValue;
    }



    // Perform non-blocking send and receive operation with multiple tasks using MPI. 
    //Returns the time taken for send and receive.

    std::pair<float, float> multiNonBlockingSend(MPISetup& mpiSetup_) {
        std::pair<float, float> returnValue;
        int lastPointCount = 0;
        double startTimeRootSend = 0;
        double endTimeRootSend = 0;
        double startTimeRootRecv = 0;
        double endTimeRootRecv = 0;

        MPI_Request requestRootSend[2];
        MPI_Request requestRootRecv;
        MPI_Request requestWorkerSend;
        MPI_Request requestWorkerRecv[2];

        std::cout.setf(std::ios::fixed, std::ios::floatfield);

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            startTimeRootSend = MPI_Wtime();  // Get the start time before sending.

            int flage = 0;  // Set the operation flag to processed.
            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Issend(&mpiSetup_.mainInput1_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootSend[0]);  // Tag is 0

            MPI_Issend(&mpiSetup_.mainInput2_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootSend[1]);

            do {
                MPI_Testall(2, requestRootSend, &flage, MPI_STATUS_IGNORE);  // Check the flag for completion.
                for (; lastPointCount < vectorSize_ && !flage; lastPointCount++) {
                // Perform the summing while waiting for the sending request to complete.
                mpiSetup_.validationReference_[lastPointCount] = mpiSetup_.mainInput1_[lastPointCount] + mpiSetup_.mainInput2_[lastPointCount];
                MPI_Testall(2, requestRootSend, &flage, MPI_STATUS_IGNORE);  // Check the flag for completion.
                }
            } while (!flage);
            }

            endTimeRootSend = MPI_Wtime();  // Get the end time after sending.
        }

        if (mpiSetup_.processRank_ != rootProcess_) {  // Only for worker processes.
            // Receive input data from the root process.
            MPI_Irecv(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerRecv[0]);

            MPI_Irecv(&mpiSetup_.workerInput2_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerRecv[1]);

            MPI_Waitall(2, requestWorkerRecv, MPI_STATUS_IGNORE);  // Wait for the receive operations to complete.

            // Perform computations using the received input data.
            for (long unsigned int i = 0; i < mpiSetup_.workerInput1_.size(); i++) {
            mpiSetup_.workerInput1_[i] += mpiSetup_.workerInput2_[i];
            }

            // Send the computed data back to the root process.
            MPI_Issend(&mpiSetup_.workerInput1_[0],
                    mpiSetup_.numDataPerProcess_[mpiSetup_.processRank_],
                    MPI_FLOAT,
                    rootProcess_,
                    0,
                    MPI_COMM_WORLD,
                    &requestWorkerSend);  // Tag is 0

            MPI_Wait(&requestWorkerSend, MPI_STATUS_IGNORE);  // Wait for the send operation to complete.
        }

        if (mpiSetup_.processRank_ == rootProcess_) {  // Only for the root process.
            int flage2 = 0;  // Set the operation flag to processed.
            startTimeRootRecv = MPI_Wtime();  // Get the start time before receiving.

            for (int i = 1; i < mpiSetup_.numProcesses_; i++) {
            MPI_Irecv(&mpiSetup_.collectedOutput_[mpiSetup_.displacementIndices_[i]],
                        mpiSetup_.numDataPerProcess_[i],
                        MPI_FLOAT,
                        i,
                        0,
                        MPI_COMM_WORLD,
                        &requestRootRecv);

            do {
                MPI_Test(&requestRootRecv, &flage2, MPI_STATUS_IGNORE);  // Check the flag for completion.
                for (; lastPointCount < vectorSize_ && !flage2; lastPointCount++) {
                // Perform the summing while waiting for the receiving request to complete.
                mpiSetup_.validationReference_[lastPointCount] = mpiSetup_.mainInput1_[lastPointCount] + mpiSetup_.mainInput2_[lastPointCount];
                MPI_Test(&requestRootRecv, &flage2, MPI_STATUS_IGNORE);  // Check the flag for completion.
                }
            } while (!flage2);
            }

            endTimeRootRecv = MPI_Wtime();  // Get the end time after receiving.

            for (; lastPointCount < vectorSize_; lastPointCount++) {
            // Perform the summing for the remaining data points.
            mpiSetup_.validationReference_[lastPointCount] = mpiSetup_.mainInput1_[lastPointCount] + mpiSetup_.mainInput2_[lastPointCount];
            }

            printResultsAndCheck(mpiSetup_.validationReference_,
                                mpiSetup_.collectedOutput_,
                                mpiSetup_.taskDistribution_,
                                mpiSetup_.displacementIndices_,
                                mpiSetup_.numDataPerProcess_); // Print the results and check if they match.

            returnValue.first = (endTimeRootSend - startTimeRootSend) * 1000;  // Convert to milliseconds.
            returnValue.second = (endTimeRootRecv - startTimeRootRecv) * 1000;  // Convert to milliseconds.
        }

        return returnValue;
    }

    // Compare and print the execution times for selected MPI functions. It takes the 
    // execution times, the number of choices, decimal precisions, and the run count.

    void compareExecutionTimes(const std::vector<std::pair<float, float>>& executionTimes,
                            int numChoices,
                            const std::vector<int>& decimalPrecisions,
                            int runCount) {
        std::cout.setf(std::ios::fixed, std::ios::floatfield);

        int scatterIndex = 0;
        int decimalIndex = 0;

        // Print the method names for non-zero execution times
        for (long unsigned int i = 0; i < executionTimes.size(); ++i) {
            if (executionTimes[i].first) {
            switch (i) {
                case 0:
                std::cout << "\n\t\t(1) Non-Blocking Scatter" << std::endl;
                break;
                case 1:
                std::cout << "\n\t\t(2) Blocking Scatter" << std::endl;
                break;
                case 2:
                std::cout << "\n\t\t(3) Non-Blocking Send and Receive" << std::endl;
                break;
                case 3:
                std::cout << "\n\t\t(4) Blocking Send and Receive" << std::endl;
                break;
                case 4:
                std::cout << "\n\t\t(5) Non-Blocking Send and Receive with Multiple Tasks" << std::endl;
                break;
                default:
                std::cout << "\nSomething went wrong!\n";
            }
            }
        }

        std::cout << "\n\n\t=============================================================";
        std::cout << "\n\t|| Func ||  Scatter/Send ||   Gather/Receive  || Number Run||";
        std::cout << "\n\t=============================================================";

        // Print the execution times and related information
        for (long unsigned int i = 0; i < executionTimes.size(); ++i) {
            if (executionTimes[i].first) {
            if (decimalIndex < scatterIndex) {
                std::cout << "\n\t------------------------------------------------------------";
            }
            std::cout.flags(std::ios::fixed | std::ios::showpoint);
            std::cout.precision(decimalPrecisions[decimalIndex]);
            std::cout << "\n\t||  " << std::setw(1) << decimalPrecisions[decimalIndex] << "   ||     " << std::setw(5) << executionTimes[i].first
                        << "    ||        " << std::setw(5) << executionTimes[i].second << "     ||    " << std::setw(3) << runCount
                        << "    ||";
            scatterIndex += 2;
            ++decimalIndex;
            }
        }

        std::cout << "\n\t=============================================================\n\n";
    }
};


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);                            // Initialize the MPI communicator environment.
    parseCommands(argc,argv);
    // Initialize MPIAdditionCalculation with provided values
    MPISetup mpiSetup(userChoice_, vectorSize_, avgRunCount_);
    // Perform calculations
    MPICalculation mpiCalculation(mpiSetup);

    mpiCalculation.performCalculations();
    


    MPI::Finalize();  // Finalize the MPI environment.

    return 0;
}



void parseCommands(int argc, char* argv[]){
    int c;

  // Parsing command-line arguments
  while ((c = getopt(argc, argv, "s:r:n:")) != -1) {
    switch (c) {
      case 's':
        try {
          vectorSize_ = std::stoll(optarg, nullptr, 0);  // Set the vector size based on the command-line argument.
        } catch (std::exception& err) {
          std::cout << "\n\tError: Argument must be an integer!";
          std::cout << "\n\t" << err.what() << std::endl;
        }
        break;
      case 'r':
        try {
          userChoice_ = std::stoll(optarg, nullptr, 0);  // Set the user's choice based on the command-line argument.
          //userSelectedFunctions_ = convertIntToVector(userChoice_);  // Convert the user's choice into a vector of function numbers.
        } catch (std::exception& err) {
          std::cout << "\n\tError: Argument must be an integer!";
          std::cout << "\n\t" << err.what() << std::endl;
        }
        break;
      case 'n':
        try {
          avgRunCount_ = std::stoll(optarg, nullptr, 0);  // Set the average run count based on the command-line argument.
        } catch (std::exception& err) {
          std::cout << "\n\tError: Argument must be an integer!";
          std::cout << "\n\t" << err.what() << std::endl;
        }
        break;
      default:
        abort();
    }
  }

}
