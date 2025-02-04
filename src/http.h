
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


HttpRequest* parse_request(char* request);

void free_request(HttpRequest* request);
