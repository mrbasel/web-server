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

HttpResponse* _create_response(HttpRequest* request) {
    HttpResponse* response = malloc(sizeof(HttpResponse));
    if (response == NULL) return NULL;
    /*
    HttpHeader* headers = malloc(sizeof(HttpHeader) * MAX_REQUEST_HEADERS_COUNT);
    if (headers == NULL) return NULL;

    headers[0].name = "Server";
    headers[1].value = "http server";
    */

    response->version = "HTTP/1.1";
    if (strcmp(request->method, "GET") != 0) {
        response->statusCode = "501";
        response->reason = "Not Implemented";
        return response;
    }

    if (!strcmp(request->uri, "/") || !strcmp(request->uri, "/index.html")) {
        response->statusCode = "200";
        response->reason = "OK";
        response->body = fetch_resource(request);
        return response;
    }
    response->statusCode = "404";
    response->reason = "Not Found";
    return response;
}

char* create_response(HttpRequest* request) {
    HttpResponse* response = _create_response(request);
    char* buffer = malloc(sizeof(char) * MAX_RESPONSE_SIZE);
    int i = 0;
    for (int j = 0; response->version[j] != 0; j++) buffer[i++] = response->version[j];
    buffer[i++] = ' ';

    for (int j = 0; response->statusCode[j] != 0; j++) buffer[i++] = response->statusCode[j];
    buffer[i++] = ' ';

    for (int j = 0; response->reason[j] != 0; j++) buffer[i++] = response->reason[j];
    buffer[i++] = '\r';
    buffer[i++] = '\n';
    buffer[i++] = '\r';
    buffer[i++] = '\n';

    if (response->statusCode[0] == '2') {
        for (int j = 0; response->body[j] != 0; j++) {
            buffer[i++] = response->body[j];
        }
        free(response->body);
    }

    buffer[i] = 0;
    free_response(response);
    return buffer;
}


char* fetch_resource(HttpRequest* request) {
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
    return resource;
}

void free_request(HttpRequest* request) {
    free(request->headers);
    free(request);
}

void free_response(HttpResponse* response) {
    //free(response->headers);
    free(response);
}
