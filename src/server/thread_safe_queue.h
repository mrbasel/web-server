#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <stdlib.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 100

typedef struct ThreadSafeQueue {
    int* array;
    int capacity;
    int front;
    int rear;
    pthread_mutex_t mutex;
} ThreadSafeQueue;

ThreadSafeQueue* queue_init() {
    ThreadSafeQueue* queue = malloc(sizeof(ThreadSafeQueue));
    if (queue == NULL) return NULL;

    queue->array = malloc(MAX_QUEUE_SIZE * sizeof(int));
    if (queue->array == NULL) {
        free(queue);
        return NULL;
    }
    queue->front = -1;
    queue->rear = -1;
    queue->capacity = 0;
    pthread_mutex_init(&(queue->mutex), NULL);
    return queue;
};

void queue_free(ThreadSafeQueue* queue) {
    pthread_mutex_destroy(&(queue->mutex));
    free(queue->array);
    free(queue);
}

int queue_enqueue(ThreadSafeQueue* queue, int val) {
    pthread_mutex_lock(&(queue->mutex));
    if (queue->capacity >= MAX_QUEUE_SIZE) return -1;

    if (queue->capacity == 0) {
        queue->front = 0;
        queue->rear = 0;
    } else {
        queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    }
    queue->array[queue->rear] = val;
    queue->capacity++;
    pthread_mutex_unlock(&(queue->mutex));
    return 0;
}

int queue_dequeue(ThreadSafeQueue* queue) {
    pthread_mutex_lock(&(queue->mutex));
    if (queue->capacity == 0) {
        pthread_mutex_unlock(&(queue->mutex));
        return -1;
    }

    int val = queue->array[queue->front];

    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    }

    queue->capacity--;
    pthread_mutex_unlock(&(queue->mutex));
    return val;
}

int queue_get_capacity(ThreadSafeQueue* queue) {
    pthread_mutex_lock(&(queue->mutex));
    int capacity = queue->capacity;
    pthread_mutex_unlock(&(queue->mutex));
    return capacity;
}

#endif
