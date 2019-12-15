#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int prodPeriod = 3;

void delay(int number_of_seconds) {
    clock_t start_time = clock();
    while (clock() < start_time + (number_of_seconds * CLOCKS_PER_SEC));
}

bool sig_caught = false;
struct queue *cQueue;
sem_t *cMutex;
struct message toInsert;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        sig_caught = true;
    }
}

int do_something(void) {
    delay(prodPeriod);

    toInsert = generateMessage(0);

    if (insertCommand(toInsert, cQueue, cMutex)) {
        printf("message %s inserted \n ", toInsert.message);
    } else {
        printf("queue is full \n");
    }

    return 0;
}

int main() {
    printf("Zgłaszam się producent C \n");

    srandom(time(NULL));

    key_t cQueKey = 13;
    int cQueShmId;

    key_t cMutKey = 23;
    int cMutShmId;

    cQueShmId = allocSHMForQueue(cQueKey);
    cQueue = atSHMForQueue(cQueShmId);
    cMutShmId = allocSHMForMutex(cMutKey);
    cMutex = atSHMForMutex(cMutShmId);

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "signal func registration failed\n");
        return 1;
    }

    printf("Producent C ready \n");

    for (;;) {
        do_something();
        if (sig_caught) {
            printf("caught signal - exiting \n");
            detachFromQueue(cQueue, cMutex);
            printf("detached from queue \n");
            return 0;
        }
    }
}