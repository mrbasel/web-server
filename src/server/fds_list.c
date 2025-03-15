#include <stdlib.h>
#include "server/fds_list.h"

FDS_LIST* fds_list_init() {
    FDS_LIST* fds_list = malloc(sizeof(FDS_LIST));
    fds_list->array = malloc(sizeof(struct pollfd) * MAX_SOCKETS_COUNT);
    fds_list->size = 0;
    return fds_list;
}

void fds_list_free(FDS_LIST* fds_list) {
    free(fds_list->array);
    free(fds_list);
}

struct pollfd fds_list_get(FDS_LIST* fds_list, int i) {
    return fds_list->array[i];
}

void fds_list_insert(FDS_LIST* fds_list, struct pollfd fd) {
    fds_list->array[fds_list->size].fd = fd.fd; 
    fds_list->array[fds_list->size].events = fd.events; 
    fds_list->size++;
}

void fds_list_delete(FDS_LIST* fds_list, int i) {
    fds_list->array[i].fd = -1;
}

void fds_purge(FDS_LIST* fds_list) {
    for (int i = 0; i < fds_list->size; i++) {
        if (fds_list->array[i].fd == -1) {
            // TODO: handle last element being -1
            fds_list->array[i].fd = fds_list->array[fds_list->size - 1].fd;
            fds_list->array[i].events = fds_list->array[fds_list->size - 1].events;
            fds_list->size--;
        }
    }
}
