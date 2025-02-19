struct HttpRequest;
typedef struct HttpRequest HttpRequest;
struct HttpResponse;
typedef struct HttpResponse HttpResponse;
struct HttpHeader;
typedef struct HttpHeader HttpHeader;

typedef HttpResponse* (*RequestHandler)(HttpRequest*, HttpResponse*);

typedef struct HttpResponse{
    char* version;
    int statusCode;
    const char* reason;
    struct HttpHeader* headers;
    int headers_count;
    char* body;
    int body_len;
} HttpResponse;

HttpResponse* create_response(HttpRequest* request, RequestHandler handler);

char* serialize_response(HttpResponse* response);

char* fetch_resource(struct HttpRequest* request, int* body_len);

void free_response(HttpResponse* response);

const char* get_status_code_reason(int status_code);
