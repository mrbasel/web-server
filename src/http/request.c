#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http/constants.h"
#include "http/request.h"
#include "http/headers.h"

HttpRequest* parse_request(char* request) {
    HttpRequest* parsed_request = malloc(sizeof(HttpRequest));
    if (parsed_request == NULL) return NULL;

    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r");
    if (request != NULL) request++; // skip newline

    HttpHeader* headers = malloc(sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    if (headers == NULL) return NULL;
    parsed_request->headers = headers;

    // parse request headers
    int i = 0;
    while (1) {
        char* name = strsep(&request, ":");
        if (request != NULL && *request == ' ') request++; // skip whitespace
        char* value = strsep(&request, "\r\n");

        if (!name || !value) break;
        headers[i].name = name;
        headers[i].value = value;
        i++;
    }
    parsed_request->headers_count = i;
    return parsed_request;
}

void free_request(HttpRequest* request) {
    free(request->headers);
    free(request);
}


