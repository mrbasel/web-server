#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http/constants.h"
#include "http/headers.h"
#include "http/request.h"
#include "http/response.h"
#include "http/utils.h"

HttpResponse* create_response(HttpRequest* request) {
    HttpResponse* response = malloc(sizeof(HttpResponse));
    response->headers_count = 0;
    response->body_len = 0;
    if (response == NULL) return NULL;
    HttpHeader* headers = malloc(sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    response->headers = headers;
    if (headers == NULL) return NULL;

    add_header(response, strdup("Server"), strdup("Http Server"));
    char date_buffer[50];
    get_date_string(date_buffer);
    add_header(response, strdup("Date"), strdup(date_buffer));

    response->version = "HTTP/1.1";

    if (!request->method || !request->uri || !request->version) {
        response->statusCode = "400";
        response->reason = "Bad Request";
        return response;
    }

    if (strcmp(request->method, "GET") != 0) {
        response->statusCode = "501";
        response->reason = "Not Implemented";
        return response;
    }

    if (!strcmp(request->uri, "/") || !strcmp(request->uri, "/index.html")) {
        response->statusCode = "200";
        response->reason = "OK";
        response->body = strdup("Hello, World!");
        response->body_len = strlen(response->body);
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

void free_response(HttpResponse* response) {
    for (int i = 0; i < response->headers_count; i++) {
        free(response->headers[i].name);
        free(response->headers[i].value);
}
    free(response->headers);
    if (response->body_len > 0) free(response->body);
    free(response);
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


