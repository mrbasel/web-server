
typedef struct {
    char* name;
    char* value;
} http_header;

typedef struct {
    char* method;
    char* uri;
    char* version;
    http_header* headers;
} http_request;


http_request* parseRequest(char* request);
