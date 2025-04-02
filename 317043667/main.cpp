#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include "shared.h"
#include "receiveAndBufferRequests.h"
#include "processRequests.h"

/*
Liam Hayes
Red ID: 827393886
*/

    //Function prototypes for thread routines
    void* producer(void* arg);
    void* consumer(void* arg);

    using namespace std;

//Checks if file can be opened first
//Counting the number of lines in a file
static int countLines(const char* filePath){
    ifstream file(filePath);
    if(!file.is_open()){
        return -1;              //Return -1  if file can't be found
    }
    int lineCount = 0;
    string temp;
    while(getline(file, temp)){
        lineCount++;
    }
    return lineCount;           //If found, return total number of lines
}


int main(int argc, char* argv[]){

    //Default parameters for delays (in microseconds)
    // queue size, progress marks, and hash interval
    int producerDelay = 50;
    int consumerDelay = 50;
    int maxQueueSize = 100;
    int progressMarks = 50;
    int hashMarkInterval = 10;
    char* requestFilePath = nullptr;        //To store the path to the request file

    //Process the command line arguments starting from argv[1]
    //Check for flags and parameters
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-'){              //Options begin with hyphen
            if(strcmp(argv[i], "-t") == 0){
                if(i + 1 >= argc){
                    cerr << "Missing argument for -t" << endl;
                    return EXIT_FAILURE;
                }
                producerDelay = atoi(argv[i + 1]);
                if(producerDelay <= 0){
                    cout << "Time for producing a request must be a number and greater than 0" << endl;
                    return EXIT_FAILURE;
                }
                i++;                        //Skip next argument since it was consumed as delay value
            }
            else if(strcmp(argv[i], "-c") == 0){
                if(i + 1 >= argc){
                    cerr << "Missing argument for -c" << endl;
                    return EXIT_FAILURE;
                }
                consumerDelay = atoi(argv[i + 1]);
                if(consumerDelay <= 0){
                    cout << "Time for consuming a request must be a number and greater than 0" << endl;
                    return EXIT_FAILURE;
                }
                i++;                        //Skip next argument
            }
            else if(strcmp(argv[i], "-q") == 0){
                if(i + 1 >= argc){
                    cerr << "Missing argument for -q" << endl;
                    return EXIT_FAILURE;
                }
                maxQueueSize = atoi(argv[i + 1]);
                if(maxQueueSize < 1){
                    cout << "Max size of request queue must be a number and greater than 0" << endl;
                    return EXIT_FAILURE;
                }
                i++;                        //Skip next argument
            }
            else if(strcmp(argv[i], "-p") == 0){
                if(i + 1 >= argc){
                    cerr << "Missing argument for -p" << endl;
                    return EXIT_FAILURE;
                }
                progressMarks = atoi(argv[i + 1]);
                if(progressMarks < 10){
                    cout << "Number of progress marks must be a number and at least 10" << endl;
                    return EXIT_FAILURE;
                }
                i++;                        //Skip next argument
            }
            else if(strcmp(argv[i], "-h") == 0){
                if(i + 1 >= argc){
                    cerr << "Missing argument for -h" << endl;
                    return EXIT_FAILURE;
                }
                hashMarkInterval = atoi(argv[i + 1]);
                if(hashMarkInterval <= 0 || hashMarkInterval > 10){
                    cout << "Hash mark interval for progress must be a number, greater than 0, and less than or equal to 10" << endl;
                    return EXIT_FAILURE;
                }
                i++;                        //Skip next argument
            }
            else{
                //If option is unknown, output error and exit
                cout << "Unknown option: " << argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
        else{
            //If argument doesn't start with '-', assume it's the request file path
            if(requestFilePath == nullptr){
                requestFilePath = argv[i];
            }
            else{
                //If more than one non-option argument, output error and exit
                cerr << "Unknown option: " <<  argv[i] << endl;
                return EXIT_FAILURE;
            }
        }
    }
    //Ensure request file path is provided
    if(requestFilePath == nullptr){
        cerr << "Expected request file argument" << endl;
        return EXIT_FAILURE;
    }
        
    //Count total requests in the request file
    int totalRequests = countLines(requestFilePath);
    if(totalRequests < 0){
        cerr << "Unable to open " << requestFilePath << endl;
        return EXIT_FAILURE;
    }

    SharedData data;

    //Initialize shared data structure
    data.processedRequests = 0;
    data.totalRequests = totalRequests;
    data.producerDelay = producerDelay;
    data.consumerDelay = consumerDelay;
    data.requestFilePath = requestFilePath;
    data.maxQueueSize = maxQueueSize;

    //Initialize Semapores
    sem_init(&data.empty, 0, data.maxQueueSize);
    sem_init(&data.full, 0, 0);
    sem_init(&data.mutex, 0, 1);

    //Create producer and consumer threads
    pthread_t producerThread, consumerThread;
    pthread_create(&producerThread, nullptr, producer, &data);
    pthread_create(&consumerThread, nullptr, consumer, &data);

    //Displaying progress bar in main thread
    int marksPrinted = 0;

    while(data.processedRequests < data.totalRequests){
        //Find fraction of requests processed
        double fraction = 0.0;
        if(data.totalRequests > 0){
            fraction = (double)data.processedRequests / data.totalRequests;
        }
    
        //How many progress marks need to be printed
        int totalNeeded = (int)(fraction * progressMarks);

        //Print new marks if needed
        while(marksPrinted < totalNeeded && marksPrinted < progressMarks){
            marksPrinted++;

            //Print # every hashMarkInterval, else -> -
            if((marksPrinted % hashMarkInterval) == 0){
                cout << "#";
            }
            else{
                cout << "-";
            }
            cout.flush();
        }


        //20ms break before checking again
        usleep(20000);
    }

    //If bar didn't compute fully, fill
    while(marksPrinted < progressMarks){
        marksPrinted++;
        if((marksPrinted % hashMarkInterval) == 0){
            cout << "#";
        }
        else{
            cout << "-";
        }
        cout.flush();
    }
    cout << endl;

    //Wait for threads to complete and join them
    pthread_join(producerThread, nullptr);
    pthread_join(consumerThread, nullptr);

    //Print status message
    cout << data.processedRequests << " requests were received and processed." << endl;

    //Clean up semaphores
    sem_destroy(&data.empty);
    sem_destroy(&data.full);
    sem_destroy(&data.mutex);

    return EXIT_SUCCESS;
}