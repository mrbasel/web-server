#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket.h"
#include "http.h"

#define PORT 8000
#define BUFFER_SIZE 1024

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    return res;
}

int main() {
    Server* server = create_server(PORT); 
    server_listen(server, handler);
}
