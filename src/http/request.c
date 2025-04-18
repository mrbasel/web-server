#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http/constants.h"
#include "http/request.h"
#include "http/headers.h"
#include "arena/arena.h"

HttpRequest* parse_request(char* request, Arena* arena) {
    HttpRequest* parsed_request = arena_alloc(arena, sizeof(HttpRequest));
    if (parsed_request == NULL) return NULL;
    parsed_request->_is_valid = 1;
    parsed_request->headers_count = 0;

    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r");
    if (request != NULL) request++; // skip newline

    // validate request line
    if (!parsed_request->method || !parsed_request->uri || !parsed_request->version) parsed_request->_is_valid = 0;
    if (parsed_request->uri && parsed_request->uri[0] != '/') parsed_request->_is_valid = 0; // TODO: validate URI properly
    if (parsed_request->version && (strcmp(parsed_request->version, "HTTP/1.0") != 0 && strcmp(parsed_request->version, "HTTP/1.1") != 0)) parsed_request->_is_valid = 0;

    if (!parsed_request->_is_valid) return parsed_request;

    HttpHeader* headers = arena_alloc(arena, sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
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
