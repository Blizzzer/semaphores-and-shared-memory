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
struct queue *bQueue;
sem_t *bMutex;
struct message toInsert;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        sig_caught = true;
    }
}

int do_something(void) {
    delay(prodPeriod);

    toInsert = generateMessage(0);

    if (insertCommand(toInsert, bQueue, bMutex)) {
        printf("message %s inserted \n ", toInsert.message);
    } else {
        printf("queue is full \n");
    }

    return 0;
}

int main() {
    printf("Zgłaszam się producent B \n");

    srandom(time(NULL));

    key_t bQueKey = 12;
    int bQueShmId;

    key_t bMutKey = 22;
    int bMutShmId;

    bQueShmId = allocSHMForQueue(bQueKey);
    bQueue = atSHMForQueue(bQueShmId);
    bMutShmId = allocSHMForMutex(bMutKey);
    bMutex = atSHMForMutex(bMutShmId);

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "signal func registration failed\n");
        return 1;
    }

    printf("Producent B ready \n");

    for (;;) {
        do_something();
        if (sig_caught) {
            printf("caught signal - exiting \n");
            detachFromQueue(bQueue, bMutex);
            printf("detached from queue \n");
            return 0;
        }
    }
}