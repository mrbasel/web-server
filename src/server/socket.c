#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "socket.h"

struct sockaddr_in* create_server_sockaddr(int port) {
    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    if (address == NULL) {
        fprintf(stderr, "Error: failed to create server sockaddr\n");
        return NULL;
    }
    bzero(address, sizeof(struct sockaddr_in)); 
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
    return address;
}

int create_server_socket(struct sockaddr_in* address) {
    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Error: failed to create server listener socket\n");
        return -1;
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                &opt, sizeof(opt))) {
        fprintf(stderr, "Error: failed to create server listener socket\n");
        return -1;
    }


    // bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)address, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Error: failed to bind server listener socket\n");
        return -1;
    }

    // listen for incoming connections (passive mode)
    if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        fprintf(stderr, "Error: failed to listen on server socket\n");
        return -1;
    }
    return server_fd;
}

int accept_connection(int server_fd) {
    int accepted_socket = accept(server_fd, NULL, NULL);

    return accepted_socket;
}
