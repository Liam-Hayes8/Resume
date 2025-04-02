#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "shared.h"
#include "receiveAndBufferRequests.h"

using namespace std;

void* producer(void* arg){

    SharedData* data = static_cast<SharedData*>(arg);
    
    //Open file request
    ifstream inFile(data->requestFilePath);
    if(!inFile){
        cerr << "Unable to open " << data->requestFilePath << " in producer thread.\n";
        pthread_exit(nullptr);
    }
    string line;
    while(getline(inFile, line)){
        //Production delay
        usleep(data->producerDelay);

        sem_wait(&data->empty);     //Ensure room in buffer -- availableSlots.wait
        sem_wait(&data->mutex);     //Acces to buffer exclusively -- mutex.wait

        data->requestQueue.push(line);

        sem_post(&data->mutex);     //Release exclusive access -- mutex.signal
        sem_post(&data->full);      // Signal consumer that an item is available -- unconsumed.signal

    }
    inFile.close();
    pthread_exit(nullptr);
}