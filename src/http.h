
typedef struct {
    char* name;
    char* value;
} http_header;

typedef struct {
    char* method;
    char* uri;
    char* version;
    http_header* headers;
    int headers_count;
} http_request;


http_request* parseRequest(char* request);
