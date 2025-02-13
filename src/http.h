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

HttpRequest* parse_request(char* request);

HttpResponse* create_response(HttpRequest* request);

char* serialize_response(HttpResponse* response);

char* fetch_resource(HttpRequest* request, int* body_len);

void add_header(HttpResponse* response, char* name, char* value);

void free_request(HttpRequest* request);

void free_response(HttpResponse* response);

void log_http_transaction(HttpRequest* request, HttpResponse* response);
