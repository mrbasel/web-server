#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include <poll.h>
#include <sys/eventfd.h>
#include "server/server.h"
#include "server/fds_list.h"
#include "socket.h"
#include "http/utils.h"
#include "arena/arena.h"
#include "http/constants.h"
#include "pool/pool.h"
#include "server/thread_safe_queue.h"

static int handle_request(void* arg) {
    Arena* arena = arena_init(ARENA_SIZE);
    RequestArgs* args = (RequestArgs*)arg;
    if (arena == NULL) {
        fprintf(stderr, "Error: failed to create arena\n");
        arena_free(arena);
        free(args);
        return 1;
    }

    int socket = args->socket;
    int efd = args->efd;
    RequestHandler handler = args->handler;
    char* buffer = args->buffer;
    ThreadSafeQueue* closed_fds_queue = args->closed_fds_queue;

    HttpRequest* parsed_request = parse_request(buffer, arena);
    if (parsed_request == NULL) {
        free(args);
        fprintf(stderr, "Error: could not parse request\n");
        arena_free(arena);
        free(args);
        return 1;
    }

    HttpResponse* response = create_response(parsed_request, handler, arena);
    if (response == NULL) {
        fprintf(stderr, "Error: could not create response\n");
        arena_free(arena);
        free(args);
        return 1;
    }
    log_http_transaction(socket, parsed_request, response);

    char* serialized_response = serialize_response(response, arena);

    if (strcmp(parsed_request->version, "HTTP/1.0") == 0) {
        uint64_t val = (uint64_t)socket;
        queue_enqueue(closed_fds_queue, socket); // schedule fd to be closed
                                                 // signal to wakeup main thread
        if ((write(efd, &val, sizeof(val))) == -1) fprintf(stderr, "Error: write to efd failed\n");
    }

    send(socket, serialized_response, strlen(serialized_response), 0);
    arena_free(arena);
    free(args);
    return 0;
}

static void delete_expired_sockets(FDS_LIST* fds_list) {
    time_t current_time = time(NULL);
    for (int i = 2; i < fds_list->size; i++) {
        if ((current_time - fds_list->fd_timestamps[i]) > 5) {
            close(fds_list->array[i].fd);
            fds_list_delete(fds_list, i);
        }
    }
}

static void close_queue_sockets(ThreadSafeQueue* closed_fds_queue, FDS_LIST* fds_list) {
    int capacity = queue_get_capacity(closed_fds_queue);

    // TODO: refactor
    while (capacity-- != 0) {
        int fd = queue_dequeue(closed_fds_queue);
        for (int i = 2; i < fds_list->size; i++) {
            if (fds_list->array[i].fd == fd) {
                close(fd);
                fds_list_delete(fds_list, i);
            }
        }
    };
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
    ThreadSafeQueue* closed_fds_queue = queue_init();
    
    // add listener socket to fds array
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server->_socket_fd;
    server_poll_fd.events = POLLIN;
    fds_list_insert(fds_list, server_poll_fd);

    int efd = eventfd(0, EFD_CLOEXEC);
    if (efd == -1) {
        fprintf(stderr, "Error: could not create event fd\n");
        return;
    }

    // add event fd to fds array
    struct pollfd event_poll_fd;
    event_poll_fd.fd = efd;
    event_poll_fd.events = POLLIN;
    fds_list_insert(fds_list, event_poll_fd);

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
                            if (fds_list->size >= MAX_SOCKETS_COUNT) continue;
                            accepted_socket = accept_connection(server->_socket_fd);
                            if (accepted_socket > 0) {
                                struct pollfd poll_fd;
                                poll_fd.fd = accepted_socket;
                                poll_fd.events = POLLIN;
                                fds_list_insert(fds_list, poll_fd);
                            }
                        } else if (fds_list->array[i].fd == efd) {
                            uint64_t u;
                            read(efd, &u, sizeof(u)); // read wakeup signal
                        }
                        else {
                            RequestArgs* args = malloc(sizeof(RequestArgs));
                            if (args == NULL) {
                                fprintf(stderr, "Error: could not process request\n");
                                continue;
                            }
                            args->socket = fds_list->array[i].fd;
                            args->efd = efd;
                            args->handler = handler;
                            args->closed_fds_queue = closed_fds_queue;

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

        close_queue_sockets(closed_fds_queue, fds_list);
        delete_expired_sockets(fds_list);
        fds_purge(fds_list);
    }
    fds_list_free(fds_list);
    free_server(server);
    pool_free(pool);
}
