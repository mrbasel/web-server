#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>
#include "server/server.h"
#include "socket.h"
#include "http/utils.h"
#include "arena/arena.h"
#include "http/constants.h"

#define BUFFER_SIZE 1024

Server* create_server(int port) {
    Server* server = malloc(sizeof(Server));
    server->port = port;

    struct sockaddr_in* socket_addr = create_server_sockaddr(port);
    int socket_fd = create_server_socket(socket_addr);
    server->_socket_fd = socket_fd;
    server->_socket_addr = socket_addr;
    return server;
}

typedef struct {
    char buffer[BUFFER_SIZE];
    int socket;
    RequestHandler handler;
} RequestArgs;

int handle_request(void* arg) {
    Arena* arena = arena_init(ARENA_SIZE);
    RequestArgs* args = (RequestArgs*)arg;
    char* buffer = args->buffer;
    int socket = args->socket;
    RequestHandler handler = args->handler;

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

    arena_free(arena);
    close(socket);
    free(args);
    return 0;
}

void server_listen(Server* server, RequestHandler handler) {
    char buffer[BUFFER_SIZE] = {0};
    int accepted_socket;
    thrd_t t;
    while(1) {
        accepted_socket = accept_connection(server->_socket_fd, server->_socket_addr, buffer, BUFFER_SIZE);
        RequestArgs* args = malloc(sizeof(RequestArgs));
        memcpy(args->buffer, buffer, BUFFER_SIZE);
        args->socket = accepted_socket;
        args->handler = handler;
        thrd_create(&t, handle_request, args);
        thrd_detach(t);
    }
    free_server(server);
}

void free_server(Server* server) {
    free(server->_socket_addr);
    free(server);
}
