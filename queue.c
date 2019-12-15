#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include  <sys/shm.h>

int isEmpty(struct queue *q) {
    return q->totalItemCount == 0;
}

int isFull(struct queue *q) {
    return q->totalItemCount == MAX;
}

int size(struct queue *q) {
    return q->totalItemCount;
}

int insertCommand(struct message command, struct queue *q, sem_t* mutex) {
    sem_wait(mutex);
    int result;
    if (!isFull(q)) {
        if (command.priority) {
            if (MAX == q->prioQueue.rear + 1) {
                q->prioQueue.rear = -1;
            }
            q->prioQueue.rear++;
            q->prioQueue.buffor[q->prioQueue.rear] = command;
            q->prioQueue.itemCount++;
        } else {
            if (MAX == q->queue.rear + 1) {
                q->queue.rear = -1;
            }
            q->queue.rear++;
            q->queue.buffor[q->queue.rear] = command;
            q->queue.itemCount++;
        }
        q->totalItemCount++;
        result = 1;
    } else {
        result = 0;
    }
    sem_post(mutex);
    return result;
}

struct message popCommand(struct queue *q, sem_t* mutex) {
    sem_wait(mutex);
    q->totalItemCount--;
    struct message result;
    if (q->prioQueue.itemCount) {
        q->prioQueue.itemCount--;

        if (q->prioQueue.front == (MAX - 1)) {
            q->prioQueue.front = 0;
            result = q->prioQueue.buffor[(MAX - 1)];
        } else {
            result = q->prioQueue.buffor[q->prioQueue.front++];
        }
    } else {
        q->queue.itemCount--;

        if (q->queue.front == (MAX - 1)) {
            q->queue.front = 0;
            result = q->queue.buffor[(MAX - 1)];
        } else {
            result = q->queue.buffor[q->queue.front++];
        }
    }
    sem_post(mutex);
    return result;
}

int allocSHMForQueue(key_t key) {
    //metoda serwera 1, klient 1
    return shmget(key, sizeof(struct queue), IPC_CREAT);
}

int allocSHMForMutex(key_t key) {
    //metoda serwera 2
    return shmget(key, sizeof(sem_t), IPC_CREAT);
}

struct queue* atSHMForQueue(int shm_id) {
    // metoda wszystkich (serwer 3, other 2)
    return (struct queue *) shmat(shm_id, NULL, 0);
}

sem_t* atSHMForMutex(int shm_id) {
    // metoda wszystkich (serwer 3, other 2)
    return (sem_t*) shmat(shm_id, NULL, 0);
}

void initMutex(sem_t* mutex) {
    sem_init(mutex, 1, 1);
}

void detachFromQueue(struct queue *q, sem_t* mutex) {
    // metoda klienta 3
    shmdt(mutex);
    shmdt(q);
}

void destroyQueue(int q_shm_id, int m_shm_id, struct queue *q, sem_t* mutex) {
    // metoda serwera 5
    sem_destroy(mutex);
    shmctl(m_shm_id, IPC_RMID, NULL);
    shmdt(mutex);
    shmctl(q_shm_id, IPC_RMID, NULL);
    shmdt(q);
}

struct message generateMessage(int prio) {
    struct message result;
    result.priority = prio;
    result.message[0] = generateSign();
    result.message[1] = generateSign();
    result.message[2] = generateSign();
    return result;
}

char generateSign() {
    int tmp = random() % 3;
    switch (tmp) {
        case 0:
            return 'A';
        case 1:
            return 'B';
        case 2:
            return 'C';
    }
}

int doWithProbabilty(int pr) {
    return (random() % 100) >= pr;
}

int checkLength(struct message msg) {
    int i;
    for (i = 0; i <= 2; i++) {
        if(msg.message[i] == 'X') {
            break;
        }
    }
    return i;
}


