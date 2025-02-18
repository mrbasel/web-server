struct HttpHeader;
typedef struct HttpHeader HttpHeader;

typedef struct HttpRequest {
    char* method;
    char* uri;
    char* version;
    struct HttpHeader* headers;
    int headers_count;
} HttpRequest;

HttpRequest* parse_request(char* request);

void free_request(HttpRequest* request);
