#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct sockaddr_in* create_server_sockaddr(int port) {
    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    bzero(address, sizeof(struct sockaddr_in)); 
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
    return address;
}

int create_server_socket(struct sockaddr_in* address) {
    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)address, sizeof(struct sockaddr_in)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections (passive mode)
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening at %d\n", PORT);
    return server_fd;
}

int accept_connection(int server_fd, struct sockaddr_in* address, char* buffer) {
    int addrlen = sizeof(*address);
    int accepted_socket;
    if ((accepted_socket = accept(server_fd, (struct sockaddr *)address,
                    (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("New client connected\n");

    int valread = read(accepted_socket, buffer, BUFFER_SIZE);
    
    return accepted_socket;
}

int main() {
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in* address = create_server_sockaddr(PORT);
    int server_fd = create_server_socket(address);
    
    int accepted_socket;
    while(1) {
        accepted_socket = accept_connection(server_fd, address, buffer);        
        printf("Received message: %s\n", buffer);
        char *response = "Hello from server";
        send(accepted_socket, response, strlen(response), 0);
        printf("Response sent\n");
        
        close(accepted_socket);
    }
    free(address);
    
    return 0;
}
