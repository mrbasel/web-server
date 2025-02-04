#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

http_request* parseRequest(char* request) {
    http_request* parsed_request = malloc(sizeof(http_request));
    char* token;
    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r");
    return parsed_request;
}
