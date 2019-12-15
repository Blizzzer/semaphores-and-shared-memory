#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include  <sys/shm.h>
#include "queue.h"
#include <signal.h>
#include <time.h>
#include <semaphore.h>


void delay(int number_of_seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (number_of_seconds * CLOCKS_PER_SEC));
}

clock_t start;

bool sig_caught = false;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        sig_caught = true;
    }
}

int do_something(void) {
    delay(1);
    clock_t current = clock() - start;
    current = current / CLOCKS_PER_SEC;
    printf("%ld second \n", current);
    return 0;
}

int main() {
    //QUEUE A
    struct queue* aQueue;
    key_t aQueKey = 11;
    int aQueShmId;

    key_t aMutKey = 21;
    int aMutShmId;
    sem_t* aMutex;

    //QUEUE B
    struct queue* bQueue;
    key_t bQueKey = 12;
    int bQueShmId;

    key_t bMutKey = 22;
    int bMutShmId;
    sem_t* bMutex;


    //QUEUE C
    struct queue* cQueue;
    key_t cQueKey = 13;
    int cQueShmId;

    key_t cMutKey = 23;
    int cMutShmId;
    sem_t* cMutex;


    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "signal func registration failed\n");
        return 1;
    }

    aQueShmId = allocSHMForQueue(aQueKey);
    bQueShmId = allocSHMForQueue(bQueKey);
    cQueShmId = allocSHMForQueue(cQueKey);
    printf("queue shm allocated \n");

    aMutShmId = allocSHMForMutex(aMutKey);
    bMutShmId = allocSHMForMutex(bMutKey);
    cMutShmId = allocSHMForMutex(cMutKey);
    printf("mutex shm allocated \n");

    aQueue = atSHMForQueue(aQueShmId);
    bQueue = atSHMForQueue(bQueShmId);
    cQueue = atSHMForQueue(cQueShmId);
    printf("pointers to queues obtained \n");

    aMutex = atSHMForMutex(aMutShmId);
    bMutex = atSHMForMutex(bMutShmId);
    cMutex = atSHMForMutex(cMutShmId);
    printf("pointers to mutexes obtained \n");

    initMutex(aMutex);
    initMutex(bMutex);
    initMutex(cMutex);
    printf("mutexes initialized \n");

    printf("Server ready for clients \n");

    start = clock();

    for (;;) {
        do_something();
        if (sig_caught) {
            printf("caught signal - exiting \n");
            destroyQueue(aQueShmId, aMutShmId, aQueue, aMutex);
            destroyQueue(bQueShmId, bMutShmId, bQueue, bMutex);
            destroyQueue(cQueShmId, cMutShmId, cQueue, cMutex);
            printf("all queues destroyed - i am exiting \n");

            return 0;
        }
    }


    return 0;
}
