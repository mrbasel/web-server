#ifndef RESPONSE_H
#define RESPONSE_H

struct HttpRequest;
typedef struct HttpRequest HttpRequest;

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

struct HttpHeader;
typedef struct HttpHeader HttpHeader;

struct Arena;
typedef struct Arena Arena;

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

HttpResponse* create_response(HttpRequest* request, RequestHandler handler, Arena* arena);

char* serialize_response(HttpResponse* response, Arena* arena);

char* fetch_resource(struct HttpRequest* request, int* body_len);

const char* get_status_code_reason(int status_code);

#endif
