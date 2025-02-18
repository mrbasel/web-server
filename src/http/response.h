struct HttpRequest;
typedef struct HttpRequest HttpRequest;
struct HttpResponse;
typedef struct HttpResponse HttpResponse;
struct HttpHeader;
typedef struct HttpHeader HttpHeader;

typedef struct HttpResponse{
    char* version;
    char* statusCode;
    char* reason;
    struct HttpHeader* headers;
    int headers_count;
    char* body;
    int body_len;
} HttpResponse;

HttpResponse* create_response(struct HttpRequest* request);

char* serialize_response(HttpResponse* response);

char* fetch_resource(struct HttpRequest* request, int* body_len);

void free_response(HttpResponse* response);
