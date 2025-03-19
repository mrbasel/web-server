#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http/constants.h"
#include "http/headers.h"
#include "http/request.h"
#include "http/response.h"
#include "http/utils.h"
#include "arena/arena.h"

HttpResponse* create_response(HttpRequest* request, RequestHandler handler, Arena* arena) {
    HttpResponse* response = arena_alloc(arena, sizeof(HttpResponse));
    response->headers_count = 0;
    response->body_len = 0;
    if (response == NULL) return NULL;
    HttpHeader* headers = arena_alloc(arena, sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    response->headers = headers;
    if (headers == NULL) return NULL;

    add_header(response, "Server", "Http Server");
    char* date_buffer = arena_alloc(arena, 50);
    get_date_string(date_buffer);
    add_header(response, "Date", date_buffer);
    response->version = "HTTP/1.1";

    response->body = arena_alloc(arena, MAX_RESPONSE_BODY_SIZE);

    handler(request, response);

    if (!request->method || !request->uri || !request->version) {
        response->statusCode = 400;
    } else if (strcmp(request->method, "GET") != 0) {
        strcpy(response->body,  "This method is not supported.");
        response->body_len = strlen(response->body);
        response->statusCode = 501;
    }

    response->reason = get_status_code_reason(response->statusCode);
    
    // set "Content-Length" header when applicable
    if (response->body_len > 0) {
        char* length = arena_alloc(arena, 5);
        sprintf(length, "%d", response->body_len);
        add_header(response, "Content-Length", length);
    }

    return response;
}

char* serialize_response(HttpResponse* response, Arena* arena) {
    char* buffer = arena_alloc(arena, sizeof(char) * MAX_RESPONSE_SIZE);
    char* buffer_ptr = buffer;
    buffer_ptr += sprintf(buffer_ptr, "%s %d %s\r\n", response->version, response->statusCode, response->reason);
    for (int j = 0; j < response->headers_count; j++) {
        HttpHeader header = response->headers[j];
        buffer_ptr += sprintf(buffer_ptr, "%s: %s\r\n", header.name, header.value);
    }
    buffer_ptr += sprintf(buffer_ptr, "\r\n");
    buffer_ptr += sprintf(buffer_ptr, "%s", response->body);

    return buffer;
}

/*
char* fetch_resource(HttpRequest* request, int* body_len) {
    char* resource = malloc(5000);

    char buffer[100];
    FILE* file;
    file = fopen("/var/www/html/index.html", "r");
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        for (int j = 0; buffer[j] != 0; j++) {
            resource[i++] = buffer[j];
        }
    };
    resource[i] = 0;
    *body_len = i;
    return resource;
}
*/

const char* get_status_code_reason(int status_code) {
    switch (status_code) {
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 103: return "Early Hints";

        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status";
        case 208: return "Already Reported";
        case 226: return "IM Used";

        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a teapot";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity";
        case 423: return "Locked";
        case 424: return "Failed Dependency";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";

        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage";
        case 508: return "Loop Detected";
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";

        default: return "Unknown Status Code";
    }
}
