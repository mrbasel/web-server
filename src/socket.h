#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct sockaddr_in* create_server_sockaddr(int port);

int create_server_socket(struct sockaddr_in* address);

int accept_connection(int server_fd, struct sockaddr_in* address, char* buffer, int n);
