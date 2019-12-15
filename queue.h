#ifndef MYSEMAPHORES_MYSEMAPHORES_H
#define MYSEMAPHORES_MYSEMAPHORES_H

#define MAX 5

#include <semaphore.h>

struct message {
    int priority;
    char message[3];
};

struct simpleQueue {
    struct message buffor[MAX];
    int front;
    int rear;
    int itemCount;
};

struct queue {
    struct simpleQueue prioQueue;
    struct simpleQueue queue;
    int totalItemCount;
};

struct message getCommand(struct queue* q);

int isEmpty(struct queue* q);

int isFull(struct queue* q);

int size(struct queue* q);

int insertCommand(struct message command, struct queue *q, sem_t* mutex);

struct message popCommand(struct queue *q, sem_t* mutex);

int allocSHMForQueue(key_t key);

int allocSHMForMutex(key_t key);

struct queue* atSHMForQueue(int shm_id);

sem_t* atSHMForMutex(int shm_id);

void initMutex(sem_t* mutex);

void detachFromQueue(struct queue *q, sem_t* mutex);

void destroyQueue(int q_shm_id, int m_shm_id, struct queue *q, sem_t* mutex);

char generateSign();

int doWithProbabilty(int pr);

int checkLength(struct message msg);

struct message generateMessage(int prio);

void rollMessage (struct message msg);

#endif //MYSEMAPHORES_MYSEMAPHORES_H
