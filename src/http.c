#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

#define MAX_REQUEST_HEADERS_COUNT 100

http_request* parseRequest(char* request) {
    http_request* parsed_request = malloc(sizeof(http_request));
    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r\n");

    http_header* headers = malloc(sizeof(http_header) * MAX_REQUEST_HEADERS_COUNT);
    parsed_request->headers = headers;

    // parse request headers
    int i = 0;
    while (1) {
        char* name = strsep(&request, ":");
        char* value = strsep(&request, "\r\n");
        if (!name || !value) break;
        headers[i].name = name;
        headers[i].value = value;
        i++;
    }
    parsed_request->headers_count = i;
    return parsed_request;
}
