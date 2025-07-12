#ifndef POOL_H
#define POOL_H

#include <bits/pthreadtypes.h>
#include <stddef.h>
struct RequestArgs;
typedef struct RequestArgs RequestArgs;

typedef void (*work_func)(void *arg);

typedef struct PoolWork {
    void* args;
    work_func func;
    struct PoolWork* next;
} PoolWork;

typedef struct Pool {
    int workers_count;
    PoolWork* queue_head;
    PoolWork* queue_tail;
    pthread_mutex_t mutex;
    pthread_cond_t has_work;
    int should_stop;
} Pool;

typedef struct WorkerArgs {
    Pool* pool;
    int i;
} WorkerArgs;

Pool* pool_init(size_t num_workers);

int pool_add_work(Pool* pool, work_func func, void* args);

void pool_free(Pool* pool);

#endif
