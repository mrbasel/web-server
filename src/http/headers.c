#include "http/response.h"
#include "http/headers.h"

void add_header(struct HttpResponse* response, char* name, char* value) {
    response->headers[response->headers_count++] = (HttpHeader){name, value};
}
