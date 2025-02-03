#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"

#define PORT 8000
#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in* address = create_server_sockaddr(PORT);
    int server_fd = create_server_socket(address);

    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server_fd, address, buffer, BUFFER_SIZE);
        printf("Received message: %s\n", buffer);
        char *response = "Hello from server";
        send(accepted_socket, response, strlen(response), 0);
        printf("Response sent\n");

        close(accepted_socket);
    }
    free(address);

    return 0;
}
