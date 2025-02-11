
typedef struct {
   int _socket_fd; 
   struct sockaddr_in* _socket_addr;
   int port;
} Server;



void free_server(Server* server);

typedef struct {
    char* name;
    char* value;
} HttpHeader;

typedef struct {
    char* method;
    char* uri;
    char* version;
    HttpHeader* headers;
    int headers_count;
} HttpRequest;

typedef struct {
    char* version;
    char* statusCode;
    char* reason;
    HttpHeader* headers;
    int headers_count;
    char* body;
    int body_len;
} HttpResponse;

typedef HttpResponse* (*onRequest)(HttpRequest*, HttpResponse*);

Server* create_server(int port);

void server_listen(Server* server, onRequest on_request);

HttpRequest* parse_request(char* request);

HttpResponse* create_response(HttpRequest* request);

char* serialize_response(HttpResponse* response);

char* fetch_resource(HttpRequest* request, int* body_len);

void add_header(HttpResponse* response, char* name, char* value);

void free_request(HttpRequest* request);

void free_response(HttpResponse* response);
