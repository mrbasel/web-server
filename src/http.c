#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

#define MAX_REQUEST_HEADERS_COUNT 100
#define MAX_RESPONSE_SIZE 10000

HttpRequest* parse_request(char* request) {
    HttpRequest* parsed_request = malloc(sizeof(HttpRequest));
    if (parsed_request == NULL) return NULL;

    parsed_request->method = strsep(&request, " ");
    parsed_request->uri = strsep(&request, " ");
    parsed_request->version = strsep(&request, "\r\n");

    HttpHeader* headers = malloc(sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    if (headers == NULL) return NULL;
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

HttpResponse* create_response(HttpRequest* request) {
    HttpResponse* response = malloc(sizeof(HttpResponse));
    response->headers_count = 0;
    response->body_len = 0;
    if (response == NULL) return NULL;
    HttpHeader* headers = malloc(sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    response->headers = headers;
    if (headers == NULL) return NULL;

    add_header(response, "Server", "Http Server");
    add_header(response, "Date", "dunno");

    response->version = "HTTP/1.1";
    if (strcmp(request->method, "GET") != 0) {
        response->statusCode = "501";
        response->reason = "Not Implemented";
        return response;
    }

    if (!strcmp(request->uri, "/") || !strcmp(request->uri, "/index.html")) {
        response->statusCode = "200";
        response->reason = "OK";
        int body_len = 0;
        response->body = fetch_resource(request, &body_len);
        response->body_len = body_len;
        return response;
    }
    response->statusCode = "404";
    response->reason = "Not Found";
    return response;
}

char* serialize_response(HttpResponse* response) {
    char* buffer = malloc(sizeof(char) * MAX_RESPONSE_SIZE);
    int i = 0;
    for (int j = 0; response->version[j] != 0; j++) buffer[i++] = response->version[j];
    buffer[i++] = ' ';

    for (int j = 0; response->statusCode[j] != 0; j++) buffer[i++] = response->statusCode[j];
    buffer[i++] = ' ';

    for (int j = 0; response->reason[j] != 0; j++) buffer[i++] = response->reason[j];
    buffer[i++] = '\r';
    buffer[i++] = '\n';

    for (int j = 0; j < response->headers_count; j++) {
        HttpHeader header = response->headers[j];
        int name_len = strlen(header.name);
        int value_len = strlen(header.value);
        strcpy(buffer + i, header.name);
        i += name_len;
        buffer[i++] = ':';
        strcpy(buffer + i, header.value);
        i += value_len;
        buffer[i++] = '\r';
        buffer[i++] = '\n';
    }
    buffer[i++] = '\r';
    buffer[i++] = '\n';

    if (response->statusCode[0] == '2') {
        for (int j = 0; response->body[j] != 0; j++) {
            buffer[i++] = response->body[j];
        }
        buffer[i] = 0;
    }

    free_response(response);
    return buffer;
}


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

void add_header(HttpResponse* response, char* name, char* value) {
    response->headers[response->headers_count++] = (HttpHeader){name, value};
}

void free_request(HttpRequest* request) {
    free(request->headers);
    free(request);
}

void free_response(HttpResponse* response) {
    free(response->headers);
    if (response->body_len > 0) free(response->body);
    free(response);
}

void log_http_transaction(HttpRequest* request, HttpResponse* response) {
    printf("%s %s %s %s\n", request->method, request->uri, request->version, response->statusCode);
}
