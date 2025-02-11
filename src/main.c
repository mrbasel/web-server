#include "server.h"

#define PORT 8000

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    return res;
}

int main() {
    Server* server = create_server(PORT); 
    server_listen(server, handler);
}
