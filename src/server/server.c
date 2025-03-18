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

Server* create_server(int port) {
    Server* server = malloc(sizeof(Server));
    server->port = port;

    struct sockaddr_in* socket_addr = create_server_sockaddr(port);
    int socket_fd = create_server_socket(socket_addr);
    server->_socket_fd = socket_fd;
    server->_socket_addr = socket_addr;
    return server;
}

int handle_request(void* arg) {
    Arena* arena = arena_init(ARENA_SIZE);
    RequestArgs* args = (RequestArgs*)arg;
    int socket = args->socket;
    RequestHandler handler = args->handler;
    char* buffer = args->buffer;
    int signal_pipe_fd = args->signal_pipe_fd;

    HttpRequest* parsed_request = parse_request(buffer, arena);
    if (parsed_request == NULL) {
        free(args);
        fprintf(stderr, "could not parse request\n");
        return 1;
    }

    HttpResponse* response = create_response(parsed_request, handler, arena);
    log_http_transaction(parsed_request, response);

    char* serialized_response = serialize_response(response, arena);
    send(socket, serialized_response, strlen(serialized_response), 0);

    if (parsed_request->version && strcmp(parsed_request->version, "HTTP/1.0") == 0) {
        // signal main thread to close and socket fd
        write(signal_pipe_fd, &socket, sizeof(socket));
    }

    arena_free(arena);
    free(args);
    return 0;
}

void server_listen(Server* server, RequestHandler handler) {
    Pool* pool = pool_init(4);
    FDS_LIST* fds_list = fds_list_init();
    int pipe_fds[2];
    
    // add listener socket to array
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server->_socket_fd;
    server_poll_fd.events = POLLIN;
    fds_list_insert(fds_list, server_poll_fd);

    if (pipe(pipe_fds) != 0) {
        fprintf(stderr, "could not create signal pipe\n");
        return;
    }
    // add pipe socket to array for signaling socket close
    struct pollfd pipe_poll_fd;
    pipe_poll_fd.fd = pipe_fds[0];
    pipe_poll_fd.events = POLLIN;
    fds_list_insert(fds_list, pipe_poll_fd);
    
    int accepted_socket;
    while(1) {
        int events_count = poll(fds_list->array, fds_list->size, -1); 
        if (events_count > 0) {
            int size = fds_list->size;
            for (int i = 0; i < size; i++) {
                if (fds_list->array[i].fd > 0) {
                    if (fds_list->array[i].revents & POLLNVAL) {
                        close(fds_list->array[i].fd);
                        fds_list_delete(fds_list, i);
                    }
                    else if (fds_list->array[i].revents & POLLIN) {
                        if (fds_list->array[i].fd == pipe_fds[0]) {
                            int fd_to_delete;
                            read(pipe_fds[0], &fd_to_delete, sizeof(fd_to_delete));
                            close(fd_to_delete);
                            for (int j = 0; j < size; j++) {
                                if (fds_list->array[j].fd == fd_to_delete) fds_list_delete(fds_list, j);
                            }
                        }
                        else if (fds_list->array[i].fd == server->_socket_fd) {
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
                            args->socket = fds_list->array[i].fd;
                            args->handler = handler;
                            args->signal_pipe_fd = pipe_fds[1];

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
        // remove fds with value of -1
        fds_purge(fds_list);
    }
    free_server(server);
    pool_free(pool);
}

void free_server(Server* server) {
    free(server->_socket_addr);
    free(server);
}
