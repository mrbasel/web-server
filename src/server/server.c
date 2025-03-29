#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include <poll.h>
#include "server/server.h"
#include "server/fds_list.h"
#include "socket.h"
#include "http/utils.h"
#include "arena/arena.h"
#include "http/constants.h"
#include "pool/pool.h"

static int handle_request(void* arg) {
    Arena* arena = arena_init(ARENA_SIZE);
    if (arena == NULL) {
        fprintf(stderr, "Error: failed to create arena\n");
        return 1;
    }
    RequestArgs* args = (RequestArgs*)arg;
    int socket = args->socket;
    RequestHandler handler = args->handler;
    char* buffer = args->buffer;

    HttpRequest* parsed_request = parse_request(buffer, arena);
    if (parsed_request == NULL) {
        free(args);
        fprintf(stderr, "Error: could not parse request\n");
        return 1;
    }

    HttpResponse* response = create_response(parsed_request, handler, arena);
    if (response == NULL) {
        fprintf(stderr, "Error: could not create response\n");
        return 1;
    }
    log_http_transaction(socket, parsed_request, response);

    char* serialized_response = serialize_response(response, arena);
    send(socket, serialized_response, strlen(serialized_response), 0);

    arena_free(arena);
    free(args);
    return 0;
}

static void delete_expired_sockets(Server* server, FDS_LIST* fds_list) {
    time_t current_time = time(NULL);
    for (int i = 0; i < fds_list->size; i++) {
        if (fds_list->array[i].fd == server->_socket_fd) continue;
        if ((current_time - fds_list->fd_timestamps[i]) > 5) {
            close(fds_list->array[i].fd);
            fds_list_delete(fds_list, i);
        }
    }
}

Server* create_server(int port) {
    Server* server = malloc(sizeof(Server));
    if (server == NULL) {
        fprintf(stderr, "Error: failed to create server\n");
        return NULL;
    }
    server->port = port;

    struct sockaddr_in* socket_addr = create_server_sockaddr(port);
    int socket_fd = create_server_socket(socket_addr);
    server->_socket_fd = socket_fd;
    server->_socket_addr = socket_addr;
    return server;
}

void free_server(Server* server) {
    free(server->_socket_addr);
    free(server);
}

void server_listen(Server* server, RequestHandler handler, size_t workers) {
    Pool* pool = pool_init(workers);
    FDS_LIST* fds_list = fds_list_init();
    
    // add listener socket to array
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server->_socket_fd;
    server_poll_fd.events = POLLIN;
    fds_list_insert(fds_list, server_poll_fd);

    int accepted_socket;
    while(1) {
        int events_count = poll(fds_list->array, fds_list->size, 5000); 
        if (events_count > 0) {
            int size = fds_list->size;
            for (int i = 0; i < size; i++) {
                if (fds_list->array[i].fd > 0) {
                    if (fds_list->array[i].revents & POLLNVAL) {
                        close(fds_list->array[i].fd);
                        fds_list_delete(fds_list, i);
                    }
                    else if (fds_list->array[i].revents & POLLIN) {
                        if (fds_list->array[i].fd == server->_socket_fd) {
                            accepted_socket = accept_connection(server->_socket_fd, server->_socket_addr);
                            if (accepted_socket > 0) {
                                struct pollfd poll_fd;
                                poll_fd.fd = accepted_socket;
                                poll_fd.events = POLLIN;
                                fds_list_insert(fds_list, poll_fd);
                            }
                        }
                        else {
                            RequestArgs* args = malloc(sizeof(RequestArgs));
                            if (args == NULL) {
                                fprintf(stderr, "Error: could not process request\n");
                                continue;
                            }
                            args->socket = fds_list->array[i].fd;
                            args->handler = handler;

                            int bytes_read = read(args->socket, args->buffer, BUFFER_SIZE);
                            if (bytes_read < 1) {
                                close(fds_list->array[i].fd);
                                fds_list_delete(fds_list, i);
                                free(args);
                                continue;
                            }
                            pool_add_work(pool, (void (*)(void*))handle_request, args);
                        }
                    }
                }
            }
        }
        delete_expired_sockets(server, fds_list);
        fds_purge(fds_list);
    }
    fds_list_free(fds_list);
    free_server(server);
    pool_free(pool);
}
