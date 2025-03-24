#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pool/pool.h"

static PoolWork* pool_get_work(Pool* pool) {
    if (pool == NULL || pool->queue_head == NULL) return NULL;

    PoolWork* work = pool->queue_head;
    if (pool->queue_head->next == NULL) {
        pool->queue_head = NULL;
        pool->queue_tail = NULL;
    }
    else pool->queue_head = pool->queue_head->next;
    return work;
}

static void pool_worker(void* arg) {
    Pool* pool = arg;
    while (1) {
        pthread_mutex_lock(&(pool->mutex));

        while (pool->queue_head == NULL) pthread_cond_wait(&(pool->has_work), &(pool->mutex));

        if (pool->should_stop) break;
        PoolWork* work = pool_get_work(pool);
        pthread_mutex_unlock(&(pool->mutex));

        if (work != NULL) {
            work->func(work->args);
            free(work);
        }

    }
}

Pool* pool_init(size_t num_workers) {
    Pool* pool = malloc(sizeof(Pool)); 
    if (pool == NULL) {
        fprintf(stderr, "Error: failed to create workers pool\n");
        return NULL;
    }
    pool->workers_count = num_workers;
    pool->queue_tail = NULL;
    pool->should_stop = 0;

    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_cond_init(&(pool->has_work), NULL);
    pool->queue_head = NULL;
    pool->queue_tail = NULL;

    pthread_t thread;
    for (size_t i = 0; i < num_workers; i++) {
        pthread_create(&thread, NULL, pool_worker, pool);
        pthread_detach(thread);
    }

    return pool;
}

void pool_free(Pool* pool) {
    pthread_mutex_lock(&(pool->mutex));
    
    PoolWork* head = pool->queue_head;
    while (head != NULL) {
        PoolWork* temp = head->next;
        free(head);
        head = temp;
    }
    pool->queue_head = NULL;

    pool->should_stop = 1;
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->has_work));
    free(pool);
}

int pool_add_work(Pool* pool, work_func func, void* args) {
    PoolWork* work = malloc(sizeof(PoolWork));
    if (work == NULL) {
        fprintf(stderr, "Error: failed to add work to pool\n");
        return 1;
    }
    work->args = args;
    work->func = func;
    work->next = NULL;

    pthread_mutex_lock(&(pool->mutex));

    if (!pool->queue_head) pool->queue_head = work;
    else {
        PoolWork* target_tail = pool->queue_head;
        while (target_tail->next != NULL) {
            target_tail = target_tail->next;
        }
        target_tail->next = work;
    }

    pthread_cond_broadcast(&(pool->has_work));
    pthread_mutex_unlock(&(pool->mutex));
    return 0;
}
