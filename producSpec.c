//
// Created by kapol on 12.12.2019.
//
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include <stdbool.h>
#include <signal.h>
#include <time.h>

int prodPeriod = 3;

void delay(float number_of_seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (number_of_seconds * CLOCKS_PER_SEC));
}

bool sig_caught = false;
struct queue* aQueue;
sem_t* aMutex;

struct queue* bQueue;
sem_t* bMutex;

struct queue* bQueue;
sem_t* bMutex;

struct message toInsert;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        sig_caught = true;
    }
}

int specialInsert(char consumedSign) {
    switch (consumedSign) {
        case 'A':
            return insertCommand(toInsert, aQueue, aMutex);
        case 'B':
            return insertCommand(toInsert, bQueue, bMutex);
        case 'C':
            return insertCommand(toInsert, bQueue, bMutex);
    }
}

int do_something(void) {
    delay(prodPeriod);

    toInsert = generateMessage(1);

    if (specialInsert(generateSign())) {
        printf("message %s inserted \n ", toInsert.message);
    } else {
        printf("queue is full \n");
    }

    return 0;
}

int main() {
    printf("Zgłaszam się producent specjalny \n");

    srandom(time(NULL));

    key_t aQueKey = 11;
    int aQueShmId;

    key_t bQueKey = 12;
    int bQueShmId;

    key_t cQueKey = 13;
    int cQueShmId;

    key_t aMutKey = 21;
    int aMutShmId;

    key_t bMutKey = 22;
    int bMutShmId;

    key_t cMutKey = 23;
    int cMutShmId;

    aQueShmId = allocSHMForQueue(aQueKey);
    aQueue = atSHMForQueue(aQueShmId);
    aMutShmId = allocSHMForMutex(aMutKey);
    aMutex = atSHMForMutex(aMutShmId);

    bQueShmId = allocSHMForQueue(bQueKey);
    bQueue = atSHMForQueue(bQueShmId);
    bMutShmId = allocSHMForMutex(bMutKey);
    bMutex = atSHMForMutex(bMutShmId);

    cQueShmId = allocSHMForQueue(cQueKey);
    bQueue = atSHMForQueue(cQueShmId);
    cMutShmId = allocSHMForMutex(cMutKey);
    bMutex = atSHMForMutex(cMutShmId);

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "signal func registration failed\n");
        return 1;
    }

    printf("producent specjalny ready \n");

    for (;;) {
        do_something();
        if (sig_caught) {
            printf("caught signal - exiting \n");
            detachFromQueue(aQueue, aMutex);
            detachFromQueue(bQueue, bMutex);
            detachFromQueue(bQueue, bMutex);

            printf("detached from queue \n");
            return 0;
        }
    }
}