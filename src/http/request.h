struct HttpHeader;
typedef struct HttpHeader HttpHeader;

struct Arena;
typedef struct Arena Arena;

typedef struct HttpRequest {
    char* method;
    char* uri;
    char* version;
    struct HttpHeader* headers;
    int headers_count;
} HttpRequest;

HttpRequest* parse_request(char* request, Arena* arena);
