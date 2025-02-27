
typedef struct Pool {
    int queue[];
    thread_t workers[];
} Pool;
