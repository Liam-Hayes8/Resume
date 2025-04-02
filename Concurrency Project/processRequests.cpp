#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "shared.h"
#include "processRequests.h"

using namespace std;

void* consumer(void* arg){
    SharedData* data = static_cast<SharedData*>(arg);

    //Open output file
    ofstream outFile("processOutputs.txt");
    if(!outFile){
        cerr << "Unable to open file for writing.\n";
        pthread_exit(nullptr);
    }

    while(true){
        //Base case
        if(data->processedRequests >= data->totalRequests){
            break;
        }

        sem_wait(&data->full);       //Wait until there is an item
        sem_wait(&data->mutex);      //Acquire exclusive access

        //Queue size before removal
        int queueSizeBefore = data->requestQueue.size();

        //Remove an item from the buffer
        string item = data->requestQueue.front();
        data->requestQueue.pop();

        sem_post(&data->mutex);     //Release exclusive access
        sem_post(&data->empty);      //Signal there is now an empty slot

        //Consumer delay in microseconds
        usleep(data->consumerDelay);

        //Queue size before removal and length of request string
        outFile << queueSizeBefore << " " << item.size() << "\n";

        //Increment count of processed requests
        data->processedRequests++;

        //Check if processing is complete
        if(data->processedRequests >= data->totalRequests){
            break;
        }

    }
    outFile.close();
    pthread_exit(nullptr);
}