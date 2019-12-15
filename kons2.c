//
// Created by kapol on 12.12.2019.
//
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include <stdbool.h>
#include <signal.h>
#include <time.h>

int pr = 30;

void delay(float number_of_seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (number_of_seconds * CLOCKS_PER_SEC));
}

bool sig_caught = false;
struct queue *aQueue;
sem_t *aMutex;

struct queue *bQueue;
sem_t *bMutex;

struct queue *bQueue;
sem_t *bMutex;
struct message popped;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        sig_caught = true;
    }
}

char consume() {
    char consumedSign = popped.message[0];
    switch (checkLength(popped)) {
        case 1:
            if (doWithProbabilty(pr)) {
                popped.message[0] = generateSign();
            } else {
                popped.message[0] = 'X';
            }
        case 2:
            popped.message[0] = popped.message[1];
            if (doWithProbabilty(pr)) {
                popped.message[1] = generateSign();
            } else {
                popped.message[1] = 'X';
            }
        case 3:
            popped.message[0] = popped.message[1];
            popped.message[1] = popped.message[2];
            if (doWithProbabilty(pr)) {
                popped.message[2] = generateSign();
            } else {
                popped.message[2] = 'X';
            }
    }
    return consumedSign;
}

void specialInsert(char consumedSign) {
    switch (consumedSign) {
        case 'A':
            insertCommand(popped, aQueue, aMutex);
            return;
        case 'B':
            insertCommand(popped, bQueue, bMutex);
            return;
        case 'C':
            insertCommand(popped, bQueue, bMutex);
            return;
    }
}

int do_something(void) {
    popped = popCommand(bQueue, bMutex);
    delay(0.5f);
    if (checkLength(popped)) {
        printf("received message %s \n", popped.message);
        specialInsert(consume());
    } else {
        printf("dumping empty message \n");
    }
    return 0;
}

int main() {
    // Konsumuje z kolejki B
    printf("Zgłaszam się konsument B \n");

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

    printf("konsument B ready \n");

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