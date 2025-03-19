#include <string.h>
#include "server/server.h"

#define PORT 8000

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    if (strcmp(req->method, "GET") == 0) {
        strcpy(res->body, "Hello, World!\n");
        res->body_len = strlen(res->body);
        res->statusCode = 200;
    }
    return res;
}

int main() {
    Server* server = create_server(PORT); 
    server_listen(server, handler);
}
