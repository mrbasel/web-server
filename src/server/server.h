#include "http/request.h"
#include "http/response.h"
#include "http/constants.h"

#define BUFFER_SIZE 1024

struct HttpRequest;
typedef struct HttpRequest HttpRequest;

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

struct FDS_LIST;
typedef struct FDS_LIST FDS_LIST;

typedef struct {
   int _socket_fd; 
   struct sockaddr_in* _socket_addr;
   int port;
} Server;

typedef struct RequestArgs {
    char buffer[BUFFER_SIZE];
    int socket;
    RequestHandler handler;
} RequestArgs;

void free_server(Server* server);

Server* create_server(int port);

typedef HttpResponse* (*RequestHandler)(HttpRequest*, HttpResponse*);

void server_listen(Server* server, RequestHandler request_handler);
