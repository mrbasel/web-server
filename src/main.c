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
    if (address == NULL) {
        fprintf(stderr, "could not create socket\n"); 
        abort();
    }
    int server_fd = create_server_socket(address);

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server_fd, address, buffer, BUFFER_SIZE);
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

        char* response = create_response(parsed_request); 
        send(accepted_socket, response, strlen(response), 0);

        free_request(parsed_request);
        close(accepted_socket);
    }
    free(address);

    return 0;
}
