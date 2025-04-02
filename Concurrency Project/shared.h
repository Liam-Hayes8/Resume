#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>
#include <queue>
#include <string>

using namespace std;

//Seperate all files
struct SharedData{

    //Bounded buffer (FIFO queue)
    queue<string> requestQueue;

    
    sem_t empty;    //Empty Slots
    sem_t full;     //Requests in queue
    sem_t mutex;    //Mutex access to queue

    //Progress indicators
    int processedRequests;
    int totalRequests;

    //Delays read from command-line in microseconds
    int producerDelay;
    int consumerDelay;

    //Store request file path
    string requestFilePath;

    //Max queue size
    int maxQueueSize;
};

#endif