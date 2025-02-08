
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
} HttpResponse;

HttpRequest* parse_request(char* request);

HttpResponse* _create_response(HttpRequest* request);

char* fetch_resource(HttpRequest* request);

char* create_response(HttpRequest* request);

void free_request(HttpRequest* request);

void free_response(HttpResponse* response);
