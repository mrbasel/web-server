#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"

#define PORT 8000
#define BUFFER_SIZE 1024

typedef struct {
    char* method;
    char* uri;
    char* version;
} http_request;

http_request* parseRequest(char* request) {
    http_request* parsed_request = malloc(sizeof(http_request));
    char* token;
    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r");
    return parsed_request;
}

int main() {
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in* address = create_server_sockaddr(PORT);
    int server_fd = create_server_socket(address);

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server_fd, address, buffer, BUFFER_SIZE);
        http_request* data = parseRequest(buffer);

        printf("method: %s\n", data->method);
        printf("uri: %s\n", data->uri);
        printf("version: %s\n", data->version);

        char *response = "HTTP/1.0 200 OK\r\n\r\n";
        send(accepted_socket, response, strlen(response), 0);

        close(accepted_socket);
    }
    free(address);

    return 0;
}
