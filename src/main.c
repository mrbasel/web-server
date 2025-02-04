#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"
#include "http.h"

#define PORT 8000
#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in* address = create_server_sockaddr(PORT);
    int server_fd = create_server_socket(address);

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server_fd, address, buffer, BUFFER_SIZE);
        HttpRequest* data = parse_request(buffer);

        printf("method: %s\n", parsed_request->method);
        printf("uri: %s\n", parsed_request->uri);
        printf("version: %s\n", parsed_request->version);
        
        for (int i = 0; i < parsed_request->headers_count; i++) printf("%s: %s", parsed_request->headers[i].name, parsed_request->headers[i].value);
        printf("\n");

        char response[100] = "HTTP/1.0 200 OK\r\n\r\nRequested path: ";
        strcat(response, parsed_request->uri);
        send(accepted_socket, response, strlen(response), 0);

        free_request(parsed_request);
        close(accepted_socket);
    }
    free(address);

    return 0;
}
