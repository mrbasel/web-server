#include "server/server.h"
#include <string.h>

#define PORT 8000

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    res->body = strdup("Hello, World!");
    res->body_len = strlen(res->body);
    res->statusCode = 200;
    return res;
}

int main() {
    Server* server = create_server(PORT); 
    server_listen(server, handler);
}
