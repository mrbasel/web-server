#include "http/request.h"
#include "http/response.h"

struct HttpRequest;
typedef struct HttpRequest HttpRequest;

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

typedef struct {
   int _socket_fd; 
   struct sockaddr_in* _socket_addr;
   int port;
} Server;

void free_server(Server* server);

Server* create_server(int port);

typedef HttpResponse* (*RequestHandler)(HttpRequest*, HttpResponse*);

void server_listen(Server* server, RequestHandler request_handler);
