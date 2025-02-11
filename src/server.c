#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "http.h"
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

void server_listen(Server* server, onRequest on_request) {
    char buffer[BUFFER_SIZE] = {0};

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server->_socket_fd, server->_socket_addr, buffer, BUFFER_SIZE);
        HttpRequest* parsed_request = parse_request(buffer);
        if (parse_request == NULL) {
            fprintf(stderr, "could not parse request\n");
            continue;
        }

        printf("method: %s\n", parsed_request->method);
        printf("uri: %s\n", parsed_request->uri);
        printf("version: %s\n", parsed_request->version);

        for (int i = 0; i < parsed_request->headers_count; i++) printf("%s: %s", parsed_request->headers[i].name, parsed_request->headers[i].value);
        printf("\n");

        HttpResponse* response = create_response(parsed_request); 
        on_request(parsed_request, response);
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
