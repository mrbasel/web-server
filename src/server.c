#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "socket.h"

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

void server_listen(Server* server, RequestHandler handler) {
    char buffer[BUFFER_SIZE] = {0};

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server->_socket_fd, server->_socket_addr, buffer, BUFFER_SIZE);
        HttpRequest* parsed_request = parse_request(buffer);
        if (parsed_request == NULL) {
            fprintf(stderr, "could not parse request\n");
            continue;
        }


        HttpResponse* response = create_response(parsed_request); 
        log_http_transaction(parsed_request, response);

        handler(parsed_request, response);
        char* serialized_response = serialize_response(response);
        send(accepted_socket, serialized_response, strlen(serialized_response), 0);

        free_request(parsed_request);
        close(accepted_socket);
    }
    free_server(server);
}

void free_server(Server* server) {
    free(server->_socket_addr);
    free(server);
}
