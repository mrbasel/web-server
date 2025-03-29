#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nadel.h"

#define DEFAULT_PORT 8000

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    if (strcmp(req->method, "GET") == 0) {
        strcpy(res->body, "Hello, World!\n");
        res->body_len = strlen(res->body);
        res->statusCode = 200;
    }
    return res;
}

int main(int argc, char* argv[]) {
    size_t workers = 4;
    int port = DEFAULT_PORT;

    if (argc > 1 && (strtol(argv[1], NULL, 10)) != 0) {
        port = strtol(argv[1], NULL, 10);
    }
    if (argc > 2 && (strtol(argv[2], NULL, 10)) != 0) {
        workers = strtol(argv[2], NULL, 10);
    }
    Server* server = create_server(port);
    printf("Server listening at %d with %ld workers\n", server->port, workers);
    server_listen(server, handler, 4);
}
