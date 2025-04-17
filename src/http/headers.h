#ifndef HEADERS_H
#define HEADERS_H

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

typedef struct HttpHeader {
    char* name;
    char* value;
} HttpHeader;

void add_header(struct HttpResponse* response, char* name, char* value);

#endif
