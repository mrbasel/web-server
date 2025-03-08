#include <poll.h>

#define MAX_SOCKETS_COUNT 500

typedef struct FDS_LIST {
    int size;
    struct pollfd* array;
} FDS_LIST; 

FDS_LIST* fds_list_init();

void fds_list_free(FDS_LIST* fds_list);

void fds_get(FDS_LIST* fds_list, int i);

void fds_list_insert(FDS_LIST* fds_list, struct pollfd fd);

void fds_list_delete(FDS_LIST* fds_list, int i);

// removes fds with value -1
void fds_purge(FDS_LIST* fds_list);
